#pragma once

#include <exception>
#include <string>

namespace discord
{
  /** Used for general Discord exceptions. */
  class DiscordException : public std::exception
  {
  protected:
    std::string m_msg;

  public:
    explicit DiscordException(const char* message) : m_msg(message) {}

    explicit DiscordException(const std::string& message) : m_msg(message) {}

    virtual ~DiscordException() throw () {}

    virtual const char* what() const throw () override {
      return m_msg.c_str();
    }
  };

  /** Used when the API responds with an error code denoting an unknown entity. */
  class UnknownException : public DiscordException
  {
  public:
    explicit UnknownException(const char* message) : DiscordException(message) {}

    explicit UnknownException(const std::string& message) : DiscordException(message) {}

    virtual ~UnknownException() throw () {}

    virtual const char* what() const throw () override {
      return m_msg.c_str();
    }
  };

  /** Used when the API responds with a permissions related error code. */
  class PermissionException : public DiscordException
  {
  protected:
    std::string m_msg;

  public:
    explicit PermissionException(const char* message) : DiscordException(message) {}

    explicit PermissionException(const std::string& message) : DiscordException(message) {}

    virtual ~PermissionException() throw () {}

    virtual const char* what() const throw () override {
      return m_msg.c_str();
    }
  };

  /** Used when the API responds with an authorization related error code. */
  class AuthorizationException : public DiscordException
  {
  protected:
    std::string m_msg;

  public:
    explicit AuthorizationException(const char* message) : DiscordException(message) {}

    explicit AuthorizationException(const std::string& message) : DiscordException(message) {}

    virtual ~AuthorizationException() throw () {}

    virtual const char* what() const throw () override {
      return m_msg.c_str();
    }
  };

  /** Used when the API responds with an Embed related error code. */
  class EmbedException : public DiscordException
  {
  protected:
    std::string m_msg;

  public:
    explicit EmbedException(const char* message) : DiscordException(message) {}

    explicit EmbedException(const std::string& message) : DiscordException(message) {}

    virtual ~EmbedException() throw () {}

    virtual const char* what() const throw () override {
      return m_msg.c_str();
    }
  };

  /** Used when the API responds with a "Too Many X" related error code. */
  class TooManyException : public DiscordException
  {
  protected:
    std::string m_msg;

  public:
    explicit TooManyException(const char* message) : DiscordException(message) {}

    explicit TooManyException(const std::string& message) : DiscordException(message) {}

    virtual ~TooManyException() throw () {}

    virtual const char* what() const throw () override {
      return m_msg.c_str();
    }
  };
}