#pragma once

#include <exception>
#include <string>

namespace discord
{
  /** Used for general Discord exceptions. */
  class discord_exception : public std::exception
  {
  protected:
    std::string m_msg;

  public:
    explicit discord_exception(const char* message) : m_msg(message) {}

    explicit discord_exception(const std::string& message) : m_msg(message) {}

    virtual ~discord_exception() throw () {}

    virtual const char* what() const throw () override {
      return m_msg.c_str();
    }
  };

  /** Used when the API responds with an error code denoting an unknown entity. */
  class unknown_exception : public discord_exception
  {
  public:
    explicit unknown_exception(const char* message) : discord_exception(message) {}

    explicit unknown_exception(const std::string& message) : discord_exception(message) {}

    virtual ~unknown_exception() throw () {}

    virtual const char* what() const throw () override {
      return m_msg.c_str();
    }
  };

  /** Used when the API responds with a permissions related error code. */
  class permission_exception : public discord_exception
  {
  protected:
    std::string m_msg;

  public:
    explicit permission_exception(const char* message) : discord_exception(message) {}

    explicit permission_exception(const std::string& message) : discord_exception(message) {}

    virtual ~permission_exception() throw () {}

    virtual const char* what() const throw () override {
      return m_msg.c_str();
    }
  };

  /** Used when the API responds with an authorization related error code. */
  class authorization_exception : public discord_exception
  {
  protected:
    std::string m_msg;

  public:
    explicit authorization_exception(const char* message) : discord_exception(message) {}

    explicit authorization_exception(const std::string& message) : discord_exception(message) {}

    virtual ~authorization_exception() throw () {}

    virtual const char* what() const throw () override {
      return m_msg.c_str();
    }
  };

  /** Used when the API responds with an embed related error code. */
  class embed_exception : public discord_exception
  {
  protected:
    std::string m_msg;

  public:
    explicit embed_exception(const char* message) : discord_exception(message) {}

    explicit embed_exception(const std::string& message) : discord_exception(message) {}

    virtual ~embed_exception() throw () {}

    virtual const char* what() const throw () override {
      return m_msg.c_str();
    }
  };

  /** Used when the API responds with a "Too Many X" related error code. */
  class too_many_exception : public discord_exception
  {
  protected:
    std::string m_msg;

  public:
    explicit too_many_exception(const char* message) : discord_exception(message) {}

    explicit too_many_exception(const std::string& message) : discord_exception(message) {}

    virtual ~too_many_exception() throw () {}

    virtual const char* what() const throw () override {
      return m_msg.c_str();
    }
  };
}