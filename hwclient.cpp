//
//  Hello World client in C++
//  Connects REQ socket to tcp://localhost:5555
//  Sends "Hello" to server, expects "World" back
//
#include <string>
#include <iostream>
#include <cstdlib>
#include <zmq.hpp>

int main (int argc, char** argv) {
  //  Prepare our context and socket
  int x;
  std::string port;
  std::string bytes;
  std::string address;
  std::string id;
  while ((x = getopt (argc, argv, "p:b:a:i:")) != -1) {
     switch (x) {
       case 'p':
         port.assign(optarg);
       break;
       case 'b':
         bytes.assign(optarg);
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
  if (port == "" || bytes == "" || address == "") {
    printf("Wrong parameters. Usage:\n");
    printf("./client -p port -a address -b number of bytes to read in each requisition -i id\n");
    exit(1);
  }
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REQ);

    std::cout << "Connecting to SLB..." << std::endl;
    std::string temp = "tcp://" + address + ":" + port;
    socket.connect (temp.c_str());

    temp = id + " - " + bytes;
    //  Do 10 requests, waiting each time for a response
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
}
