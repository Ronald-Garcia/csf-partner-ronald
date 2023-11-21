#ifndef CLIENT_UTIL_H
#define CLIENT_UTIL_H

#include <string>
class Connection;
struct Message;

// this header file is useful for any declarations for functions
// or classes that are used by both clients (the sender and receiver)

std::string ltrim(const std::string &s);
std::string rtrim(const std::string &s);
std::string trim(const std::string &s);

int create_server_socket(int port);

int accept_connection(int ssock_fd, struct sockaddr_in *clientaddr);

void fatal(const char *msg);

void handle_delivery(Message message, std::string room);
// you can add additional declarations here...

#endif // CLIENT_UTIL_H
