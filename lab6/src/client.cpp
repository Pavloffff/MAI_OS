#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <unistd.h>
#include <csignal>
#include <thread>
#include <zmq.hpp>
#include <nlohmann/json.hpp>
#include "../include/labtools.h"

using namespace advancedZMQ;

zmq::context_t context;
zmq::socket_t demonSocket(context, zmq::socket_type::pair);

int main(int argc, char const *argv[])
{
    int demonPid = fork();
    if (demonPid == 0) {
        execl("./demon", "./demon", NULL);
        return 0;
    } else {
        demonSocket.setsockopt(ZMQ_RCVTIMEO, WAIT_TIME);
        demonSocket.setsockopt(ZMQ_SNDTIMEO, WAIT_TIME);
        demonSocket.bind(("tcp://127.0.0.1:" + std::to_string(DEMON_PORT)));
        std::cout << "\nWrite:\n command [arg1] ... [argn]\n";
        std::cout << "\ncreate [id] [parentId] to create calculation node\n";
        std::cout << "\nremove [id] [parentId] to remove calculation node\n";
        std::cout << "\nexec [id] [key:string] [value:int] to add [key:value] to map on calculation node\n";
        std::cout << "\nexec [id] [key:string] to show the value in map on calculation node\n";
        std::cout << "\npingall to show all the unreachible nodes\n";
        std::cout << "\nprint to show NTree topology\n";
        std::cout << "\nhelp to see commands again\n\n";
        std::string command;
        while (std::cin >> command) {
            nlohmann::json demonRequest;
            if (command == "create") {
                int id, parentId;
                std::cin >> id >> parentId;
                demonRequest["type"] = "create";
                demonRequest["id"] = id;
                demonRequest["parentId"] = parentId;
                Send(demonRequest, demonSocket);
            } else if (command == "remove") {
                int id;
                std::cin >> id;
                demonRequest["type"] = "remove";
                demonRequest["id"] = id;
                Send(demonRequest, demonSocket);
            } else if (command == "print") {
                demonRequest["type"] = "print";
                Send(demonRequest, demonSocket);
            } else if (command == "exec") {
                std::string key;
                int id;
                char c;
                std::cin >> id >> key;
                c = getchar();
                if (c == ' ') {
                    int value;
                    std::cin >> value;
                    demonRequest["type"] = "exec";
                    demonRequest["action"] = "add";
                    demonRequest["id"] = id;
                    demonRequest["key"] = key;
                    demonRequest["value"] = value;
                } else {
                    demonRequest["type"] = "exec";
                    demonRequest["action"] = "check";
                    demonRequest["id"] = id;
                    demonRequest["key"] = key;
                }
                Send(demonRequest, demonSocket);
            } else if (command == "pingall") {
                demonRequest["type"] = "pingall";
                Send(demonRequest, demonSocket);
            } else if (command == "help") {
                std::cout << "\nWrite:\n command [arg1] ... [argn]\n";
                std::cout << "\ncreate [id] [parentId] to create calculation node\n";
                std::cout << "\nremove [id] [parentId] to remove calculation node\n";
                std::cout << "\nexec [id] [key:string] [value:int] to add [key:value] to map on calculation node\n";
                std::cout << "\nexec [id] [key:string] to show the value in map on calculation node\n";
                std::cout << "\npingall to show all the unreachible nodes\n";
                std::cout << "\nprint to show NTree topology\n";
                std::cout << "\nhelp to see commands again\n\n";
            }
        }
        if (std::cin.eof()) {
            nlohmann::json demonRequest;
            demonRequest["type"] = "close";
            Send(demonRequest, demonSocket);
        }
        demonSocket.close();
    }
    return 0;
}
