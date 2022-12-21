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

std::vector<std::pair<void *, void *>> childContextsAndSockets;

int main(int argc, char const *argv[])
{
    int curId = std::stoi(std::string(argv[1])), flag = 1;
    void *parentContext = zmq_ctx_new();
    void *parentSocket = zmq_socket(parentContext, ZMQ_PAIR);
    void *childContext = NULL;
    void *childSocket = NULL;
    zmq_connect(parentSocket, ("tcp://localhost:" + std::to_string(PORT + curId)).c_str());

    std::map<std::string, int> localDict;
    
    while (flag) {
        nlohmann::json reply = Recv(parentSocket);
        nlohmann::json request;
        request["ans"] = "error";
        if (reply["type"] == "ping") {
            if (reply["id"] == curId) {
                request["ans"] = "ok";
            } else {
                for (int i = 0; i < childContextsAndSockets.size(); i++) {
                    nlohmann::json pingRequest;
                    pingRequest["type"] = "ping";
                    pingRequest["id"] = reply["id"];
                    nlohmann::json pingReply = sendAndRecv(pingRequest, childContextsAndSockets[i].second, 0);
                    if (pingReply["ans"] == "ok") {
                        request["ans"] = "ok";
                        break;
                    }
                }
            }
        } else if (reply["type"] == "create") {
            if (reply["parentId"] == curId) {
                childContext = zmq_ctx_new();
                childSocket = zmq_socket(childContext, ZMQ_PAIR);
                zmq_setsockopt(childSocket, ZMQ_RCVTIMEO, &WAIT_TIME, sizeof(int));
                zmq_setsockopt(childSocket, ZMQ_SNDTIMEO, &WAIT_TIME, sizeof(int));
                int i = reply["id"];
                zmq_bind(childSocket, ("tcp://*:" + std::to_string(PORT + i)).c_str());
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
                        std::cout << "OK: " << getpid() << std::endl;
                        childContextsAndSockets.push_back(std::make_pair(childContext, childSocket));
                        request["ans"] = "ok";
                    }
                }
            } else {
                nlohmann::json newRequest = reply;
                for (int i = 0; i < childContextsAndSockets.size(); i++) {
                    nlohmann::json newReply = sendAndRecv(newRequest, childContextsAndSockets[i].second, 0);
                    if (newReply["ans"] == "ok") {
                        request["ans"] = "ok";
                        break;
                    }
                }
            }
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
                    for (int i = 0; i < childContextsAndSockets.size(); i++) {
                        nlohmann::json newReply = sendAndRecv(newRequest, childContextsAndSockets[i].second, 0);
                        if (newReply["ans"] == "ok") {
                            request["ans"] = "ok";
                            break;
                        }
                    }
                }
            } else {
                if (reply["id"] == curId) {
                    std::string key = reply["key"];
                    std::cout << "ok:" << curId << ":" << localDict[key] << std::endl;
                    request["ans"] = "ok";
                } else {
                    nlohmann::json newRequest = reply;
                    for (int i = 0; i < childContextsAndSockets.size(); i++) {
                        nlohmann::json newReply = sendAndRecv(newRequest, childContextsAndSockets[i].second, 0);
                        if (newReply["ans"] == "ok") {
                            request["ans"] = "ok";
                            break;
                        }
                    }
                }
            }
        } else {
            
        }
        Send(request, parentSocket);
    }
    zmq_close(parentSocket);
    zmq_ctx_destroy(parentContext);
    return 0;
}
