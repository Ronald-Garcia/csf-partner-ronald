#include <iostream>
#include <string>
#include "connection.h"
#include "message.h"
#include "client_util.h"
#include <netinet/in.h>
#include <sstream>
#include <exception>

// string trim functions shamelessly stolen from
// https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s) {
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string rtrim(const std::string &s) {
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 
std::string trim(const std::string &s) {
  return rtrim(ltrim(s));
}


/*
 * SOURCE: EXAMPLE CODE IN LECTURE sockets.zip
 */
int create_server_socket(int port) {
  struct sockaddr_in serveraddr = {0};
  int ssock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (ssock_fd < 0) 
    fatal("socket failed");

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)port);
  if (bind(ssock_fd, (struct sockaddr *) &serveraddr, 
	   sizeof(serveraddr)) < 0) 
    fatal("bind failed");

  if (listen(ssock_fd, 5) < 0)
    fatal("listen failed");

  return ssock_fd;
}

/*
 * SOURCE: EXAMPLE CODE IN LECTURE sockets.zip
 */
int accept_connection(int ssock_fd, struct sockaddr_in *clientaddr) {
  unsigned clientlen = sizeof(clientaddr);
  int client_fd = accept(ssock_fd, (struct sockaddr *) clientaddr, &clientlen);
  if (client_fd < 0) 
    fatal("accept failed");
  return client_fd;
}

void handle_delivery(Message message, std::string room) {

  // get message data from message
  std::string payload = message.data;

  // get the first instance of colon
  int first_colon_index = payload.find(":");
  // get index in substring then add by offset
  int second_colon_index = first_colon_index + 1 + payload.substr(first_colon_index + 1).find(":");

  // divide up each string
  std::string room_name = payload.substr(0, first_colon_index);
  std::string username = payload.substr(first_colon_index + 1, second_colon_index - first_colon_index - 1);
  std::string message_content = payload.substr(second_colon_index + 1);

  // if room matches, write
  if (room == room_name) {
    std::cout << username << ": " << message_content << std::endl;
  } 
}

void fatal(const char* msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

Message handle_line_command(std::string line) {
  
  
  Message message_to_send;

  //Place string in string stream.
  std::stringstream ss;
  ss.str(line);

  // gets first word of the line
  std::string cur_word;
  ss >> cur_word;

  std::string command = cur_word;

  if (command == std::string("/join")) {
    
    // get the room name
    ss >> cur_word;

    std::string room_name = cur_word;

    message_to_send.tag = TAG_JOIN;
    message_to_send.data = room_name; 
  } else if (command == std::string("/leave")) {
    
    message_to_send.tag = TAG_LEAVE;
    message_to_send.data = "bye";
  } else if (command == std::string("/quit")) {
    message_to_send.tag = TAG_QUIT;
    message_to_send.data = "bye";
  } else {
    throw std::invalid_argument("Invalid argument");
  }


  return message_to_send;
}