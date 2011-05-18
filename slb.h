// This still needs to be made pretty.
#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>
#include <map>
#include "request.pb.h"
#include "slb_message.pb.h"
#include "z_utils.h"
#define OPEN_REQUEST 1
#define OPEN_RESPONSE 2
#define READ_REQUEST 3
#define READ_RESPONSE 4


class SLB {
 public:
  SLB(std::string port);
  void Run();
 private:
  bool ProcessOpenRequest(const SLBMessage& slb_message, zmq::socket_t &socket);
  bool ProcessReadRequest(const SLBMessage& slb_message, zmq::socket_t &socket);
  std::map<int, FILE*> file_handles_;
  std::string port_;
  // Keeps track of current ID given to open requests.
  int current_ID_;
};

