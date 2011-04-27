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

class SLB {
 public:
  SLB(std::string port, std::string filename):
  port_(port), filename_(filename) { }
  void Run() {
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REP);
    std::string temp = "tcp://*:" + port_;
    socket.bind (temp.c_str());
    int id, bytes;
    char buffer[500];
    while (true) {
      zmq::message_t request;
      socket.recv(&request);
      if (sscanf((char*)request.data(), "%d - %d", &id, &bytes) != 2) {
        continue;
      }
      std::cout<<"Received a request from "<<id<<", wanting "<<bytes<<" bytes"<<std::endl;
      if (file_handles_.count(id) <= 0) {
        file_handles_[id] = fopen(filename_.c_str(), "r");
      }
      sleep(1);
      zmq::message_t reply (bytes + 1);
      fread(buffer, 1, bytes, file_handles_[id]);
      buffer[bytes] = '\0';
      memcpy ((void *) reply.data (), buffer, bytes + 1);
      socket.send (reply);
    }
  }
 private:
  std::map<int, FILE*> file_handles_;
  std::string port_;
  std::string filename_;
};


int main () {
    std::string port = "5555";
    SLB oi(port, "ae");
    oi.Run();
    return 0;
}
