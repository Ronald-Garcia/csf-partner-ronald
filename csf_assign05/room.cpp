#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"
#include "room.h"

Room::Room(const std::string &room_name)
  : room_name(room_name) {
  // Completed: initialize the mutex

  if (!pthread_mutex_init(&lock, NULL)) {
    throw std::exception();
  }
}

Room::~Room() {
  // Completed: destroy the mutex

  if (!pthread_mutex_destroy(&lock)) {
    throw std::exception();
  }
}

void Room::add_member(User *user) {
  // Completed: add User to the room
  {
    Guard g(lock);
    members.insert(user); 
  }
}

void Room::remove_member(User *user) {
  // Completed: remove User from the room
  {
    Guard g(lock);
    members.erase(user);
  }
}

void Room::broadcast_message(const std::string &sender_username, const std::string &message_text) {

  // TODO: send a message to every (receiver) User in the room

  for (UserSet::iterator i = members.begin(); i != members.end(); i++ ) {
    Message m(TAG_SENDALL, message_text);
    (*i)->mqueue.enqueue(&m);
  }
}
