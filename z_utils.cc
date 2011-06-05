#include "z_utils.h"
bool Z_Utils::SendString (zmq::socket_t& socket,
const std::string & string) {
    zmq::message_t message(string.size());
    memcpy(message.data(), string.data(), string.size());

    bool rc = socket.send(message);
    return (rc);
}

std::string Z_Utils::ReceiveString(zmq::socket_t & socket) {
    zmq::message_t message;
    socket.recv(&message);
    return std::string(static_cast<char*>(message.data()), message.size());
}
