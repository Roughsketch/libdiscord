#pragma once

#include "common.h"
#include "serializable.h"

namespace discord
{
  class ConnectionState;

  /** The footer object of an Embed. */
  class EmbedFooter : Serializable
  {
    std::string m_text;
    std::string m_icon_url;
    std::string m_proxy_icon_url;
  public:
    explicit EmbedFooter(ConnectionState* owner, rapidjson::Value& data);
    explicit EmbedFooter(std::string text = "", std::string icon_url = "", std::string proxy_icon_url = "");

    void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) const override;

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

  /** An image object of an Embed. */
  class EmbedImage : Serializable
  {
    std::string m_url;
    std::string m_proxy_url;
    uint32_t m_height;
    uint32_t m_width;
  public:
    explicit EmbedImage(ConnectionState* owner, rapidjson::Value& data);
    explicit EmbedImage(std::string url = "", uint32_t width = 0, uint32_t height = 0, std::string proxy_url = "");

    void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) const override;

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

  //  EmbedImage and embed_thumbnail objects are identical, so just typedef it.
  typedef EmbedImage embed_thumbnail;

  /** A video object of an Embed. */
  class EmbedVideo : Serializable
  {
    std::string m_url;
    uint32_t m_height;
    uint32_t m_width;
  public:
    explicit EmbedVideo(ConnectionState* owner, rapidjson::Value& data);
    explicit EmbedVideo(std::string url = "", uint32_t width = 0, uint32_t height = 0);

    void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) const override;

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

  /** Wraps Embed provider information */
  class EmbedProvider : Serializable
  {
    std::string m_name;
    std::string m_url;
  public:
    explicit EmbedProvider(ConnectionState* owner, rapidjson::Value& data);
    explicit EmbedProvider(std::string name = "", std::string url = "");

    void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) const override;

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

  /** Holds information on the listed author of an Embed. */
  class EmbedAuthor : Serializable
  {
    std::string m_name;
    std::string m_url;
    std::string m_icon_url;
    std::string m_proxy_icon_url;
  public:
    explicit EmbedAuthor(ConnectionState* owner, rapidjson::Value& data);
    explicit EmbedAuthor(std::string name = "", std::string url = "", std::string icon_url = "", std::string proxy_icon_url = "");

    void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) const override;

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

  /** Represents a field of an Embed. */
  class EmbedField : Serializable
  {
    std::string m_name;
    std::string m_value;
    bool m_inline;
  public:
    EmbedField();
    explicit EmbedField(ConnectionState* owner, rapidjson::Value& data);
    explicit EmbedField(std::string name = "", std::string value = "", bool is_inline = false);

    void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) const override;

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

  /** Represents a Discord Embed object. */
  class Embed : Serializable
  {
    std::string m_title;
    std::string m_type;
    std::string m_description;
    std::string m_url;
    std::string m_timestamp;
    uint32_t m_color;
    EmbedFooter m_footer;
    EmbedImage m_image;
    embed_thumbnail m_thumbnail;
    EmbedVideo m_video;
    EmbedProvider m_provider;
    EmbedAuthor m_author;
    std::vector<EmbedField> m_fields;
  public:
    Embed();
    explicit Embed(ConnectionState* owner, rapidjson::Value& data);

    void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& writer) const override;

    /** Sets the title of the Embed.
     *
     * @param title The title for this Embed.
     */
    void set_title(std::string title);

    /** Sets the description of the Embed.
     *
     * @param description The description for this Embed.
     */
    void set_description(std::string description);

    /** Sets the url of the Embed.
     *
     * @param url The url for this Embed.
     */
    void set_url(std::string url);

    /** Sets the footer of the Embed.
     *
     * @param text The text of the footer.
     * @param icon_url The icon to use for the footer.
     * @param proxy_icon_url
     */
    void set_footer(std::string text, std::string icon_url = "", std::string proxy_icon_url = "");

    /** Sets the image of the Embed.
     *
     * @param url A URL to the image.
     */
    void set_image(std::string url);

    /** Sets the thumbnail of the Embed.
     *
     * @param url A URL to the thumbnail.
     */
    void set_thumbnail(std::string url);

    /** Sets the video of the Embed.
     *
     * @param url A URL to the video.
     */
    void set_video(std::string url);

    /** Sets the author of the Embed.
     *
     * @param name The name of the author.
     * @param url A URL to the author.
     * @param icon_url A URL to an icon to use for the author.
     */
    void set_author(std::string name, std::string url = "", std::string icon_url = "");

    /** Adds a field to the Embed.
     *
     * @param name The name of this field.
     * @param value The content of this Embed.
     * @param is_inline Whether or not this field should be displayed inline or not.
     */
    void add_field(std::string name, std::string value = "", bool is_inline = false);

    std::string title() const;
    std::string type() const;
    std::string description() const;
    std::string url() const;
    std::string timestamp() const;
    uint32_t color() const;
    EmbedFooter& footer();
    EmbedImage& image();
    embed_thumbnail& thumbnail();
    EmbedVideo& video();
    EmbedProvider& provider();
    EmbedAuthor& author();
    std::vector<EmbedField>& fields();

    bool empty() const;
  };
}
