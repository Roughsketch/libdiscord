#include <discord.h>

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

  auto bot = discord::bot(settings["token"], settings["prefix"]);

  bot.add_command("info", [](auto& event)
  {
    event.respond("I am " + bot.profile().distinct() + "(" + bot.profile().id().to_string() + ")");
  });

  bot.run();
}