#include "common.h"

INITIALIZE_EASYLOGGINGPP

namespace discord
{
  std::string json_to_string(rapidjson::Value& data)
  {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    data.Accept(writer);

    return buffer.GetString();
  }

  std::string json_to_string(const rapidjson::Value& data)
  {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    data.Accept(writer);

    return buffer.GetString();
  }
}
