#include <cassert>
#include <ctime>
#include "message_queue.h"
#include "guard.h"

MessageQueue::MessageQueue() {
  // Complete: initialize the mutex and the semaphore

  pthread_mutex_init(&m_lock, NULL);
  sem_init(&m_avail, 0, QUEUE_MAX);
  sem_init(&m_items, 0, 0);
  
}

MessageQueue::~MessageQueue() {
  // Completed: destroy the mutex and the semaphore
  
  pthread_mutex_destroy(&m_lock);
  sem_destroy(&m_avail);
  sem_destroy(&m_items);
}

void MessageQueue::enqueue(Message *msg) {
  // Completed: put the specified message on the queue

  sem_wait(&m_avail);

  {
    Guard g(m_lock);
    m_messages.push_back(msg);
  }  

  sem_post(&m_items);
  
  // be sure to notify any thread waiting for a message to be
  // available by calling sem_post
}

Message *MessageQueue::dequeue() {
  struct timespec ts;

  // get the current time using clock_gettime:
  // we don't check the return value because the only reason
  // this call would fail is if we specify a clock that doesn't
  // exist
  clock_gettime(CLOCK_REALTIME, &ts);

  // compute a time one second in the future
  ts.tv_sec += 1;

  // Completed: call sem_timedwait to wait up to 1 second for a message
  //       to be available, return nullptr if no message is available

  if (!sem_timedwait(&m_items, &ts)) {
    return nullptr;
  }
  Message *msg = nullptr;

  {
    Guard g(m_lock);
    msg = m_messages.front();
    m_messages.pop_front();
  }

  sem_post(&m_avail);


  // Completed: remove the next message from the queue, return it
  return msg;
}
