#pragma once

#include "common.h"

namespace discord
{
  class serializable
  {
  public:
    virtual ~serializable() {}
    virtual void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) const = 0;
  };
}