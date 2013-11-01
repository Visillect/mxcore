#pragma once

#include <mxasync/queue.hpp>
#include <boost/noncopyable.hpp>
#include <typeinfo>
#include <boost/thread.hpp>
#include <exception>
#include <mxasync/base_messages.hpp>


namespace mxasync {


class Actor : private boost::noncopyable
{
public:

  Actor()
  { }

  virtual ~Actor() { }

  bool isRunning() const { return thread.joinable(); }
  
  void start()
  {
    thread = boost::thread(ThreadProc(*this));
  }

  bool join()
  {
    if (!thread.joinable())
      return false;

    thread.join();
    return true;
  }

protected:

  virtual void run() = 0;

private:
  struct ThreadProc
  {
    Actor & owner;
    ThreadProc(Actor & owner)
    : owner(owner)
    { }

    void operator () ()
    {
      owner.run();
    }
  };

  boost::thread thread;
};

} // namespace mxasync
