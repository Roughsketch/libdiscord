#include <cpprest/http_msg.h>
#include <zlib.h>

#include "api.h"
#include "gateway.h"

namespace discord
{
  const uint8_t gateway::LARGE_SERVER = 100;
  const utility::string_t gateway::VERSION = utility::string_t(U("6"));
  const utility::string_t gateway::ENCODING = utility::string_t(U("json"));

  void gateway::connect()
  {
    m_client.connect(m_wss_url);
  }

  void gateway::on_message(web::websockets::client::websocket_incoming_message msg)
  {
    std::string str;

    //  If the message is binary data, then we need to decompress it using ZLib.
    if (msg.message_type() == web::websockets::client::websocket_message_type::binary_message)
    {
      Concurrency::streams::container_buffer<std::string> strbuf;
      std::string compressed;

      //  Read the entire binary payload and put into a string container
      compressed = msg.body().read_to_end(strbuf).then([strbuf](size_t bytesRead)
      {
        return strbuf.collection();
      }).get();

      z_stream zs;
      memset(&zs, 0, sizeof(zs));

      if (inflateInit(&zs) != Z_OK)
      {
        LOG(ERROR) << "Could not initialize zlib Inflate";
      }

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
      }
    }
    else
    {
      //  If not compressed, just get the string.
      str = msg.extract_string().get();
    }

    //  Parse our payload as JSON.
    rapidjson::Document payload;
    payload.Parse(str.c_str());

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
    case Hello:
      m_connected = true;
      m_heartbeat_interval = data["heartbeat_interval"].GetInt();
      LOG(DEBUG) << "Set heartbeat interval to " << m_heartbeat_interval;

      if (m_use_resume)
      {
        LOG(INFO) << "Connected again, sending Resume packet.";
        send_resume();
      }
      else
      {
        m_heartbeat_thread = std::thread([&]() {
          while (connected())
          {
            send_heartbeat();
            std::this_thread::sleep_for(std::chrono::milliseconds(m_heartbeat_interval));
          }

          LOG(DEBUG) << "Disconnected, stopping heartbeats.";
        });

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

  void gateway::handle_dispatch_event(std::string event_name, rapidjson::Value& data)
  {
    if (event_name == "READY")
    {
      LOG(DEBUG) << "Using gateway version " << data["v"].GetInt();

      //  Save session id so we can restart a session
      m_session_id = data["session_id"].GetString();

      m_on_dispatch(event_name, data);
    }
    else if (event_name == "RESUMED")
    {
      LOG(DEBUG) << "Successfully resumed.";
    }
    else
    {
      handle_dispatch(event_name, data);
    }
  }

  void gateway::send(Opcode op, rapidjson::Value& packet)
  {
    rapidjson::Document payload;

    payload["op"].SetInt(op);
    payload.AddMember("d", packet, payload.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    payload.Accept(writer);

    auto payload_str = buffer.GetString();

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

  void gateway::send_heartbeat()
  {
    if (!m_recieved_ack)
    {
      LOG(WARNING) << "Did not recieve a heartbeat ACK packet before this heartbeat.";
    }

    LOG(DEBUG) << "Sending heartbeat packet.";

    send(Heartbeat, rapidjson::Value(m_last_seq).Move());
    m_recieved_ack = false;
  }

  void gateway::send_identify()
  {
    LOG(DEBUG) << "Sending identify packet.";

    //  TODO: Convert this to RapidJSON.
    /*
    send(Identify,
    {
      { "token", m_bot->token() },
      {
        "properties",
        {
          { "$os", "windows" },
          { "$browser", "Discord" },
          { "$device", "Discord" },
          { "$referrer", "" },
          { "$refferring_domain", "" }
        }
      },
      { "compress", true },
      { "large_threshold", LARGE_SERVER },
      { "shard", nlohmann::json::array({ 0, 1 }) }
    });
    */
  }

  void gateway::send_resume()
  {
    LOG(DEBUG) << "Sending resume packet.";

    rapidjson::Value payload;

    payload["token"].SetString(m_token.c_str(), m_token.size());
    payload["sesson_id"].SetString(m_session_id.c_str(), m_session_id.size());
    payload["seq"].SetInt(m_last_seq);
    send(Resume, payload);
  }

  gateway::gateway(std::string token) : m_token(token)
  {
    m_heartbeat_interval = 0;
    m_recieved_ack = true; // Set true to start because first hearbeat sent doesn't require an ACK.
    m_last_seq = 0;
    m_connected = false;
    m_use_resume = false;
  }

  void gateway::start()
  {
    web::uri_builder builder(U(""));
    builder.append_query(U("v"), VERSION);
    builder.append_query(U("encoding"), ENCODING);

    if (m_wss_url.empty())
    {
      do
      {
        try
        {
          //  Attempt to get the Gateway URL.
          m_wss_url = utility::conversions::to_string_t(api::get_wss_url());
        }
        catch (const web::http::http_exception& e)
        {
          LOG(ERROR) << "Exception getting gateway URL: " << e.what();
          LOG(ERROR) << "Sleeping for 5 seconds and trying again.";
          std::this_thread::sleep_for(std::chrono::seconds(5));
        }
      } while (m_wss_url.empty());  //  Keep trying until we get it.

      m_wss_url += builder.to_string();
    }

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

  void gateway::on_dispatch(std::function<void(std::string, rapidjson::Value&)> callback)
  {
    m_on_dispatch = callback;
  }

  bool gateway::connected() const
  {
    return m_connected;
  }
}
