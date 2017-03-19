#pragma once

#include "common.h"

namespace discord
{
  class connection_state;

  /** The footer object of an embed. */
  class embed_footer
  {
    std::string m_text;
    std::string m_icon_url;
    std::string m_proxy_icon_url;
  public:
    explicit embed_footer(connection_state* owner, rapidjson::Value& data);
    explicit embed_footer(std::string text = "", std::string icon_url = "", std::string proxy_icon_url = "");

    template <typename Writer>
    void Serialize(Writer& writer) const;

    /** Get the text from this footer.
     *
     * @return The text of the footer.
     */
    std::string content() const;

    /** Get the icon url.
     *
     * @return The icon url.
     */
    std::string icon_url() const;

    /** Get the proxy icon url.
     *
     * @return The proxy icon url.
     */
    std::string proxy_icon_url() const;

    /** Whether or not this object should be considered incomplete/empty.
     * 
     * @return True if this object shouldn't be sent in an API call.
     */
    bool empty() const
    {
      return m_text.empty();
    }
  };

  /** An image object of an embed. */
  class embed_image
  {
    std::string m_url;
    std::string m_proxy_url;
    uint32_t m_height;
    uint32_t m_width;
  public:
    explicit embed_image(connection_state* owner, rapidjson::Value& data);
    explicit embed_image(std::string url = "", uint32_t width = 0, uint32_t height = 0, std::string proxy_url = "");

    template <typename Writer>
    void Serialize(Writer& writer) const;

    /** Get the URL of the image.
     *
     * @return The URL of the image.
     */
    std::string url() const;

    /** Get the proxied URL of the image.
     *
     * @return The proxied URL of the image.
     */
    std::string proxy_url() const;

    /** Get the width of the image.
     *
     * @return The width of the image
     */
    uint32_t width() const;

    /** Get the height of the image.
     *
     * @return The height of the image.
     */
    uint32_t height() const;

    /** Whether or not this object should be considered incomplete/empty.
     *
     * @return True if this object shouldn't be sent in an API call.
     */
    bool empty() const
    {
      return m_url.empty();
    }
  };

  //  embed_image and embed_thumbnail objects are identical, so just typedef it.
  typedef embed_image embed_thumbnail;

  /** A video object of an embed. */
  class embed_video
  {
    std::string m_url;
    uint32_t m_height;
    uint32_t m_width;
  public:
    explicit embed_video(connection_state* owner, rapidjson::Value& data);
    explicit embed_video(std::string url = "", uint32_t width = 0, uint32_t height = 0);

    template <typename Writer>
    void Serialize(Writer& writer) const;

    /** Get the URL of the video.
     *
     * @return The URL of the video.
     */
    std::string url() const;

    /** Get the width of the video.
     *
     * @return The width of the video.
     */
    uint32_t width() const;

    /** Get the height of the video.
     *
     * @return The height of the video.
     */
    uint32_t height() const;

    /** Whether or not this object should be considered incomplete/empty.
     *
     * @return True if this object shouldn't be sent in an API call.
     */
    bool empty() const
    {
      return m_url.empty();
    }
  };

  /** Wraps embed provider information */
  class embed_provider
  {
    std::string m_name;
    std::string m_url;
  public:
    explicit embed_provider(connection_state* owner, rapidjson::Value& data);
    explicit embed_provider(std::string name = "", std::string url = "");

    template <typename Writer>
    void Serialize(Writer& writer) const;

    /** Get the name of the provider.
     *
     * @return The name of the provider.
     */
    std::string name() const;

    /** Get the URL of the provider.
     *
     * @return The URL of the provider.
     */
    std::string url() const;

    /** Whether or not this object should be considered incomplete/empty.
     *
     * @return True if this object shouldn't be sent in an API call.
     */
    bool empty() const
    {
      return m_name.empty();
    }
  };

  /** Holds information on the listed author of an embed. */
  class embed_author
  {
    std::string m_name;
    std::string m_url;
    std::string m_icon_url;
    std::string m_proxy_icon_url;
  public:
    explicit embed_author(connection_state* owner, rapidjson::Value& data);
    explicit embed_author(std::string name = "", std::string url = "", std::string icon_url = "", std::string proxy_icon_url = "");

    template <typename Writer>
    void Serialize(Writer& writer) const;

    /** Get the name of the author.
     *
     * @return The name of the author.
     */
    std::string name() const;

    /** Get the URL of the author.
     *
     * @return The URL of the author.
     */
    std::string url() const;

    /** Get the icon URL of the author.
     *
     * @return The icon URL of the author.
     */
    std::string icon_url() const;

    /** Get the proxied icon URL of the author.
     *
     * @return The proxied icon URL of the author.
     */
    std::string proxy_icon_url() const;

    /** Whether or not this object should be considered incomplete/empty.
     *
     * @return True if this object shouldn't be sent in an API call.
     */
    bool empty() const
    {
      return m_name.empty();
    }
  };

  /** Represents a field of an embed. */
  class embed_field
  {
    std::string m_name;
    std::string m_value;
    bool m_inline;
  public:
    embed_field();
    explicit embed_field(connection_state* owner, rapidjson::Value& data);
    explicit embed_field(std::string name = "", std::string value = "", bool is_inline = false);

    template <typename Writer>
    void Serialize(Writer& writer) const;

    /** Get the name of the field.
     *
     * @return The name of the field.
     */
    std::string name() const;

    /** Get the content of the field.
     *
     * @return The content of the field.
     */
    std::string content() const;

    /** Whether this field is inline or not.
     *
     * @return Whether this field is inline or not.
     */
    bool is_inline() const;

    /** Whether or not this object should be considered incomplete/empty.
     *
     * @return True if this object shouldn't be sent in an API call.
     */
    bool empty() const
    {
      return m_name.empty() || m_value.empty();
    }
  };

  /** Represents a Discord embed object. */
  class embed
  {
    std::string m_title;
    std::string m_type;
    std::string m_description;
    std::string m_url;
    std::string m_timestamp;
    uint32_t m_color;
    embed_footer m_footer;
    embed_image m_image;
    embed_thumbnail m_thumbnail;
    embed_video m_video;
    embed_provider m_provider;
    embed_author m_author;
    std::vector<embed_field> m_fields;
  public:
    embed();
    explicit embed(connection_state* owner, rapidjson::Value& data);

    template <typename Writer>
    void Serialize(Writer& writer) const;

    /** Sets the title of the embed.
     *
     * @param title The title for this embed.
     */
    void set_title(std::string title);

    /** Sets the description of the embed.
     *
     * @param description The description for this embed.
     */
    void set_description(std::string description);

    /** Sets the url of the embed.
     *
     * @param url The url for this embed.
     */
    void set_url(std::string url);

    /** Sets the footer of the embed.
     *
     * @param text The text of the footer.
     * @param icon_url The icon to use for the footer.
     * @param proxy_icon_url
     */
    void set_footer(std::string text, std::string icon_url = "", std::string proxy_icon_url = "");

    /** Sets the image of the embed.
     *
     * @param url A URL to the image.
     */
    void set_image(std::string url);

    /** Sets the thumbnail of the embed.
     *
     * @param url A URL to the thumbnail.
     */
    void set_thumbnail(std::string url);

    /** Sets the video of the embed.
     *
     * @param url A URL to the video.
     */
    void set_video(std::string url);

    /** Sets the author of the embed.
     *
     * @param name The name of the author.
     * @param url A URL to the author.
     * @param icon_url A URL to an icon to use for the author.
     */
    void set_author(std::string name, std::string url = "", std::string icon_url = "");

    /** Adds a field to the embed.
     *
     * @param name The name of this field.
     * @param value The content of this embed.
     * @param is_inline Whether or not this field should be displayed inline or not.
     */
    void add_field(std::string name, std::string value = "", bool is_inline = false);

    std::string title() const;
    std::string type() const;
    std::string description() const;
    std::string url() const;
    std::string timestamp() const;
    uint32_t color() const;
    embed_footer& footer();
    embed_image& image();
    embed_thumbnail& thumbnail();
    embed_video& video();
    embed_provider& provider();
    embed_author& author();
    std::vector<embed_field>& fields();

    bool empty() const;
  };

  /******************************************
   * Start RapidJSON Serialization methods. *
   ******************************************/

  template <typename Writer>
  void embed_footer::Serialize(Writer& writer) const
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

  template <typename Writer>
  void embed_image::Serialize(Writer& writer) const
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

  template <typename Writer>
  void embed_video::Serialize(Writer& writer) const
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

  template <typename Writer>
  void embed_provider::Serialize(Writer& writer) const
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

  template <typename Writer>
  void embed_author::Serialize(Writer& writer) const
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

  template <typename Writer>
  void embed_field::Serialize(Writer& writer) const
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

  template <typename Writer>
  void embed::Serialize(Writer& writer) const
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
      for (auto& field : m_fields)
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
