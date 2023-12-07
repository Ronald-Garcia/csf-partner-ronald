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

  

   

  // Completed: depending on whether the client logged in as a sender or
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

  delete connInfo;



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

    Message received_message;

    while (!quit) {
      received_message.tag = "";
      received_message.data = "";

      if (!connection->receive(received_message)) {
        // no need to check for valid send, as we create the message
        err_message.data = "invalid message";
        connection->server_send(err_message);
        return;
      } else if (received_message.tag == std::string(TAG_JOIN)) { //Join logic.
        cur_room = server->find_or_create_room(received_message.data);
        in_room = true;
        ok_message.data = "ok joined " + std::string(cur_room->get_room_name());
        connection->server_send(ok_message);
      } else if (received_message.tag == std::string(TAG_SENDALL)){ //Send logic.
        if(!in_room) {
          err_message.data = "not in a room";
          connection->server_send(err_message);
        } else {

          // broadcasting delivery.
          cur_room->broadcast_message(user.username, received_message.data);
          ok_message.data = "sent message";
          connection->server_send(ok_message);
        }
      } else if (received_message.tag == std::string(TAG_LEAVE)) {
        if(in_room) {
          in_room = false;
          ok_message.data = "ok left " + std::string(cur_room->get_room_name());
          connection->server_send(ok_message);
        }
        else { //If not in room.
          err_message.data = "not in a room";
          connection->server_send(err_message);
        }
      } else if (received_message.tag == std::string(TAG_QUIT)) {

        in_room = false;

        ok_message.data = "ok quit";

        connection->server_send(ok_message);
        quit = true;
      } else {
        //We recieved a "valid" messege, but not for sender at this point.
        err_message.data = "invalid message";
        connection->server_send(err_message);
        return;
      }

    }
  }

  void chat_with_receiver(Server::ConnInfo* connInfo, User& user) {

    Connection* connection = connInfo->connection;
    Server* server = connInfo->server;
    Room* cur_room;

    bool in_room = false;

    Message err_message(TAG_ERR, "Sorry, you can't do that");
    Message ok_message(TAG_OK, "confirmed.");

    //First, get the join message from reciever. If anything but join, send
    //error back.
    while (!in_room) {
      Message received_message;
      
      if (!connection->receive(received_message)) { //Get the messege.
        connection->close();
        return;
      } else if (received_message.tag == std::string(TAG_JOIN) && (!in_room)){ //If its join
      //Find or create room being referenced, place reciever in that room.
        cur_room = server->find_or_create_room(received_message.data);
        cur_room->add_member(&user);

        ok_message.data = "joined room " + std::string(received_message.data);
        connection->server_send(ok_message);
        in_room = true;
      } else { //Otherwise, recieved a message that is not valid. Another leave case.
        return;
      }
    }
    //After joining a room, just wait for messages from senders.
    Message* new_message_ptr;
    Message new_message;

    while (connection->is_open()) {
      //Reset.
      new_message_ptr = nullptr;
      new_message.tag = "";
      new_message.data = "";
      //Just try to deque immediately. If fails, loop again.
      new_message_ptr = user.mqueue.dequeue();
      if (new_message_ptr != nullptr) {
        new_message.tag = new_message_ptr->tag;
        new_message.data = new_message_ptr->data;
        connection->server_send(new_message);
      }
    }
    //Remove user from room.
    cur_room->remove_member(&user);
    
    delete new_message_ptr;
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
  
  m_ssock = create_server_socket(m_port);

  
  return m_ssock >= 0;
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
    ConnInfo* connInfo = new ConnInfo;
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
