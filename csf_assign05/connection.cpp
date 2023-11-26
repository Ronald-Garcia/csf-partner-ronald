#include <sstream>
#include <cctype>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"
#include <iostream>

// DONE
Connection::Connection()
  : m_fd(-1)
  , m_last_result(SUCCESS) {
}

// DONE
Connection::Connection(int fd)
  : m_fd(fd)
  , m_last_result(SUCCESS) {
  // Completed: call rio_readinitb to initialize the rio_t object
  if (m_fd < 0) {
    fatal("could not connect to server");
  }
  rio_readinitb(&m_fdbuf, m_fd);  
}

// DONE
void Connection::client_connect(const std::string &hostname, int port) {

  // Completed: make server socket
  //int ssock_fd = create_server_socket(port);
  //struct sockaddr_in clientaddr;

  //int client_fd = accept_connection(ssock_fd, &clientaddr);
  // Completed: call open_clientfd to connect to the server

  std::string p = std::to_string(port);

  m_fd = Open_clientfd(hostname.c_str(), p.c_str());
  

  if (!Connection::is_open()) {
    fatal("could not connect to server");
  }

  // Completed: call rio_readinitb to initialize the rio_t object
  rio_readinitb(&m_fdbuf, m_fd);  
}

// DONE
void Connection::server_connect(const std::string &hostname, int port) {

  // Completed: make server socket
  //int ssock_fd = create_server_socket(port);
  //struct sockaddr_in clientaddr;

  //int client_fd = accept_connection(ssock_fd, &clientaddr);
  // Completed: call open_clientfd to connect to the server

  std::string p = std::to_string(port);

  m_fd = Open_clientfd(hostname.c_str(), p.c_str());
  

  if (!Connection::is_open()) {
    fatal("could not connect to server");
  }

  // Completed: call rio_readinitb to initialize the rio_t object
  rio_readinitb(&m_fdbuf, m_fd);  
}

// DONE
Connection::~Connection() {
  if (Connection::is_open()) {
    Connection::close();
  }
}

// DONE
bool Connection::is_open() const {
  return m_fd >= 0;
}

// DONE
void Connection::close() {
  Close(m_fd);
}

// DONE
bool Connection::send(const Message &msg) {

  if (!msg.is_valid_tag()) {
    m_last_result = INVALID_MSG;
    return false;
  }

  // Completed: send a message
  Rio_writen(m_fd, msg.to_string().c_str(), msg.to_string().size() + 1);

  Message confirmation_msg;

  // Completed: return true if successful, false if not
  // Completed: make sure that m_last_result is set appropriately

  bool received_confirmation = Connection::receive(confirmation_msg);
  
  if (!received_confirmation || confirmation_msg.tag == std::string(TAG_ERR)) {
    m_last_result = EOF_OR_ERROR;
    if (received_confirmation && confirmation_msg.tag == std::string(TAG_ERR)) {
      std::cerr << confirmation_msg.data << std::endl;
    }
    return false;
  }

  m_last_result = SUCCESS;
  return true;
}

// DONE
bool Connection::receive(Message &msg) {

  char buf[msg.MAX_LEN + 1];

  size_t bytes_read = Rio_readlineb(&m_fdbuf, buf, msg.MAX_LEN);

  // if nothing was read
  if (bytes_read <= 0) {
    m_last_result = EOF_OR_ERROR;
    return false;
  }

  // otherwise, initialize message
  std::string cpp_buf(buf);
  std::string delimiter = ":";
  int colon_index = cpp_buf.find(delimiter);
  std::string tag = trim(cpp_buf.substr(0, colon_index));
  std::string data = trim(cpp_buf.substr(colon_index + 1));

  // Completed: receive a message, storing its tag and data in msg
  msg.tag = tag;
  msg.data = data;

  // return true if valid message
  if (!msg.is_valid_tag()) {
    m_last_result = INVALID_MSG;
    return false;
  }

  // Completed: return true if successful, false if not
  // Completed: make sure that m_last_result is set appropriately
  m_last_result = SUCCESS;
  return true;

}
