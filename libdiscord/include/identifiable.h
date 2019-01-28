#pragma once

#include "snowflake.h"

namespace discord
{
  class Identifiable
  {
  protected:
    Snowflake m_id;
  public:
    Identifiable() : m_id(0) {}
    explicit Identifiable(Snowflake id) : m_id(id) {}
    explicit Identifiable(rapidjson::Value& value)
    {
      m_id = Snowflake(value.GetString());
    }

    bool operator<(const Identifiable& rhs) const
    {
      return m_id < rhs.m_id;
    }

    explicit operator Snowflake() const
    {
      return m_id;
    }

    void set_id(Snowflake id)
    {
      m_id = id;
    }

    Snowflake id() const
    {
      return m_id;
    }
  };
}
