#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  Connection conn;

  // Completed: connect to server
  conn.client_connect(server_hostname, server_port);

  // Completed: send rlogin and join messages (expect a response from
  //       the server for each one)

  Message rlogin_message(TAG_RLOGIN, username);
  Message join_message(TAG_JOIN, room_name);

  bool rlogin_res = !conn.send(rlogin_message);
  bool join_res =  !conn.send(join_message);

  if (rlogin_res && join_res) {
    fatal("Server message not received");
  }

  Message incoming_message;

  while (1) {
    
    if (!conn.receive(incoming_message)) {
      fatal("Error");
    }

    const char* incoming_tag = incoming_message.tag.c_str();
    if ( strcmp(incoming_tag, TAG_DELIVERY) ) {
      fatal("Not a delivery");
    } 

    if (!strcmp(incoming_tag, TAG_ERR)) {
      fatal("Connection closed unexpectedly.");
    }
    handle_delivery(incoming_message, room_name);
  }

  return 0;
}
