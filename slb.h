//
//  Hello World server in C++
//  Binds REP socket to tcp://*:5555
//  Expects "Id - size" from client, replies with size bytes.
//
#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>
#include <map>
#include "request.pb.h"

class SLB {
 public:
  SLB(std::string port);
  void Run();
 private:
  bool ProcessOpenRequest(zmq::message_t &request, zmq::socket_t &socket);
  bool ProcessReadRequest(zmq::message_t &request, zmq::socket_t &socket);
  std::map<int, FILE*> file_handles_;
  std::string port_;
};


