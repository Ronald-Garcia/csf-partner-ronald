#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "Usage: ./sender [server_address] [port] [username]\n";
    return 1;
  }

  std::string server_hostname;
  int server_port;
  std::string username;

  server_hostname = argv[1];
  server_port = std::stoi(argv[2]);
  username = argv[3];

  Connection conn;

  // Completed: connect to server
  conn.client_connect(server_hostname, server_port);

  // Completed: send rlogin and join messages (expect a response from
  //       the server for each one)

  Message slogin_message(TAG_SLOGIN, username);
  
  // send slogin to server
  bool slogin_res = !conn.send(slogin_message);

  // if there was an error message, then exit (error message is sent to cerr already)
  if (slogin_res) {
    exit(1);
  }
  
  // stores each line of text sent by sender
  std::string local_buf;

  bool in_room = false;
  while ( !std::cin.eof()) {
    Message message_to_send;
    getline(std::cin, local_buf);

    if (std::cin.eof()) {
      message_to_send.tag = TAG_QUIT;
      conn.send(message_to_send);
      exit(0);
    }

    if (local_buf.at(0) == '/') {
      try {
        message_to_send = handle_line_command(local_buf);
        bool send_res = conn.send(message_to_send);
        // if the message was sent correctly, 
        // in a room if join message was sent
        // not in a room if leave or quit message was sent
        if (send_res) {
          in_room = message_to_send.tag == std::string(TAG_JOIN);
          
          // if message was quit, stop program
          if (message_to_send.tag == std::string(TAG_QUIT)) {
            exit(0);
          }
        }
        // } else if (send_res 
        //         && (message_to_send.tag == std::string(TAG_LEAVE)
        //         || message_to_send.tag == std::string(TAG_QUIT))) {
        //           in_room = false;
        //         }
        
      } catch (std::invalid_argument&) { // only happens if invalid command was used (/command)
        std::cerr << "Invalid command used." << std::endl;
      } 
    } else if (in_room) {
      // if in a room, send the message to the room users
      message_to_send.tag = TAG_SENDALL;
      message_to_send.data = local_buf;
      conn.send(message_to_send);
    } else {
      // if not in a room and a chat message is trying to be sent, then error (not in a room, can't send)
      std::cerr << "Not in a room yet!" << std::endl;
    }
  }
  // Completed: loop reading commands from user, sending messages to
  //       server as appropriate

  return 0;
}
