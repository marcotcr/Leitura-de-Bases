#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>
#include <map>

#ifndef Z_UTILS_H
#define Z_UTILS_H
class Z_Utils {
 public:
  //  Convert string to 0MQ string and send to socket
  static bool SendString (zmq::socket_t& socket, const std::string& string);
  //  Receive 0MQ string from socket and convert into string
  static std::string ReceiveString (zmq::socket_t& socket);
};
#endif // Z_UTILS_H
