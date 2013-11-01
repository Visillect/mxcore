#pragma once

#include <deque>
#include <boost/thread.hpp>

namespace mxasync {

template <class T>
class Queue
{
public:
  Queue()
    : _notEmptyPredicate(_queue)
  {
  }

  void push(T x)
  {
    boost::lock_guard<boost::mutex> lock(_mutex);
    _queue.push_back(x);
    _notEmptyCondvar.notify_all();
  }

  T pop()
  {
    boost::unique_lock<boost::mutex> lock(_mutex);
    _notEmptyCondvar.wait(lock, _notEmptyPredicate);
    T x = _queue.front();
    _queue.pop_front();
    return x;
  }

  // discard all but the most recent
  T pop_most_recent()
  {
    boost::unique_lock<boost::mutex> lock(_mutex);
    _notEmptyCondvar.wait(lock, _notEmptyPredicate);
    T x = _queue.back();
    _queue.clear();
    return x;
  }

  // @return false timeout expired, t is untouched
  // @return true everything ok, t stores the popped object
  bool timed_pop(T &t, unsigned milliseconds)
  {
    boost::unique_lock<boost::mutex> lock(_mutex);
    bool res = _notEmptyCondvar.timed_wait(lock, boost::posix_time::millisec(milliseconds), _notEmptyPredicate);
    if (res)
    {
      t = _queue.front();
      _queue.pop_front();
    }
    return res;
  }

  // @return false timeout expired, t is untouched
  // @return true everything ok, t stores the popped object
  bool timed_pop_most_recent(T &t, unsigned milliseconds)
  {
    boost::unique_lock<boost::mutex> lock(_mutex);
    bool res = _notEmptyCondvar.timed_wait(lock, boost::posix_time::millisec(milliseconds), _notEmptyPredicate);
    if (res)
    {
      t = _queue.back();
      _queue.clear();
    }
    return res;
  }

  void clear()
  {
    boost::lock_guard<boost::mutex> lock(_mutex);
    _queue.clear();
  }

  int size() const
  {
    boost::lock_guard<boost::mutex> lock(_mutex);
    int s = _queue.size();
    return s;
  }

  bool empty() const
  {
    return size() == 0;
  }

private:
  std::deque<T> _queue;
  mutable boost::mutex _mutex;
  mutable boost::condition_variable _notEmptyCondvar;

  class _NotEmptyPredicate
  {
  public:
    _NotEmptyPredicate(const std::deque<T> &queue)
      : _queue(queue)
    {
    }
    bool operator()() const
    {
      return !_queue.empty();
    }
  private:
    const std::deque<T> &_queue;
  };
  _NotEmptyPredicate _notEmptyPredicate;
};

}  // namespace mxasync
