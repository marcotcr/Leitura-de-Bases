// This still needs to be made pretty.
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <zmq.hpp>
#include "request.pb.h"
#include "slb_message.pb.h"
#include "z_utils.h"


int main (int argc, char** argv) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  //  Prepare our context and socket
  int x;
  std::string port;
  int bytes = 0;
  std::string address;
  std::string file_name;
  while ((x = getopt (argc, argv, "p:b:a:f:")) != -1) {
     switch (x) {
       case 'p':
         port.assign(optarg);
       break;
       case 'b':
         bytes= atoi(optarg);
       break;
       case 'a':
         address.assign(optarg);
       break;
       case 'f':
         file_name.assign(optarg);
       break;
       default:
         printf("Invalid parameter!");
         exit(1);
       break;
     }
  }
  if (port == "" || bytes == 0 || address == "" || file_name == "") {
    printf("Wrong parameters. Usage:\n");
    printf("./client -f filename -p port -a address -b number of bytes to read in each requisition\n");
    exit(1);
  }
  zmq::context_t context (1);
  zmq::socket_t socket (context, ZMQ_REQ);

  std::cout << "Connecting to SLB..." << std::endl;
  std::string temp = "tcp://" + address + ":" + port;
  socket.connect (temp.c_str());
  request::Open open_request;
  open_request.set_file_name(file_name);
  //open_request.set_id(id_);
  if (!open_request.SerializeToString(&temp)) {
    std::cerr << "Failed to serialize file name" << std::endl;
  }
  SLBMessage slb_message;
  slb_message.set_type(1);
  slb_message.set_data(temp);
  if (!slb_message.SerializeToString(&temp)) {
    std::cout << "Failed to serialize slb_message" << std::endl;
    
  }
  std::cout << "Sending request to open file ae " << std::endl;
  Z_Utils::SendString(socket, temp);
  zmq::message_t reply;
  std::string data = Z_Utils::ReceiveString(socket);

  SLBMessage slb_response;
  if (!slb_response.ParseFromString(data)) {
    std::cout << "Failure to parse slb_message from response" << std::endl;
    exit(1);
  }
  request::OpenResponse open_response;
  int id;
  if (!open_response.ParseFromString(slb_response.data())) {
    std::cout << "Failure to parse response from slb_message" << std::endl;
    exit(1);
  }
  if (open_response.operation_successful()) {
    std::cout << "Operation successful!" << std::endl;
    id = open_response.attributed_id();
    std::cout << "Received ID: " << open_response.attributed_id() << std::endl;
  }
  else {
    std::cout << "Operation failed" << std::endl;
    std::cout << "Error: " << open_response.error_message() << std::endl;
    exit(1);
  }
  for (int i = 0; i < 2; ++i) {
    request::Read read_request;
    request::ReadResponse read_response;
    read_request.set_bytes(bytes);
    read_request.set_id(id);
   // read_request.set_id(id_);
    if (!read_request.SerializeToString(&temp)) {
      std::cerr << "Failed to serialize file name" << std::endl;
    }
    slb_message.set_type(3);
    slb_message.set_data(temp);
    if (!slb_message.SerializeToString(&temp)) {
      std::cout << "Unable to serialize slb for read request" << std::endl;
    }
  
    Z_Utils::SendString(socket, temp);
    data = Z_Utils::ReceiveString(socket);
  
    if (!slb_response.ParseFromString(data)) {
      std::cout << "Failure to parse slb_response from string" << std::endl;
    }
    if (!read_response.ParseFromString(slb_response.data())) {
      std::cout << "Failure to parse read response from string" << std::endl;
    }
    std::cout << "Received status: " << read_response.operation_successful() << std::endl;
    if (read_response.operation_successful()) {
      std::cout << "Received text: "<< read_response.response() << std::endl;
    }
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
