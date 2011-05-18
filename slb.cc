#include "slb.h"

SLB::SLB(std::string port): port_(port), current_ID_(0) {
}
void SLB::Run() {
  zmq::context_t context (1);
  zmq::socket_t socket (context, ZMQ_REP);
  std::string temp = "tcp://*:" + port_;
  socket.bind (temp.c_str());
  while (true) {
    std::string reply_string;
    std::string request = Z_Utils::ReceiveString(socket);
    SLBMessage slb_message;
    if (!slb_message.ParseFromString(request)) {
      std::cout<< " CANT PARSE SLB_MESSAGE"<<std::endl;
      continue;
    };
    switch (slb_message.type()) {
      case OPEN_REQUEST:
        this->ProcessOpenRequest(slb_message, socket);
      break;
      case READ_REQUEST:
        this->ProcessReadRequest(slb_message, socket);
      break;
    }
  }
}

bool SLB::ProcessOpenRequest(const SLBMessage& slb_message,
zmq::socket_t &socket) {
  request::Open open_request;
  request::OpenResponse open_response;
  std::string reply_string;
  if (!open_request.ParseFromString(slb_message.data())) {
    std::cout<< " CANT PARSE OPEN_REQUEST"<<std::endl;
    return 0;
  }
  FILE* temp_file = fopen(open_request.file_name().c_str(), "r");
  if (temp_file != NULL) {
    open_response.set_operation_successful(1);
    open_response.set_attributed_id(current_ID_);
    file_handles_[current_ID_] = temp_file;
    current_ID_++;
  }
  else {
    std::cout << "Could not open the requested file " << std::endl;
    open_response.set_operation_successful(false);
    open_response.set_error_message("Could not open the requested file");
  }
  if (!open_response.SerializeToString(&reply_string)) {
    std::cout << "Failure to serialize OpenResponse" << std::endl;
  }
  SLBMessage slb_response;
  slb_response.set_type(OPEN_RESPONSE);
  slb_response.set_data(reply_string);

  if (!slb_response.SerializeToString(&reply_string)) {
    std::cout << "Failure to serialize slb response" << std::endl;
  }

  SLBMessage slb_z;
  if (!slb_z.ParseFromString(reply_string)) {
    std::cout << "FAIL TO PARSE "<<std::endl;
  }
  request::OpenResponse temp;
  if (!temp.ParseFromString(slb_z.data())) {
    std::cout << "Could not parse from slb_message" << std::endl;
  }
  std::cout<<std::endl;
  Z_Utils::SendString(socket, reply_string);
  std::cout<<"Received a request from "<< current_ID_ - 1 <<" asking to open file "<<
  open_request.file_name() <<std::endl;
  std::cout<<"Replied with ";
  if (open_response.operation_successful() == true) {
    std::cout << "success"<< std::endl;
  }
  else { 
    std::cout << "Error: "<< open_response.error_message() << std::endl;
  }
  return 1;
}

bool SLB::ProcessReadRequest(const SLBMessage& slb_message,
zmq::socket_t &socket) {
  request::Read read_request;
  request::ReadResponse read_response;
  std::string reply_string;
  if (!read_request.ParseFromString(slb_message.data())) {
    return 0;
  }
  if (file_handles_.count(read_request.id()) <= 0) {
    read_response.set_operation_successful(false);
    read_response.set_error_message("Id non-existant");
  }
  else {
    char temp[read_request.bytes() + 1];
    int bytes_read;
    bytes_read = fread(temp, 1, read_request.bytes(),
    file_handles_[read_request.id()]);
    if (bytes_read <= 0) {
      read_response.set_operation_successful(false);
      read_response.set_error_message("Finished reading file");
    }
    else {
      temp[bytes_read] = '\0';
      read_response.set_operation_successful(true);
      read_response.set_response(temp);
    }
  }
  if (!read_response.SerializeToString(&reply_string)) {
    std::cout << "Failure to serialize read response" << std::endl;
  }
  SLBMessage slb_response;
  slb_response.set_type(READ_RESPONSE);
  slb_response.set_data(reply_string);
  if (!slb_response.SerializeToString(&reply_string)) {
    std::cout << "Failure to serialize slb response" << std::endl;
  }

  Z_Utils::SendString(socket, reply_string);
  return 1;
}
