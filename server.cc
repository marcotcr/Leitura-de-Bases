#include "slb.h"
#include <iostream>
#include <string>

int main () {
    std::string port = "5555";
    SLB oi(port);
    oi.Run();
    return 0;
}
