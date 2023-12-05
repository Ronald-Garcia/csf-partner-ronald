#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <vector>
#include <cctype>
#include <cassert>
#include "message.h"
#include "connection.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "client_util.h"
#include "server.h"

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////

// TODO: add any additional data types that might be helpful
//       for implementing the Server member functions

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {

void *worker(void *arg) {
  pthread_detach(pthread_self());

  // Completed: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  Connection* connection = (Connection*) arg;

  // Completed: read login message (should be tagged either with
  //       TAG_SLOGIN or TAG_RLOGIN), send response

  Message msg;
  if (!connection->receive(msg)) {
    fatal("Did not receive message");
  }
  Message response_message;
  if (msg.tag == std::string(TAG_SLOGIN) || msg.tag == std::string(TAG_RLOGIN)) {

    response_message.tag = TAG_OK;
    response_message.data = "logged in";

  } else {
    
    response_message.tag = TAG_ERR;
    response_message.data = "Sorry, you can't do that!";

  }
  if (!connection->server_send(response_message)) {
    fatal("Against all odds, an invalid message was sent.");
  }

  

   

  // TODO: depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities
  //       is a good idea)

  return nullptr;
}

}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
  : m_port(port)
  , m_ssock(-1) {
  // Completed: initialize mutex

  pthread_mutex_init(&m_lock, NULL);
}

Server::~Server() {
  // Completed: destroy mutex
  pthread_mutex_destroy(&m_lock);
}

bool Server::listen() {
  // Completed: use open_listenfd to create the server socket, return true
  //       if successful, false if not
  return open_listenfd((const char*) &m_port) >= 0;
}

void Server::handle_client_requests() {
  // TODO: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client

  while (1) {

    int clientfd = Accept(m_ssock, NULL, NULL);
    
    if (clientfd < 0) { 
      fatal("Error accepting client connection.");
    }

    pthread_t thr_id;

    Connection* connection = new Connection(clientfd);
    

    if (pthread_create(&thr_id, NULL, worker, connection) != 0) {
      fatal("pthread_create failed.");
    }
  }

}

Room *Server::find_or_create_room(const std::string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary

  std::pair<const std::string, Room*> room_name_pair(room_name, new Room(room_name)) ;

  std::pair<RoomMap::iterator, bool> i = m_rooms.insert(room_name_pair);

  if (!i.second) {
    delete room_name_pair.second;
  }

  return i.first->second;
}
