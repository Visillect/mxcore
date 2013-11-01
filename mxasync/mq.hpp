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
#include <compat/tr1_memory.h>
#include <boost/noncopyable.hpp>
#include <mxasync/queue.hpp>
#include <mxasync/base_messages.hpp>
#include <vector>
#include <stdexcept>


namespace mxasync {

class MessageInput : private boost::noncopyable
{
public:
  virtual ~MessageInput()
  { }

  virtual PMessage pop() = 0;
  virtual PMessage popMostRecent() = 0;
  virtual bool     timedPop(PMessage & m, unsigned milliseconds) = 0;
  virtual bool     timedPopMostRecent(PMessage & m, unsigned milliseconds) = 0;

protected:
  MessageInput()
  { }
};

typedef std::tr1::shared_ptr<MessageInput> PMessageInput;

class MessageOutput : private boost::noncopyable
{
public:
  virtual ~MessageOutput()
  { }

  virtual void push(PMessage const& m) = 0;

protected:
  MessageOutput()
  { }
};

typedef std::tr1::shared_ptr<MessageOutput> PMessageOutput;

class NullMessageOutput : public MessageOutput
{
public:
  NullMessageOutput()
  { }

  virtual void push(PMessage const& m)
  { }
};


class MessageQueue : public MessageInput,
                     public MessageOutput
{
public:
  MessageQueue()
  { }

  virtual PMessage pop()
  {
    return queue.pop();
  }

  virtual PMessage popMostRecent()
  {
    return queue.pop_most_recent();
  }

  virtual bool timedPop(PMessage & m, unsigned milliseconds)
  {
    return queue.timed_pop(m, milliseconds);
  }

  virtual bool timedPopMostRecent(PMessage & m, unsigned milliseconds)
  {
    return queue.timed_pop_most_recent(m, milliseconds);
  }

  void clear()
  {
    return queue.clear();
  }

  int size()
  {
    return queue.size();
  }

  virtual void push(PMessage const& m)
  {
    queue.push(m);
  }


protected:
  Queue<PMessage> queue;
};

typedef std::tr1::shared_ptr<MessageQueue> PMessageQueue;


class MessageMulticaster : public MessageOutput
{
public:
  MessageMulticaster()
  { }

  // non-thread-safe! invoke before threads started
  void addOutput(PMessageOutput const& out)
  {
    if (!out)
      throw std::invalid_argument("null output");
    outputs.push_back(out);
  }

  PMessageQueue createOutput()
  {
    PMessageQueue out(new MessageQueue());
    addOutput(out);
    return out;
  }

  void clearOutputs()
  {
    outputs.clear();
  }

  virtual void push(PMessage const& m)
  {
    for (size_t i = 0; i < outputs.size(); ++i)
      outputs[i]->push(m);
  }

private:
  std::vector<PMessageOutput> outputs;
};

typedef std::tr1::shared_ptr<MessageMulticaster> PMessageMulticaster;


} // namespace mxasync
