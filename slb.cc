#include "slb.h"

SLB::SLB(std::string port): port_(port) {
}
void SLB::Run() {
  int id, bytes;
  char buffer[500];
  zmq::context_t context (1);
  zmq::socket_t socket (context, ZMQ_REP);
  std::string temp = "tcp://*:" + port_;
  socket.bind (temp.c_str());
  while (true) {
    std::string reply_string;
    zmq::message_t request;

    socket.recv(&request);
    // Isso é uma marreta, mas não consegui pensar em um jeito melhor de fazer.
    // Quando ele não consegue processar um request de open, é porque é um
    // request de read, ai ele retorna 0 (embora o protobuf solte uma mensagem
    // de erro).
    if (this->ProcessOpenRequest(request, socket)) {
      continue;
    }
    if (this->ProcessReadRequest(request, socket)) {
      continue;
    }
    else  {
      std::cout << "CAI AQUI "<< std::endl;
    }
  }
}

bool SLB::ProcessOpenRequest(zmq::message_t &request, zmq::socket_t &socket) {
  request::Open open_request;
  request::OpenResponse open_response;
  std::string reply_string;
  if (!open_request.ParseFromString((char*)request.data())) {
    std::cout<< " CANT PARSE "<<std::endl;
    return 0;
  };
  if (file_handles_.count(open_request.id()) <= 0) {
    file_handles_[open_request.id()] = fopen(open_request.file_name().c_str(), "r");
    if (file_handles_[open_request.id()] == NULL) {
      open_response.set_file_opened(false);
      file_handles_.erase(open_request.id());
      std::cout << "Failure to open file " << open_request.file_name() <<
      std::endl;
    }
    else {
      open_response.set_file_opened(true);
    }
  }
  else {
    open_response.set_file_opened(true);
  }
  if (!open_response.SerializeToString(&reply_string)) {
    std::cout << "Failure to serialize response" << std::endl;
  }
  zmq::message_t reply(reply_string.length() + 1);
  memcpy((void *) reply.data(), reply_string.c_str(), reply_string.length() + 1);
  socket.send(reply);
  std::cout<<"Received a request from "<<open_request.id()<<" asking to open file "<<
  open_request.file_name() <<std::endl;
  return 1;
}

bool SLB::ProcessReadRequest(zmq::message_t &request, zmq::socket_t &socket) {
  request::Read read_request;
  request::ReadResponse read_response;
  std::string reply_string;
  if (!read_request.ParseFromString((char*) request.data())) {
    return 0;
  }
  if (file_handles_.count(read_request.id()) <= 0) {
    read_response.set_success(false);
  }
  else {
    char temp[read_request.bytes() + 1];
    if (fread(temp, 1, read_request.bytes(), file_handles_[read_request.id()])
    <= 0) {
      read_response.set_success(false);
    }
    else {
      temp[read_request.bytes()] = '\0';
      read_response.set_success(true);
      read_response.set_response(temp);
    }
  }
  if (!read_response.SerializeToString(&reply_string)) {
    std::cout << "Failure to serialize response" << std::endl;
  }
  zmq::message_t reply(reply_string.length() + 1);
  memcpy((void *) reply.data(), reply_string.c_str(), reply_string.length() + 1);
  socket.send(reply);
  return 1;
}
