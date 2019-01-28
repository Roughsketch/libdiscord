#pragma once

#include "common.h"

namespace discord
{
  class Serializable
  {
  public:
    virtual ~Serializable() {}
    virtual void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) const = 0;
  };
}