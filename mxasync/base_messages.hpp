/*
Copyright (c) 2013, Anton Grigoryev. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of copyright holders.
*/


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
