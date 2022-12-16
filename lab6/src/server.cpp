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

int main(int argc, char const *argv[])
{
    int curId = std::stoi(std::string(argv[1])), childId = -1, hasChild = 0, flag = 1;
    // std::cout << "curID[" << getpid() <<"]: " << curId << std::endl;
    void *parentContext = zmq_ctx_new();
    void *parentSocket = zmq_socket(parentContext, ZMQ_PAIR);
    void *childContext = NULL;
    void *childSocket = NULL;
    zmq_connect(parentSocket, ("tcp://localhost:" + std::to_string(PORT + curId)).c_str());
    std::cout << "OK: " << getpid() << std::endl;
    while (1) {
        // std::cout << "parentSocket[" << getpid() <<"]: " << parentSocket << std::endl;
        // std::cout << "hasChild[" << getpid() <<"]: " << hasChild << "\n";
        nlohmann::json reply = Recv(parentSocket);
        // std::cout << "compare:[" << getpid() <<"]: " << reply["id"] << " " << curId << std::endl;
        nlohmann::json request;
        request["ans"] = "error";
        if (reply["type"] == "ping") {
            // std::cout << "hello[" << getpid() <<"]\n";
            if (reply["id"] == curId) {
                // std::cout << "hello[" << getpid() <<"]\n";
                request["ans"] = "ok";
            } else if (hasChild == 1) {
                // std::cout << "ChildSocket[" << getpid() <<"]: " << childSocket << std::endl;
                nlohmann::json pingRequest;
                pingRequest["type"] = "ping";
                pingRequest["id"] = reply["id"];
                pingRequest["parentId"] = reply["parentId"];
                nlohmann::json pingReply = sendAndRecv(pingRequest, childSocket, 0);
                if (pingReply["ans"] == "ok") {
                    request["ans"] = "ok";
                }
            }
        } else if (reply["type"] == "create") {
            if (reply["parentId"] == curId) {
                if (hasChild == 1) {
                    zmq_close(childSocket);
                    zmq_ctx_term(childContext);
                }
                childContext = zmq_ctx_new();
                childSocket = zmq_socket(childContext, ZMQ_PAIR);
                zmq_setsockopt(childSocket, ZMQ_RCVTIMEO, &WAIT_TIME, sizeof(int));
                zmq_setsockopt(childSocket, ZMQ_SNDTIMEO, &WAIT_TIME, sizeof(int));
                int i = reply["id"];
                zmq_bind(childSocket, ("tcp://*:" + std::to_string(PORT + i)).c_str());
                int pid = fork();
                if (pid == 0) {
                    int i = reply["id"];
                    // std::cout << "replyId:[" << getpid() <<"]: " << i << std::endl;
                    execl("./server", "./server", std::to_string(i).c_str(), NULL);
                    return 0;
                } else {
                    int ch = 1;
                    if (hasChild == 1) {
                        nlohmann::json bindRequest;
                        bindRequest["type"] = "bind";
                        bindRequest["parentId"] = reply["id"];
                        bindRequest["id"] = childId;
                        // std::cout << "bindRequest[" << getpid() <<"]: " << bindRequest << std::endl;
                        nlohmann::json bindReply = sendAndRecv(bindRequest, childSocket, 0);
                        if (bindReply["ans"] != "ok") {
                            ch = !ch;
                        }
                    }
                    if (ch) {
                        // std::cout << "childSocket[" << getpid() <<"]: " << childSocket << std::endl;
                        nlohmann::json pingRequest;
                        pingRequest["type"] = "ping";
                        pingRequest["id"] = reply["id"];
                        pingRequest["parentId"] = reply["id"];
                        nlohmann::json pingReply = sendAndRecv(pingRequest, childSocket, 0);
                        if (pingReply["ans"] != "ok") {
                            ch = !ch;
                        }
                        if (ch) {
                            request["ans"] = "ok";
                            childId = reply["id"];
                            hasChild = 1;
                        } else {
                            zmq_close(childSocket);
                            zmq_ctx_term(childContext);
                        }
                    }
                }
            } else if (hasChild == 1) {
                nlohmann::json newRequest = reply;
                nlohmann::json newReply = sendAndRecv(newRequest, childSocket, 0);
                if (newReply["ans"] == "ok") {
                    request["ans"] = "ok";
                }
            }
        } else if (reply["type"] == "bind") {
            if (reply["parentId"] == curId) {
                childContext = zmq_ctx_new();
                childSocket = zmq_socket(childContext, ZMQ_PAIR);
                zmq_setsockopt(childSocket, ZMQ_RCVTIMEO, &WAIT_TIME, sizeof(int));
                zmq_setsockopt(childSocket, ZMQ_SNDTIMEO, &WAIT_TIME, sizeof(int));
                int i = reply["id"];
                zmq_bind(childSocket, ("tcp://*:" + std::to_string(PORT + i)).c_str());
                hasChild = 1;
                childId = reply["id"];
                nlohmann::json pingRequest;
                pingRequest["type"] = "ping";
                pingRequest["id"] = childId;
                pingRequest["parentId"] = childId;
                nlohmann::json pingReply = sendAndRecv(request, childSocket, 0);
                if (pingReply["ans"] == "ok") {
                    request["ans"] = "ok";
                }
            }
        } else {
            // std::cout << request << "\n";
            // std::cout << hasChild << "\n";
        }
        // std::cout << "finalRequest[" << getpid() <<"]: " << request << "\n";
        Send(request, parentSocket);
    }
    zmq_close(parentSocket);
    zmq_ctx_destroy(parentContext);
    return 0;
}
