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
