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
std::map<int, std::pair<void *, void *>> contextsAndSockets;

int main(int argc, char const *argv[])
{
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
            int n = 0;
            if (parentId == -1) {
                void *context = zmq_ctx_new();
                void *socket = zmq_socket(context, ZMQ_PAIR);
                zmq_setsockopt(socket, ZMQ_RCVTIMEO, &WAIT_TIME, sizeof(int));
                zmq_setsockopt(socket, ZMQ_SNDTIMEO, &WAIT_TIME, sizeof(int));
                zmq_bind(socket, ("tcp://*:" + std::to_string(PORT + id)).c_str());
                if (nodes.find(id) != -1) {
                    std::cout << "Error: child " << id << " already exists" << std::endl;
                    continue;
                }
                pid_t pid = fork();
                if (pid == 0) {
                    execl("./server", "./server", std::to_string(id).c_str(), NULL);
                    return 0;
                } else {
                    nlohmann::json request;
                    request["type"] = "ping";
                    request["parentId"] = id;
                    request["id"] = id;
                    nlohmann::json reply = sendAndRecv(request, socket, 0);
                    if (reply["ans"] == "ok") {
                        // std::cout << id << "\n";
                        contextsAndSockets.insert({id, std::make_pair(context, socket)});
                        nodes.insert(-1, id);
                    } else {
                        zmq_close(socket);
                        zmq_ctx_destroy(context);
                        std::cout << "Error: parent " << parentId << " is unavailable" << std::endl;
                        continue;
                    }
                }
            } else if (n = nodes.find(parentId) == -1) {
                std::cout << "Error: parent " << parentId << " not found" << std::endl;
                continue;
            } else {
                if (nodes.find(id) != -1) {
                    std::cout << "Error: child " << id << " already exists" << std::endl;
                    continue;
                }
                nlohmann::json request;
                request["type"] = "create";
                request["parentId"] = parentId;
                request["id"] = id;
                // std::cout << "socket: " << n << "\n";
                nlohmann::json reply = sendAndRecv(request, contextsAndSockets[n].second, 0);
                if (reply["ans"] == "ok") {
                    nodes.insert(parentId, id);
                } else {
                    std::cout << "Error: parent " << parentId << " is unavailable" << std::endl;
                    continue;
                }
            }
        } else if (command == "print") {
            nodes.print();
        } else if (command == "exec") {
            std::string key;
            int value;
            std::cin >> key >> value;
        }
    }
    return 0;
}
