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

NTree nodes;

int main(int argc, char const *argv[])
{
    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_PAIR);
    zmq_setsockopt(socket, ZMQ_RCVTIMEO, &WAIT_TIME, sizeof(int));
    zmq_setsockopt(socket, ZMQ_SNDTIMEO, &WAIT_TIME, sizeof(int));
    zmq_bind(socket, ("tcp://*:" + std::to_string(PORT - 1)).c_str());
    int pid = fork();
    if (pid == 0) {
        execl("./server", "./server", std::to_string(-1).c_str(), NULL);
        return 0;
    }
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
        if (command == "create") {
            int id, parentId;
            std::cin >> id >> parentId;
            if (nodes.find(-1, parentId) == -1 && parentId != -1) {
                std::cout << "Error: parent " << parentId << " not found" << std::endl;
                continue;
            }
            if (nodes.find(parentId, id) != -1) {
                std::cout << "Error: child " << id << " already exists" << std::endl;
                continue;
            }
            nlohmann::json pingRequest;
            pingRequest["type"] = "ping";
            pingRequest["id"] = parentId;
            nlohmann::json pingReply = sendAndRecv(pingRequest, socket, 0);
            if (pingReply["ans"] != "ok") {
                zmq_close(socket);
                zmq_ctx_destroy(context);
                std::cout << "Error: parent " << parentId << " is unavailable" << std::endl;
                continue;
            }
            nlohmann::json request;
            request["type"] = "create";
            request["id"] = id;
            request["parentId"] = parentId;
            nlohmann::json reply = sendAndRecv(request, socket, 0);
            if (reply["ans"] != "ok") {
                zmq_close(socket);
                zmq_ctx_destroy(context);
                std::cout << "Error: parent " << parentId << " is unavailable" << std::endl;
                continue;
            } else {
                nodes.insert(parentId, id);
            }
        } else if (command == "print") {
            nodes.print();
        } else if (command == "exec") {
            std::string key;
            int id;
            char c;
            std::cin >> id >> key;
            c = getchar();
            if (id == -1) {
                std::cout << "Error: control node can't do calculation" << std::endl;
                continue;
            }
            if (nodes.find(-1, id) == -1) {
                std::cout << "Error: node " << id << " not found" << std::endl;
                continue;
            }
            if (c == ' ') {
                int value;
                std::cin >> value;
                nlohmann::json request;
                request["type"] = "exec";
                request["action"] = "add";
                request["id"] = id;
                request["key"] = key;
                request["value"] = value;
                nlohmann::json reply = sendAndRecv(request, socket, 1);
                if (reply["ans"] != "ok") {
                    std::cout << "Error: node " << id << " is unavailable" << std::endl;
                    continue;
                }
            } else {
                nlohmann::json request;
                request["type"] = "exec";
                request["action"] = "check";
                request["id"] = id;
                request["key"] = key;
                nlohmann::json reply = sendAndRecv(request, socket, 1);
                if (reply["ans"] != "ok") {
                    std::cout << "Error: node " << id << " is unavailable" << std::endl;
                    continue;
                }
            }
        }
    }
    return 0;
}
