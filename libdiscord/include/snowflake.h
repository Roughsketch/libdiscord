#pragma once

#include <cstdint>
#include <string>

namespace discord
{
  class snowflake
  {
    uint64_t m_id;
  public:
    snowflake() : m_id(0) {};
    snowflake(uint64_t id) : m_id(id) {};

    explicit snowflake(const std::string s)
    {
      m_id = std::stoull(s);
    }

    bool operator==(const snowflake& rhs) const
    {
      return m_id == rhs.m_id;
    }

    bool operator!=(const snowflake& rhs) const
    {
      return m_id != rhs.m_id;
    }

    bool operator<(const snowflake& rhs) const
    {
      return m_id < rhs.m_id;
    }

    operator uint64_t() const
    {
      return m_id;
    }

    std::string to_string() const
    {
      return std::to_string(m_id);
    }

    uint64_t id() const
    {
      return m_id;
    }
  };
}