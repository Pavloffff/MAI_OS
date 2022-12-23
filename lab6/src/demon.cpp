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

//APIDEMON

zmq::context_t context;
zmq::socket_t socket(context, zmq::socket_type::pair);

zmq::socket_t demonSocket(context, zmq::socket_type::pair);

NTree nodes;

int main(int argc, char const *argv[])
{
    demonSocket.connect(("tcp://127.0.0.1:" + std::to_string(DEMON_PORT)));
    socket.setsockopt(ZMQ_RCVTIMEO, WAIT_TIME);
    socket.setsockopt(ZMQ_SNDTIMEO, WAIT_TIME);
    socket.bind(("tcp://127.0.0.1:" + std::to_string(PORT)));
    int pid = fork();
    if (pid == 0) {
        execl("./server", "./server", std::to_string(-1).c_str(), NULL);
        return 0;
    }
    int flag = 1;
    while (flag) {
        nlohmann::json demonReply;
        demonReply = Recv(demonSocket);
        if (demonReply["type"] == "create") {
            int id = demonReply["id"];
            int parentId = demonReply["parentId"];
            if (nodes.findNode(parentId).second == -1 && parentId != -1) {
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
                socket.close();
                context.close();
                std::cout << "Error: parent " << parentId << " is unavailable" << std::endl;
                continue;
            }
            nlohmann::json request;
            request["type"] = "create";
            request["id"] = id;
            request["parentId"] = parentId;
            nlohmann::json reply = sendAndRecv(request, socket, 0);
            if (reply["ans"] != "ok") {
                socket.close();
                context.close();
                std::cout << "Error: parent " << parentId << " is unavailable" << std::endl;
                continue;
            } else {
                nodes.insert(parentId, id);
            }
        } else if (demonReply["type"] == "remove") {
            int id = demonReply["id"], parentId;
            if (nodes.findNode(id).second == -1) {
                std::cout << "Error: node " << id << " not found" << std::endl;
                continue;
            }
            parentId = nodes.findNode(id).first;
            nlohmann::json pingRequest;
            pingRequest["type"] = "ping";
            pingRequest["id"] = parentId;
            nlohmann::json pingReply = sendAndRecv(pingRequest, socket, 0);
            if (pingReply["ans"] != "ok") {
                std::cout << "Error: parent " << parentId << " is unavailable" << std::endl;
                continue;
            }
            nlohmann::json request;
            request["type"] = "remove";
            request["id"] = id;
            request["parentId"] = parentId;
            nlohmann::json reply = sendAndRecv(request, socket, 0);
            if (reply["ans"] != "ok") {
                std::cout << "Error: node " << id << " is unavailable" << std::endl;
                continue;
            } else {
                nodes.destroyUndertree(id);
                std::cout << "ok" << std::endl;
            }
        } else if (demonReply["type"] == "print") {
            nodes.print();
        } else if (demonReply["type"] == "exec") {
            int id = demonReply["id"];
            if (id == -1) {
                std::cout << "Error: control node can't do calculation" << std::endl;
                continue;
            }
            if (nodes.findNode(id).second == -1) {
                std::cout << "Error: node " << id << " not found" << std::endl;
                continue;
            }
            nlohmann::json reply = sendAndRecv(demonReply, socket, 0);
            if (reply["ans"] != "ok") {
                std::cout << "Error: node " << id << " is unavailable" << std::endl;
                continue;
            }
        } else if (demonReply["type"] == "pingall") {
            nlohmann::json reply = sendAndRecv(demonReply, socket, 0);
            if (reply["ans"] == "ok") {
                std::cout << "ok" << std::endl;
            }
        } else if (demonReply["type"] == "close") {
            nlohmann::json destroyRequest;
            destroyRequest["type"] = "remove";
            std::vector<int> nodesRoot = nodes.findChilds(-1);
            for (int i = 0; i < nodesRoot.size(); i++) {
                destroyRequest["id"] = nodesRoot[i];
                sendAndRecv(destroyRequest, socket, 0);
            }
            flag = 0;
        }
    }
    socket.close();
    demonSocket.close();
    return 0;
}
