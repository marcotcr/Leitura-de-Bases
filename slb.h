// This still needs to be made pretty.
#ifndef SLB_
#define SLB_

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



// This class defines the base reading service. At the time, there is one base
// reading service for the whole application. It can currently read from any
// file.
// FIXME: CHANGE THIS NAME (SLB is in portuguese)
class SLB {
 public:
  // Initializes the SLB such that it receives messages through the defined
  // port.
  SLB(std::string port);
  // Runs the base reading service. This basically receives requests and replies
  // accordingly.
  void Run();
 private:
  // Tries to open the requested file. Returns true if a reply was sent, false
  // if the request could not be parsed. This replies to the requester  with the
  // ID assigned to the requester if everything went well, or an error message
  // otherwise.
  bool ProcessOpenRequest(const SLBMessage& slb_message, zmq::socket_t &socket);

  // Reads the number of bytes requested from the file associated with the
  // requester's ID. Returns the data to the requester if successful, and an
  // error message  otherwise. Returns an error message to the requester if EOF
  // is found. This function returns 1 if a reply was sent, false otherwise.
  bool ProcessReadRequest(const SLBMessage& slb_message, zmq::socket_t &socket);
  // Maps requesters to their respective file handles.
  std::map<int, FILE*> file_handles_;
  std::string port_;
  // Keeps track of current ID given to open requests.
  int current_ID_;
};

#endif // SLB_
