#include <discord.h>

#include "getRSS.h"
#include "json.hpp"

nlohmann::json read_json_file(std::string file)
{
  std::ifstream ifs(file);
  nlohmann::json json;

  try
  {
    if (ifs.is_open())
    {
      ifs >> json;
    }
  }
  catch (std::invalid_argument)
  {
    //  Do nothing and keep the json data empty.
  }

  return json;
}

int main()
{
  auto settings = read_json_file("settings.json");

  discord::bot bot(settings["token"], settings["prefix"]);

  bot.add_command("info", [&bot](auto& event)
  {
    event.respond("I am " + bot.profile().distinct() + "(" + bot.profile().id().to_string() + ")");
  });

  bot.add_command("mem", [&bot](auto& event)
  {
    auto current_mb = std::round(RSS::current() / 1000.0) / 1000.0;
    auto peak_mb = std::round(RSS::peak() / 1000.0) / 1000.0;
    event.respond("```Current memory usage: " + std::to_string(current_mb) + "MB\nPeak memory usage:    " + std::to_string(peak_mb) + "MB```");
  });

  bot.run();
}