#include <cpprest/http_msg.h>
#include <zlib.h>

#include "api.h"
#include "gateway.h"

namespace discord
{
  const uint8_t Gateway::LARGE_SERVER = 100;
  const utility::string_t Gateway::VERSION = utility::string_t(U("6"));
  const utility::string_t Gateway::ENCODING = utility::string_t(U("json"));

  void Gateway::connect()
  {
    try
    {
      m_client.connect(m_wss_url);
    }
    catch (const std::exception& e)
    {
      LOG(ERROR) << "Could not connect to " << m_wss_url << ": " << e.what();
    }
  }

  void Gateway::on_message(web::websockets::client::websocket_incoming_message msg)
  {
    std::string str;

    //  If the message is binary data, then we need to decompress it using ZLib.
    if (msg.message_type() == web::websockets::client::websocket_message_type::binary_message)
    {
      Concurrency::streams::container_buffer<std::string> strbuf;

      //  Read the entire binary payload and put into a string container
      auto task = msg.body().read_to_end(strbuf).then([strbuf](size_t bytesRead)
      {
        return strbuf.collection();
      });

      z_stream zs;
      memset(&zs, 0, sizeof(zs));

      if (inflateInit(&zs) != Z_OK)
      {
        LOG(ERROR) << "Could not initialize zlib Inflate";
        return;
      }

      std::string compressed = task.get();

      zs.next_in = reinterpret_cast<Bytef *>(const_cast<char *>(compressed.data()));
      zs.avail_in = static_cast<uInt>(compressed.size());

      int ret;
      char buffer[32768];

      do
      {
        zs.next_out = reinterpret_cast<Bytef *>(buffer);
        zs.avail_out = sizeof(buffer);

        ret = inflate(&zs, 0);

        if (str.size() < zs.total_out)
        {
          str.append(buffer, zs.total_out - str.size());
        }
      } while (ret == Z_OK);

      inflateEnd(&zs);

      if (ret != Z_STREAM_END)
      {
        LOG(ERROR) << "Error during zlib decompression: (" << ret << ")";
        return;
      }
    }
    else
    {
      //  If not compressed, just get the string.
      str = msg.extract_string().get();
    }

    //  Parse our payload as JSON.
    rapidjson::Document payload;
    payload.Parse(str.c_str(), str.size());

    if (str.size() > 1000)
    {
      LOG(DEBUG) << "Got WS Payload: " << str.substr(0, 1000);
    }
    else
    {
      LOG(DEBUG) << "Got WS Payload: " << str;
    }

    rapidjson::Value& data = payload["d"]; //  Get the data for the event

    switch (payload["op"].GetInt())
    {
    case Dispatch:
      m_last_seq = payload["s"].GetInt();
      handle_dispatch_event(payload["t"].GetString(), data);
      break;
    case Reconnect:
      send_resume();
      break;
    case Invalidate_Session:
      LOG(INFO) << "Session was invalidated, sending identify packet again.";
      send_identify();
      break;
    case Hello:
      m_connected = true;
      m_heartbeat_interval = data["heartbeat_interval"].GetInt();
      LOG(DEBUG) << "Set heartbeat interval to " << m_heartbeat_interval;

      m_heartbeat_thread = std::thread([&]() {
        while (connected())
        {
          LOG(DEBUG) << "Sending heartbeat.";
          try
          {
            send_heartbeat();
            std::this_thread::sleep_for(std::chrono::milliseconds(m_heartbeat_interval));
          }
          catch (const std::exception& e)
          {
            LOG(ERROR) << "Could not send a heartbeat: " << e.what();
          }
        }

        LOG(DEBUG) << "Disconnected, stopping heartbeats.";
      });

      if (m_use_resume)
      {
        LOG(INFO) << "Connected again, sending Resume packet.";
        send_resume();
      }
      else
      {
        LOG(DEBUG) << "Connected, sending Identify packet.";
        send_identify();
        m_use_resume = true;  //  Next time use Resume
      }
      break;
    case Heartbeat_ACK:
      LOG(TRACE) << "Recieved Heartbeat ACK.";
      m_recieved_ack = true;
      break;
    default:
      LOG(WARNING) << "Unhandled WS Opcode (" << static_cast<int>(payload["op"].GetInt()) << ")";
    }
  }

  void Gateway::handle_dispatch_event(std::string event_name, rapidjson::Value& data)
  {
    if (event_name == "RESUMED")
    {
      LOG(DEBUG) << "Successfully resumed.";
      return;
    }

    if (event_name == "READY")
    {
      LOG(DEBUG) << "Using Gateway version " << data["v"].GetInt();

      //  Save session id so we can restart a session
      m_session_id = data["session_id"].GetString();
    }

    m_on_dispatch(event_name, data);
  }

  void Gateway::send(Opcode op, rapidjson::Value& packet)
  {
    rapidjson::Document payload(rapidjson::kObjectType);

    payload.AddMember("op", op, payload.GetAllocator());
    payload.AddMember("d", packet, payload.GetAllocator());

    auto payload_str = json_to_string(payload);

    web::websockets::client::websocket_outgoing_message msg;
    msg.set_utf8_message(payload_str);

    LOG(DEBUG) << "Sending packet: " << payload_str;

    try
    {
      m_client.send(msg);
    }
    catch (web::websockets::client::websocket_exception& e)
    {
      LOG(ERROR) << "WS Exception: " << e.what();
    }
  }

  void Gateway::send_heartbeat()
  {
    if (!m_recieved_ack)
    {
      LOG(WARNING) << "Did not recieve a heartbeat ACK packet before this heartbeat.";
    }

    LOG(DEBUG) << "Sending heartbeat packet.";

    rapidjson::Document doc(rapidjson::kNumberType);
    doc.SetInt(m_last_seq);
    send(Heartbeat, doc);
    m_recieved_ack = false;
  }

  void Gateway::send_identify()
  {
    LOG(DEBUG) << "Sending identify packet.";

    auto json = R"(
    {
      "token": ")" + m_token + R"(",
      "properties":
      {
        "$os": "windows",
        "$browser": "Discord",
        "$device": "Discord",
        "$referrer": "",
        "$refferring_domain": ""
      },
      "compress": true,
      "large_threshold": )" + std::to_string(LARGE_SERVER) + R"(,
      "shard": [)" + std::to_string(m_shard) + ", " + std::to_string(m_total_shards) + R"(]
    })";

    rapidjson::Document doc;
    doc.Parse(json);
    send(Identify, doc);
  }

  void Gateway::send_resume()
  {
    LOG(DEBUG) << "Sending resume packet.";

    rapidjson::Document payload(rapidjson::kObjectType);
    rapidjson::Value token_value;
    token_value.SetString(m_token.c_str(), m_token.size());
    rapidjson::Value session_value;
    session_value.SetString(m_session_id.c_str(), m_session_id.size());
    rapidjson::Value seq_value(m_last_seq);

    payload.AddMember("token", token_value, payload.GetAllocator());
    payload.AddMember("sesson_id", session_value, payload.GetAllocator());
    payload.AddMember("seq", seq_value, payload.GetAllocator());

    send(Resume, payload);
  }

  Gateway::Gateway(utility::string_t wss_url, const std::string& token, int shard, int total_shards)
    : m_token(token), m_wss_url(wss_url), m_shard(shard), m_total_shards(total_shards)
  {
    m_heartbeat_interval = 0;
    m_recieved_ack = true; // Set true to start because first hearbeat sent doesn't require an ACK.
    m_last_seq = 0;
    m_connected = false;
    m_use_resume = false;
  }

  void Gateway::start()
  {
    m_client.set_message_handler([&](web::websockets::client::websocket_incoming_message msg)
    {
      try
      {
        on_message(msg);
      }
      catch (const std::exception& e)
      {
        LOG(ERROR) << "WebSocket Exception: " << e.what();
      }
    });

    m_client.set_close_handler([&](web::websockets::client::websocket_close_status status, const utility::string_t& reason, const std::error_code& code)
    {
      if (m_connected)
      {
        LOG(ERROR) << "WebSocket connection has closed with reason "
          << utility::conversions::to_utf8string(reason) << " - "
          << code.message() << " (" << code.value() << ")";
        m_connected = false;
        connect();
      }
    });

    connect();
  }

  void Gateway::on_dispatch(std::function<void(std::string, rapidjson::Value&)> callback)
  {
    m_on_dispatch = callback;
  }

  bool Gateway::connected() const
  {
    return m_connected;
  }
}
