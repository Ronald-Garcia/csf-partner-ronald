#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <vector>
#include <cctype>
#include <cassert>
#include "message.h"
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

  void chat_with_sender(Server::ConnInfo* connInfo, User& user);
  void chat_with_receiver(Server::ConnInfo* connInfo, User& user);


void *worker(void *arg) {
  pthread_detach(pthread_self());

  // Completed: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  Server::ConnInfo* connInfo = (Server::ConnInfo*) arg;

  Connection* connection = connInfo->connection;

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

  User user(msg.data);

  // sender
  if (msg.tag == std::string(TAG_SLOGIN)) {
    chat_with_sender(connInfo, user);
  } else {
    // if not slogin, then receiver
    chat_with_receiver(connInfo, user);
  }


  return nullptr;
}

  void chat_with_sender(Server::ConnInfo* connInfo, User& user) {

    Connection* connection = connInfo->connection;
    Server* server = connInfo->server;
    Room* cur_room;

    bool in_room = false;
    bool quit = false;

    Message err_message(TAG_ERR, "Sorry, you can't do that");
    Message ok_message(TAG_OK, "confirmed.");

    while (!quit) {

      Message received_message;
      
      if (!connection->receive(received_message)) {
        
        // no need to check for valid send, as we create the message
        connection->server_send(err_message);
      } else if (received_message.tag == std::string(TAG_JOIN)) {
        cur_room = server->find_or_create_room(received_message.data);
        in_room = true;
        ok_message.data = "ok joined.";
        connection->server_send(ok_message);
      } else if (received_message.tag == std::string(TAG_SENDALL)){
        if(!in_room) {
          connection->server_send(err_message);
        } else {

          // broadcasting delivery.
          cur_room->broadcast_message(user.username, received_message.data);
          ok_message.data = "sent message";
          connection->server_send(ok_message);
        }
      } else if (received_message.tag == std::string(TAG_LEAVE)) {

        in_room = false;
        ok_message.data = "ok left";
        connection->server_send(ok_message);
      } else if (received_message.tag == std::string(TAG_QUIT)) {

        in_room = false;

        ok_message.data = "ok quit";

        connection->server_send(ok_message);
        quit = true;
      }

    }
  }

  void chat_with_receiver(Server::ConnInfo* connection, User& user) {
    bool in_room = false;
    bool quit = false;

    while (!quit) {
      connection;
    }

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

  char port = m_port;
  return open_listenfd(&port) >= 0;
}

void Server::handle_client_requests() {
  // Completed: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client

  while (1) {

    int clientfd = Accept(m_ssock, NULL, NULL);
    
    if (clientfd < 0) { 
      fatal("Error accepting client connection.");
    }

    pthread_t thr_id;
    ConnInfo* connInfo = (ConnInfo*) malloc(sizeof(ConnInfo));
    connInfo->connection = new Connection(clientfd);
    connInfo->server = this;

    if (pthread_create(&thr_id, NULL, worker, connInfo) != 0) {
      fatal("pthread_create failed.");
    }
  }

}

Room *Server::find_or_create_room(const std::string &room_name) {
  // Completed: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary

  std::pair<const std::string, Room*> room_name_pair(room_name, new Room(room_name)) ;

  std::pair<RoomMap::iterator, bool> i = m_rooms.insert(room_name_pair);

  if (!i.second) {
    delete room_name_pair.second;
  }

  return i.first->second;
}
