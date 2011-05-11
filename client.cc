//
//  Hello World client in C++
//  Connects REQ socket to tcp://localhost:5555
//  Sends "Hello" to server, expects "World" back
//
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <zmq.hpp>
#include "request.pb.h"


int main (int argc, char** argv) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  //  Prepare our context and socket
  int x;
  std::string port;
  int bytes = 0;
  std::string address;
  std::string id;
  while ((x = getopt (argc, argv, "p:b:a:i:")) != -1) {
     switch (x) {
       case 'p':
         port.assign(optarg);
       break;
       case 'b':
         bytes= atoi(optarg);
       break;
       case 'a':
         address.assign(optarg);
       case 'i':
         id.assign(optarg);
       break;
       default:
         printf("Invalid parameter!");
         exit(1);
       break;
     }
  }
  if (port == "" || bytes == 0 || address == "") {
    printf("Wrong parameters. Usage:\n");
    printf("./client -p port -a address -b number of bytes to read in each requisition -i id\n");
    exit(1);
  }
  zmq::context_t context (1);
  zmq::socket_t socket (context, ZMQ_REQ);

  std::cout << "Connecting to SLB..." << std::endl;
  std::string temp = "tcp://" + address + ":" + port;
  socket.connect (temp.c_str());
  request::Open open_request;
  open_request.set_file_name("ae");
  std::stringstream temp2;
  temp2 << id;
  int id_;
  temp2 >> id_;
  open_request.set_id(id_);
  if (!open_request.SerializeToString(&temp)) {
    std::cerr << "Failed to serialize file name" << std::endl;
  }
  zmq::message_t request(temp.length() + 1);
  memcpy ((void *) request.data (), temp.c_str(), temp.length());
  std::cout << "Sending request to open file ae " << std::endl;
  socket.send(request);
  zmq::message_t reply;
  socket.recv (&reply);
  request::OpenResponse open_response;
  if (!open_response.ParseFromString((char*) reply.data())) {
    std::cout << "Failure to parse response from string" << std::endl;
  }
  std::cout << "Received: " << open_response.file_opened() << std::endl;

  request::Read read_request;
  request::ReadResponse read_response;
  read_request.set_bytes(bytes);
  read_request.set_id(id_);
  if (!read_request.SerializeToString(&temp)) {
    std::cerr << "Failed to serialize file name" << std::endl;
  }
  zmq::message_t request2(temp.length() + 1);
  memcpy ((void *) request2.data (), temp.c_str(), temp.length());
  std::cout << "Sending request to read "<< bytes << "bytes." << std::endl;
  socket.send(request2);
  socket.recv(&reply);
  if (!read_response.ParseFromString((char*) reply.data())) {
    std::cout << "Failure to parse response from string" << std::endl;
  }
  std::cout << "Received status: " << read_response.success() << std::endl;
  if (read_response.success()) {
    std::cout << "Received text: "<< read_response.response() << std::endl;
  }
}
    

    

 /*   //  Do 10 requests, waiting each time for a response
    for (int request_nbr = 0; request_nbr != 10; request_nbr++) {
        zmq::message_t request (temp.length() + 1);
        memcpy ((void *) request.data (), temp.c_str(), temp.length());
        std::cout << "Sending request " <<std::endl;
        socket.send (request);
        //  Get the reply.
        zmq::message_t reply;
        socket.recv (&reply);
        std::cout << "Received: " << (char*)reply.data() << std::endl;
    }
    return 0;
} */
