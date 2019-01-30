This library is no longer being actively maintained, but pull requests will still be reviewed.

- [Getting Started](#getting-started)
  - [Compiling libdiscord for Windows](#compiling-libdiscord-for-windows)
  - [Compiling a bot on Windows](#compiling-a-bot-on-windows)
  - [Compiling libdiscord for Linux](#compiling-libdiscord-for-linux)
  - [Compiling a bot on Linux](#compiling-a-bot-on-linux)
- [Examples](#examples)
  - [Handling OnMessage](#handling-onmessage)
  - [Creating a Command](#creating-a-command)
  - [Respond as a Stream](#respond-as-a-stream)
  
## Getting Started

### Compiling libdiscord for Windows

You should be able to simply download this git repository and open up the .sln file and build it after installing some dependencies. 

The solution was made in Visual Studio 2015 and has the following dependencies:

- **CPPRestSDK** You can install this as a Nuget package.
- **ZLib** You can install this as a Nuget package. 

After installing these dependencies you should be able to compile the libdiscord project.

### Compiling a Bot on Windows
To compile a bot on Windows you must have libdiscord.lib and the libdiscord include directory in your path. If using Visual Studio, simply add them to your project settings. After your project is set up, you must include the `discord.h` header file to access the library.

### Compiling libdiscord for Linux
The git project has a very simple makefile, so you should be able to just use make in the root directory. However, you will need to have CPPRestSDK and its dependencies installed. Visit [this link](https://github.com/Microsoft/cpprestsdk/wiki/How-to-build-for-Linux) to learn how to install it.

Here is a list of all the libraries you must have installed:

- boost_system
- cpprest
- crypto
- pthread
- ssl
- z

The command that you should run is `make && sudo make install`. This will build libdiscord.so into the `lib` directory (Create this if it's missing), and the install command will place the resulting library into `/usr/lib/libdiscord.so`. From there, your programs should be able to compile using this library.

### Compiling a Bot on Linux
This is a bit more involved than Windows simply because I don't know if you can combine shared libraries easily. Assuming you have a project with a single `main.cpp` file, you would compile it like so:

```
g++ main.cpp -Ilibdiscord/include -ldiscord -lboost_system -lcrypto -lssl -lcpprest -lz -lpthread -std=c++14 -o output_file
```

In here, you should be using the `-I` flag to point to the libdiscord includes. After that, you must include `-ldiscord` for this library, and then link it against all its dependencies as well. This is a bit more than I would like for a simple program, so I'll try to look into simplifying it later.

## Examples
### Handling OnMessage
The following bot will respond to any `Ping!` with a `Pong!`

```cpp
#include <discord.h>

//  Use this to avoid using discord:: everywhere
//  using namespace discord;

//  Or use this to shorten it to just ds::
//  namespace ds = discord;

int main() {
  std::string token = "YOUR_TOKEN";
  auto bot = discord::Bot(token);
  
  bot->on_message([](discord::MessageEvent& event) {
    if (event.content() == "Ping!") {
      event.respond("Pong!");
    }
  });
  
  bot->run();
}
```

### Creating a Command
Creating a command is just like handling `on_message`, except it takes a command name parameter.

```cpp
#include <discord.h>

int main() {
  std::string token = "YOUR_TOKEN";
  auto bot = discord::Bot(token, "!"); // ! is the prefix
  
  //  This command will respond "Pong!" to any "!ping"
  bot->add_command("ping", [](discord::MessageEvent& event) {
    event.respond("Pong!");
  });
  
  bot->run();
}
```

### Respond as a Stream
You may also respond to events similar to how you would use `std::cout`. An example output of this command is `Hello Amoo#2681!`.

```cpp
bot->add_command("hello", [](discord::MessageEvent& event) {
  event << "Hello " << event.author()->distinct() << "!";
});
```

You may also queue up several different responses and they will be combined into one single response. This example is another way to write the above.

```cpp
bot->add_command("hello", [](discord::MessageEvent& event) {
  event << "Hello ";
  event << event.author()->distinct();
  event << "!";
  //  Message is sent after lambda is finished.
});
```
