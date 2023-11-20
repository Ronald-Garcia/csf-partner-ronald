#include <iostream>
#include <string>
#include "connection.h"
#include "message.h"
#include "client_util.h"
#include <netinet/in.h>

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

void fatal(const char* msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}
