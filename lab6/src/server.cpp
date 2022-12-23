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

std::vector<zmq::socket_t> childSockets;
std::vector<int> childIds;

int main(int argc, char const *argv[])
{
    int curId = std::stoi(std::string(argv[1])), flag = 1;
    
    zmq::context_t parentContext(1);
    zmq::socket_t parentSocket(parentContext, zmq::socket_type::pair);
    parentSocket.connect(("tcp://127.0.0.1:" + std::to_string(PORT + curId + 1)));

    std::map<std::string, int> localDict;
    void *arg = NULL;
    while (flag) {
        nlohmann::json reply = Recv(parentSocket);
        nlohmann::json request;
        request["type"] = reply["type"];
        request["id"] = curId;
        request["pid"] = 0;
        request["ans"] = "error";
        if (reply["type"] == "ping") {
            if (reply["id"] == curId) {
                request["ans"] = "ok";
                request["id"] = curId;
                request["pid"] = getpid();
            } else {
                for (int i = 0; i < childSockets.size(); i++) {
                    nlohmann::json pingRequest;
                    pingRequest["type"] = "ping";
                    pingRequest["id"] = reply["id"];
                    nlohmann::json pingReply = sendAndRecv(pingRequest, childSockets[i], 0);
                    if (pingReply["ans"] == "ok") {
                        request["ans"] = "ok";
                        request["id"] = curId;
                        request["pid"] = getpid();
                        break;
                    }
                }
            }
        } else if (reply["type"] == "create") {
            if (reply["parentId"] == curId) {
                int i = reply["id"];
                zmq::socket_t childSocket(parentContext, zmq::socket_type::pair);
                childSocket.setsockopt(ZMQ_RCVTIMEO, WAIT_TIME);
                childSocket.setsockopt(ZMQ_SNDTIMEO, WAIT_TIME);
                childSocket.bind(("tcp://*:" + std::to_string(PORT + i + 1)).c_str());
                int pid = fork();
                if (pid == 0) {
                    int i = reply["id"];
                    execl("./server", "./server", std::to_string(i).c_str(), NULL);
                    return 0;
                } else {
                    nlohmann::json pingRequest;
                    pingRequest["type"] = "ping";
                    pingRequest["id"] = reply["id"];
                    nlohmann::json pingReply = sendAndRecv(pingRequest, childSocket, 0);
                    if (pingReply["ans"] == "ok") {
                        std::cout << "OK: " << pingReply["pid"] << std::endl;
                        int i = reply["id"];
                        childSockets.push_back(std::move(childSocket));
                        childIds.push_back(reply["id"]);
                        request["ans"] = "ok";
                        request["parentId"] = reply["parentId"];
                    }
                }
            } else {
                nlohmann::json newRequest = reply;
                for (int i = 0; i < childSockets.size(); i++) {
                    nlohmann::json newReply = sendAndRecv(newRequest, childSockets[i], 0);
                    if (newReply["ans"] == "ok") {
                        request["ans"] = "ok";
                        break;
                    }
                }
            }
        } else if (reply["type"] == "remove") {
            int c = 0;
            for (int i = 0; i < childSockets.size(); i++) {
                int childId = childIds[i];
                nlohmann::json destroyRequest;
                destroyRequest["type"] = "destroy";
                if (childId == reply["id"]) {
                    Send(destroyRequest, childSockets[i]);
                    int k = reply["id"];
                    childSockets.erase(std::next(childSockets.begin() + i));
                    childIds.erase(childIds.begin() + i);
                    c++;
                    break; 
                }
            }
            if (c > 0) {
                request["ans"] = "ok";
                c = 0;
            }
            for (int i = 0; i < childSockets.size(); i++) {
                nlohmann::json newReply = sendAndRecv(reply, childSockets[i], 0);
                if (newReply["ans"] == "ok") {
                    request["ans"] = "ok";
                }
            }
        } else if (reply["type"] == "destroy") {
            nlohmann::json newRequest = reply;
            for (int i = 0; i < childSockets.size(); i++) {
                int childId = childIds[i];
                Send(newRequest, childSockets[i]);
            }
            flag = 0;
        } else if (reply["type"] == "exec") {
            if (reply["action"] == "add") {
                if (reply["id"] == curId) {
                    std::string key = reply["key"];
                    int value = reply["value"];
                    localDict.insert({key, value});
                    std::cout << "ok:" << curId << ":" << key << ":" << value << std::endl;
                    request["ans"] = "ok";
                } else {
                    nlohmann::json newRequest = reply;
                    for (int i = 0; i < childSockets.size(); i++) {
                        nlohmann::json newReply = sendAndRecv(newRequest, childSockets[i], 0);
                        if (newReply["ans"] == "ok") {
                            request["ans"] = "ok";
                            break;
                        }
                    }
                }
            } else {
                if (reply["id"] == curId) {
                    std::string key = reply["key"];
                    if (localDict.find(key) != localDict.cend()) {
                        std::cout << "ok:" << curId << ":" << localDict[key] << std::endl;
                        request["ans"] = "ok";
                    } else {
                        std::cout << "Error: key " << key << " not found" << std::endl;
                        request["ans"] = "ok";
                    }
                } else {
                    nlohmann::json newRequest = reply;
                    for (int i = 0; i < childSockets.size(); i++) {
                        nlohmann::json newReply = sendAndRecv(newRequest, childSockets[i], 0);
                        if (newReply["ans"] == "ok") {
                            request["ans"] = "ok";
                            break;
                        }
                    }
                }
            }
        } else if (reply["type"] == "pingall") {
            nlohmann::json newRequest;
            newRequest["type"] = "pingall";
            for (int i = 0; i < childSockets.size(); i++) {
                nlohmann::json newReply = sendAndRecv(newRequest, childSockets[i], 0);
                if (newReply["ans"] == "ok") {
                    request["ans"] = "ok";
                } else {
                    std::cout << childIds[i] << std::endl;
                }
            }
            request["ans"] = "ok";
        }
        Send(request, parentSocket);
    }
    for (int i = 0; i < childSockets.size(); i++) {
        childSockets[i].close();
    }
    parentSocket.close();
    return 0;
}
