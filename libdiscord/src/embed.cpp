#include "embed.h"
#include "connection_state.h"

namespace discord
{
  EmbedFooter::EmbedFooter(ConnectionState* owner, rapidjson::Value& data)
  {
    set_from_json(m_text, "text", data);
    set_from_json(m_icon_url, "icon_url", data);
    set_from_json(m_proxy_icon_url, "proxy_icon_url", data);
  }

  EmbedFooter::EmbedFooter(std::string text, std::string icon_url, std::string proxy_icon_url)
    : m_text(text), m_icon_url(icon_url), m_proxy_icon_url(proxy_icon_url)
  {
  }

  std::string EmbedFooter::content() const
  {
    return m_text;
  }

  std::string EmbedFooter::icon_url() const
  {
    return m_icon_url;
  }

  std::string EmbedFooter::proxy_icon_url() const
  {
    return m_proxy_icon_url;
  }

  EmbedImage::EmbedImage(ConnectionState* owner, rapidjson::Value& data)
  {
    set_from_json(m_url, "url", data);
    set_from_json(m_proxy_url, "proxy_url", data);
    set_from_json(m_height, "height", data);
    set_from_json(m_width, "width", data);
  }

  EmbedImage::EmbedImage(std::string url, uint32_t width, uint32_t height, std::string proxy_url)
    : m_url(url), m_proxy_url(proxy_url), m_height(height), m_width(width)
  {
  }

  std::string EmbedImage::url() const
  {
    return m_url;
  }

  std::string EmbedImage::proxy_url() const
  {
    return m_proxy_url;
  }

  uint32_t EmbedImage::width() const
  {
    return m_width;
  }

  uint32_t EmbedImage::height() const
  {
    return m_height;
  }

  EmbedVideo::EmbedVideo(ConnectionState* owner, rapidjson::Value& data)
  {
    set_from_json(m_url, "url", data);
    set_from_json(m_height, "height", data);
    set_from_json(m_width, "width", data);
  }

  EmbedVideo::EmbedVideo(std::string url, uint32_t width, uint32_t height)
    : m_url(url), m_height(height), m_width(width)
  {
  }

  std::string EmbedVideo::url() const
  {
    return m_url;
  }

  uint32_t EmbedVideo::width() const
  {
    return m_width;
  }

  uint32_t EmbedVideo::height() const
  {
    return m_height;
  }

  EmbedProvider::EmbedProvider(ConnectionState* owner, rapidjson::Value& data)
  {
    set_from_json(m_name, "name", data);
    set_from_json(m_url, "url", data);
  }

  EmbedProvider::EmbedProvider(std::string name, std::string url)
    : m_name(name), m_url(url)
  {
  }

  std::string EmbedProvider::name() const
  {
    return m_name;
  }

  std::string EmbedProvider::url() const
  {
    return m_url;
  }

  EmbedAuthor::EmbedAuthor(ConnectionState* owner, rapidjson::Value& data)
  {
    set_from_json(m_name, "name", data);
    set_from_json(m_url, "url", data);
    set_from_json(m_icon_url, "icon_url", data);
    set_from_json(m_proxy_icon_url, "proxy_icon_url", data);
  }

  EmbedAuthor::EmbedAuthor(std::string name, std::string url, std::string icon_url, std::string proxy_icon_url)
    : m_name(name), m_url(url), m_icon_url(icon_url), m_proxy_icon_url(proxy_icon_url)
  {
  }

  std::string EmbedAuthor::name() const
  {
    return m_name;
  }

  std::string EmbedAuthor::url() const
  {
    return m_url;
  }

  std::string EmbedAuthor::icon_url() const
  {
    return m_icon_url;
  }

  std::string EmbedAuthor::proxy_icon_url() const
  {
    return m_proxy_icon_url;
  }

  EmbedField::EmbedField()
  {
    m_inline = false;
  }

  EmbedField::EmbedField(ConnectionState* owner, rapidjson::Value& data)
  {
    set_from_json(m_name, "name", data);
    set_from_json(m_value, "value", data);
    set_from_json(m_inline, "inline", data);
  }

  EmbedField::EmbedField(std::string name, std::string value, bool is_inline)
    : m_name(name), m_value(value), m_inline(is_inline)
  {
  }

  std::string EmbedField::name() const
  {
    return m_name;
  }

  std::string EmbedField::content() const
  {
    return m_value;
  }

  bool EmbedField::is_inline() const
  {
    return m_inline;
  }

  Embed::Embed()
  {
    m_color = 0;
  }

  Embed::Embed(ConnectionState* owner, rapidjson::Value& data)
  {
    set_from_json(m_title, "title", data);
    set_from_json(m_type, "type", data);
    set_from_json(m_description, "description", data);
    set_from_json(m_url, "url", data);
    set_from_json(m_timestamp, "timestamp", data);
    set_from_json(m_color, "color", data);

    auto found = data.FindMember("footer");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_footer = EmbedFooter(owner, data);
    }

    found = data.FindMember("image");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_image = EmbedImage(owner, data);
    }

    found = data.FindMember("thumbnail");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_thumbnail = embed_thumbnail(owner, data);
    }

    found = data.FindMember("video");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_video = EmbedVideo(owner, data);
    }

    found = data.FindMember("provider");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_provider = EmbedProvider(owner, data);
    }

    found = data.FindMember("author");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      m_author = EmbedAuthor(owner, data);
    }

    found = data.FindMember("fields");
    if (found != data.MemberEnd() && !found->value.IsNull())
    {
      for (auto& field : found->value.GetArray())
      {
        m_fields.emplace_back(owner, field);
      }
    }
  }

  void Embed::set_title(std::string title)
  {
    m_title = title;
  }

  void Embed::set_description(std::string description)
  {
    m_description = description;
  }

  void Embed::set_url(std::string url)
  {
    m_url = url;
  }

  void Embed::set_footer(std::string text, std::string icon_url, std::string proxy_icon_url)
  {
    m_footer = EmbedFooter(text, icon_url, proxy_icon_url);
  }

  void Embed::set_image(std::string url)
  {
    m_image = EmbedImage(url);
  }

  void Embed::set_thumbnail(std::string url)
  {
    m_thumbnail = embed_thumbnail(url);
  }

  void Embed::set_video(std::string url)
  {
    m_video = EmbedVideo(url);
  }

  void Embed::set_author(std::string name, std::string url, std::string icon_url)
  {
    m_author = EmbedAuthor(name, url, icon_url);
  }

  void Embed::add_field(std::string name, std::string value, bool is_inline)
  {
    m_fields.push_back(EmbedField(name, value, is_inline));
  }

  std::string Embed::title() const
  {
    return m_title;
  }

  std::string Embed::type() const
  {
    return m_type;
  }

  std::string Embed::description() const
  {
    return m_description;
  }

  std::string Embed::url() const
  {
    return m_url;
  }

  std::string Embed::timestamp() const
  {
    return m_timestamp;
  }

  uint32_t Embed::color() const
  {
    return m_color;
  }

  EmbedFooter& Embed::footer()
  {
    return m_footer;
  }

  EmbedImage& Embed::image()
  {
    return m_image;
  }

  embed_thumbnail& Embed::thumbnail()
  {
    return m_thumbnail;
  }

  EmbedVideo& Embed::video()
  {
    return m_video;
  }

  EmbedProvider& Embed::provider()
  {
    return m_provider;
  }

  EmbedAuthor& Embed::author()
  {
    return m_author;
  }

  std::vector<EmbedField>& Embed::fields()
  {
    return m_fields;
  }

  bool Embed::empty() const
  {
    return  m_title.empty() && m_description.empty() &&
            m_footer.empty() && m_image.empty() &&
            m_thumbnail.empty() && m_video.empty() &&
            m_provider.empty() && m_author.empty() && m_fields.size() == 0;
  }

  /******************************************
  * Start RapidJSON Serialization methods. *
  ******************************************/

  void EmbedFooter::Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
  {
    writer.StartObject();

    writer.String("text");
    writer.String(m_text);

    if (!m_icon_url.empty())
    {
      writer.String("icon_url");
      writer.String(m_icon_url);
    }

    if (!m_proxy_icon_url.empty())
    {
      writer.String("proxy_icon_url");
      writer.String(m_proxy_icon_url);
    }

    writer.EndObject();
  }

  void EmbedImage::Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
  {
    writer.StartObject();

    writer.String("url");
    writer.String(m_url);

    if (m_width > 0)
    {
      writer.String("width");
      writer.Uint(m_width);
    }

    if (m_height > 0)
    {
      writer.String("height");
      writer.Uint(m_height);
    }

    if (!m_proxy_url.empty())
    {
      writer.String("proxy_url");
      writer.String(m_proxy_url);
    }

    writer.EndObject();
  }

  void EmbedVideo::Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
  {
    writer.StartObject();

    writer.String("url");
    writer.String(m_url);

    if (m_width > 0)
    {
      writer.String("width");
      writer.Uint(m_width);
    }

    if (m_height > 0)
    {
      writer.String("height");
      writer.Uint(m_height);
    }

    writer.EndObject();
  }

  void EmbedProvider::Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
  {
    writer.StartObject();

    writer.String("name");
    writer.String(m_name);

    if (!m_url.empty())
    {
      writer.String("url");
      writer.String(m_url);
    }

    writer.EndObject();
  }

  void EmbedAuthor::Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
  {
    writer.StartObject();

    writer.String("name");
    writer.String(m_name);

    if (!m_url.empty())
    {
      writer.String("url");
      writer.String(m_url);
    }

    if (!m_icon_url.empty())
    {
      writer.String("icon_url");
      writer.String(m_icon_url);
    }

    if (!m_proxy_icon_url.empty())
    {
      writer.String("proxy_icon_url");
      writer.String(m_proxy_icon_url);
    }

    writer.EndObject();
  }

  void EmbedField::Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
  {
    writer.StartObject();

    writer.String("name");
    writer.String(m_name);

    writer.String("value");
    writer.String(m_value);

    writer.String("inline");
    writer.Bool(m_inline);

    writer.EndObject();
  }

  void Embed::Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
  {
    writer.StartObject();

    if (!m_title.empty())
    {
      writer.String("title");
      writer.String(m_title);
    }

    if (!m_description.empty())
    {
      writer.String("description");
      writer.String(m_description);
    }

    if (!m_url.empty())
    {
      writer.String("url");
      writer.String(m_url);
    }

    if (m_color != 0)
    {
      writer.String("color");
      writer.Uint(m_color);
    }

    if (!m_footer.empty())
    {
      writer.String("footer");
      m_footer.Serialize(writer);
    }

    if (!m_image.empty())
    {
      writer.String("image");
      m_image.Serialize(writer);
    }

    if (!m_thumbnail.empty())
    {
      writer.String("thumbnail");
      m_thumbnail.Serialize(writer);
    }

    if (!m_video.empty())
    {
      writer.String("video");
      m_video.Serialize(writer);
    }

    if (!m_author.empty())
    {
      writer.String("author");
      m_author.Serialize(writer);
    }

    if (m_fields.size() > 0)
    {
      writer.String("fields");
      writer.StartArray();
      for (const auto& field : m_fields)
      {
        //  Check empty to avoid adding fields that aren't initialized correctly.
        if (!field.empty())
        {
          field.Serialize(writer);
        }
      }
      writer.EndArray();
    }

    writer.EndObject();
  }
}
