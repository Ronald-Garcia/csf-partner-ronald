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

  bool invalid_command;
  bool in_room = false;
  while ( !std::cin.eof()) {
    invalid_command = false;
    Message message_to_send;
    getline(std::cin, local_buf);


    if (local_buf.at(0) == '\\') {
      try {
        message_to_send = handle_line_command(local_buf);
        in_room = conn.send(message_to_send)  // message is sent correctly
               && message_to_send.tag == std::string(TAG_JOIN); // message was a join message
               // if the message was anything but a join message, then not in room (quit or leave)
               // both leave a room
      } catch (std::invalid_argument) { // only happens if invalid command was used (/command)
        std::cerr << "Invalid command used." << std::endl;
        invalid_command = true;
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
