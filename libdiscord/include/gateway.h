#pragma once

#include <cstdint>
#include <cpprest/ws_client.h>
#include <thread>

#include "common.h"

namespace discord
{
  struct BotData;
  class Bot;

  class Gateway
  {
    //  Client variables
    std::string m_token;
    utility::string_t m_wss_url;
    web::websockets::client::websocket_callback_client m_client;
    std::mutex m_client_mutex;

    //  Heartbeat variables
    std::thread m_heartbeat_thread;
    uint32_t m_heartbeat_interval;
    bool m_recieved_ack;

    //  Session variables
    uint32_t m_last_seq;
    std::string m_session_id;
    volatile bool m_connected;
    bool m_use_resume;
    int m_shard;
    int m_total_shards;

    std::function<void(std::string, rapidjson::Value&)> m_on_dispatch = nullptr;

    //  Private enumeration for Opcodes
    enum Opcode : uint8_t
    {
      Dispatch = 0,
      Heartbeat,
      Identify,
      Presence,
      Voice_State,
      Voice_Ping,
      Resume,
      Reconnect,
      Request_Members,
      Invalidate_Session,
      Hello,
      Heartbeat_ACK
    };

    void connect();
    void on_message(web::websockets::client::websocket_incoming_message msg);
    void handle_dispatch_event(std::string event_name, rapidjson::Value& data);
    void send(Opcode op, rapidjson::Value& packet);
    void send_heartbeat();
    void send_identify();
    void send_resume();
  public:
    //  Constants
    static const uint8_t LARGE_SERVER;
    static const utility::string_t VERSION;
    static const utility::string_t ENCODING;

    explicit Gateway(utility::string_t wss_url, const std::string& token, int shard = 0, int total_shards = 1);

    void start();
    void on_dispatch(std::function<void(std::string, rapidjson::Value&)> callback);
    bool connected() const;
  };
}
