#include <discord.h>

#include "getRSS.h"
#include "json.hpp"
#include <mutex>

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

  bot.on_ready([]()
  {
    std::cout << "Ready.\n";
  });

  bot.add_command("info", [&bot](auto event)
  {
    event.respond("I am " + bot.profile().distinct() + "(" + bot.profile().id().to_string() + ")");
  });

  bot.add_command("sleep", [](auto event)
  {
    event.channel().start_typing();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    event.respond("Done sleeping.");
  });

  bot.add_command("create", [](auto event)
  {
    std::istringstream iss(event.content());
    std::vector<std::string> words;

    std::copy(std::istream_iterator<std::string>(iss),
              std::istream_iterator<std::string>(),
              std::back_inserter(words));

    if (words.size() >= 2)
    {
      auto channel_name = words[1];

      discord::guild guild = event.guild();
      discord::channel channel = guild.create_text_channel(channel_name).get();
      channel.modify(channel.name(), "This is a newly created channel.");
    }
  });

  bot.add_command("remove", [](auto event)
  {
    std::istringstream iss(event.content());
    std::vector<std::string> words;

    std::copy(std::istream_iterator<std::string>(iss),
              std::istream_iterator<std::string>(),
              std::back_inserter(words));

    if (words.size() >= 2)
    {
      auto channel_name = words[1];

      discord::guild guild = event.guild();
      discord::channel channel = guild.find_channel(channel_name);

      if (!channel.empty())
      {
        channel.remove();
      }
      else
      {
        event.respond("Could not find channel with name " + channel_name);
      }
    }
  });

  bot.add_command("embed", [](auto event)
  {
    event.channel().send_embed([](discord::embed &e)
    {
      e.set_title("Example embed");
      e.set_description("This is how to use the send_embed callback.");
      e.add_field("Field1", "You can add any embed object by using the embed mutator methods.");
      e.add_field("Field2", "When this lambda is finished, the embed will be sent.");
    });
  });

  bot.add_command("react", [](auto event)
  {
    discord::emoji shelterfrog;
    bool found = event.guild().find_emoji("shelterfrog", shelterfrog);

    try
    {
      //  Try adding the :thinking: emoji as a reaction.
      event.react("%F0%9F%A4%94");
    }
    catch (discord::unknown_exception&)
    {
      std::cout << "Could not find thinking emoji." << std::endl;
    }

    if (found)
    {
      try
      {
        //  Try adding a custom emoji reaction.
        event.react(shelterfrog);
      }
      catch (discord::unknown_exception&)
      {
        std::cout << "Could not find :shelterfrog: emoji." << std::endl;
      }
    }
    else
    {
      event.respond("Could not find shelterfrog emoji.");
    }
  });

  bot.add_command("mem", [&bot](auto event)
  {
    auto current_mb = std::round(RSS::current() / 1000.0) / 1000.0;
    auto peak_mb = std::round(RSS::peak() / 1000.0) / 1000.0;
    event.respond("```Current memory usage: " + std::to_string(current_mb) + "MB\nPeak memory usage:    " + std::to_string(peak_mb) + "MB```");
  });

  bot.add_command("guilds", [&bot](auto event) {
    auto guilds = bot.guilds();
    auto response = "I am currently in " + std::to_string(guilds.size()) + " guilds. Top guilds by member count:\n```";

    std::sort(std::begin(guilds), std::end(guilds), [](auto a, auto b)
    {
      return a.member_count() > b.member_count();
    });

    for (size_t i = 0; i < 25 && i < guilds.size(); ++i)
    {
      response += guilds[i].name() + ": " + std::to_string(guilds[i].member_count()) + "\n";
    }

    response += "```";

    event.respond(response);
  });

  bot.run();
}