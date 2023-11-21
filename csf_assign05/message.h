#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <string>

// standard message tags (note that you don't need to worry about
// "senduser" or "empty" messages)
#define TAG_ERR       "err"       // protocol error
#define TAG_OK        "ok"        // success response
#define TAG_SLOGIN    "slogin"    // register as specific user for sending
#define TAG_RLOGIN    "rlogin"    // register as specific user for receiving
#define TAG_JOIN      "join"      // join a chat room
#define TAG_LEAVE     "leave"     // leave a chat room
#define TAG_SENDALL   "sendall"   // send message to all users in chat room
#define TAG_SENDUSER  "senduser"  // send message to specific user in chat room
#define TAG_QUIT      "quit"      // quit
#define TAG_DELIVERY  "delivery"  // message delivered by server to receiving client
#define TAG_EMPTY     "empty"     // sent by server to receiving client to indicate no msgs available

/*
 * A message must be a single line of text with no newline characters contained within it.
 * A message ends at a newline terminator which can be either "\n" or "\r\n". Your programs must be able to accept both of these as newline delimiters.
 * The tag must be one of the operations specified in the “tag table”.
 * The payload is an arbitrary sequence of characters. If a tag has a structured payload, the payload must be formatted exactly as specified.
 * If a tag has a payload that is ignored (e.g., the “quit” and “leave” tags), the tag/payload separator character : must still be present (e.g. quit: not quit), even if the payload is empty
 * An encoded message must not be more than MAX_LEN bytes.
 */

struct Message {
  // An encoded message may have at most this many characters,
  // including the trailing newline ('\n'). Note that this does
  // *not* include a NUL terminator (if one is needed to
  // temporarily store the encoded message.)
  static const unsigned MAX_LEN = 255;

  std::string tag; // COMMANDS (must always have :)
  std::string data; // PARAMETERS FOR COMMANDS

  Message() { }

  Message(const std::string &tag, const std::string &data)
    : tag(tag), data(data) { }

  std::string to_string() const {
    std::string ret_string;

    ret_string.append(tag);
    ret_string.append(":");
    ret_string.append(data);
    ret_string.append("\n");

    return ret_string;
  }

  bool is_valid_tag() const {
    return tag == TAG_ERR ||
           tag == TAG_OK || 
           tag == TAG_SLOGIN ||
           tag == TAG_RLOGIN ||
           tag == TAG_JOIN ||
           tag == TAG_LEAVE ||
           tag == TAG_SENDALL ||
           tag == TAG_QUIT ||
           tag == TAG_DELIVERY ||
           tag == TAG_EMPTY ||
           tag == TAG_SENDUSER;
  }
};


#endif // MESSAGE_H
