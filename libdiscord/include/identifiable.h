#pragma once

#include "snowflake.h"

namespace discord
{
  class identifiable
  {
  protected:
    snowflake m_id;
  public:
    identifiable() : m_id(0) {}
    explicit identifiable(snowflake id) : m_id(id) {}

    bool operator<(const identifiable& rhs) const
    {
      return m_id < rhs.m_id;
    }

    explicit operator snowflake() const
    {
      return m_id;
    }

    void set_id(snowflake id)
    {
      m_id = id;
    }

    snowflake id() const
    {
      return m_id;
    }
  };
}
