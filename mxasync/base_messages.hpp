#pragma once

#include <boost/noncopyable.hpp>
#include <typeinfo>
#include <compat/tr1_memory.h>
#include <string>
#include <exception>

namespace mxasync {


class Message : private boost::noncopyable
{
public:
  virtual ~Message() { }

  virtual std::string toString() const
  {
    return typeid(*this).name();
  }

protected:
  Message()
  { }
};

typedef std::tr1::shared_ptr<const Message> PMessage;
#define DECLARE_PMESSAGE_TYPE(ty) typedef std::tr1::shared_ptr<const ty> P##ty

// just a shorter alias for std::tr1::dynamic_pointer_cast
template <class T>
inline std::tr1::shared_ptr<const T> msg_cast(PMessage const& m)
{
  return std::tr1::dynamic_pointer_cast<const T>(m);
}

class BadMessage : public std::exception
{
public:
  BadMessage(PMessage const& msg)
  {
    message = "BadMessage: ";
    if (!msg)
      message += "null";
    else
      message += typeid(*msg).name() + (": " + msg->toString());
  }

  virtual ~BadMessage() throw ()
  { }

  const char * what() const throw()
  {
    return message.c_str();
  }

private:
  std::string message;
};


class TextMessage : public Message
{
public:
  TextMessage(std::string const& text)
  : text(text)
  { }

  virtual std::string toString() const
  {
    return text;
  }

private:
  std::string text;
};
DECLARE_PMESSAGE_TYPE(TextMessage);

class StopMessage;
DECLARE_PMESSAGE_TYPE(StopMessage);
class StopMessage : public TextMessage
{
public:
  StopMessage(std::string const& text = "stop")
  : TextMessage(text)
  { }

  static PStopMessage create(std::string const& text = "stop")
  {
    return PStopMessage(new StopMessage(text));
  }
};


} // namespace mxasync
