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
  
  bool slogin_res = !conn.send(slogin_message);

  if (slogin_res) {
    fatal("Server message not received");
  }
  std::string local_buf;
  while ( std::cin >> local_buf) {
    Message messageToSend;
  }
  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate

  return 0;
}
