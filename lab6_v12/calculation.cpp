#include <iostream>
#include <unistd.h>
#include "messages.hpp"

void computeLPSArray(std::string &pattern, std::vector<int> &lps)
{
    int len = 0;
    int i = 1;
    lps[0] = 0;
    while (i < pattern.length()) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        }
        else {
            if (len != 0) {
                len = lps[len-1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

void KMPSearch(std::string text, std::string pattern)
{
    int m = pattern.length();
    int n = text.length();
    std::vector<int> lps(m);
    computeLPSArray(pattern, lps);
    int i = 0;
    int j = 0;
    while (i < n) {
        if (pattern[j] == text[i]) {
            j++;
            i++;
        }
        if (j == m) {
            std::cout <<  i - j << ';';
            j = lps[j - 1];
        }
        else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    int curId = atoi(argv[1]), childId = -1;
    bool hasChild = false, flag = true;
    zmq::context_t parentCtx;
    zmq::socket_t parentSock(parentCtx, zmq::socket_type::pair);
    parentSock.connect(("tcp://127.0.0.1:" + std::to_string(PORT + curId)).c_str());
    zmq::context_t nodeCtx;
    zmq::socket_t nodeSock;
    std::cout << "OK: " << getpid() << std::endl;
    while (flag) {
        nlohmann::json reply = Recv(parentSock);
        nlohmann::json request;
        request["ans"] = "error";
        request["type"] = reply["type"];
        request["id"] = reply["id"];
        if (reply["type"] == "ping") {
            if (reply["id"] == curId) {
                request["ans"] = "ok";
            } else if (hasChild) {
                nlohmann::json downRequest = reply;
                nlohmann::json downReply = sendAndRecv(downRequest, nodeSock, 0);
                if (downReply["ans"] == "ok") {
                    request = downReply;
                }
            }
        } else if (reply["type"] == "create") {
            if (reply["parentId"] == curId) {
                if (!hasChild) {
                    int i = reply["id"];
                    initSock(nodeCtx, nodeSock);
                    nodeSock.bind(("tcp://*:" + std::to_string(PORT + i)).c_str());
                    pid_t pid = fork();
                    if (pid == 0) {
                        execl("./calculation", "./calculation", std::to_string(i).c_str(), nullptr);
                        return 0;
                    } else {
                        if (!hasChild) {
                            childId = reply["id"];
                            hasChild = true;
                            request["ans"] = "ok";
                        } else {
                            destroySock(nodeCtx, nodeSock);
                        }
                    }
                }
            } else if (hasChild) {
                nlohmann::json downRequest = reply;
                nlohmann::json downReply = sendAndRecv(downRequest, nodeSock, 0);
                if (downReply["ans"] == "ok") {
                    request = downReply;
                }
            }
        } else if (reply["type"] == "remove") {
            if (reply["id"] == curId) {
                if (!hasChild) {
                    flag = false;
                    request["ans"] = "ok";
                }
            } else if (reply["id"] == childId) {
                hasChild = false;
                request["ans"] = "ok";
                nlohmann::json downRequest = reply;
                nlohmann::json downReply = sendAndRecv(downRequest, nodeSock, 0);
                if (downReply["ans"] == "ok") {
                    request = downReply;
                    request["ans"] = "ok";
                    request["parentId"] = curId;
                    destroySock(nodeCtx, nodeSock);
                }
            } else {
                nlohmann::json downRequest = reply;
                nlohmann::json downReply = sendAndRecv(downRequest, nodeSock, 0);
                if (downReply["ans"] == "ok") {
                    request = downReply;
                }
            }
        } else if (reply["type"] == "exec") {
            if (reply["id"] == curId) {
                std::string string = reply["string"];
                std::string pattern = reply["pattern"];
                std::cout << curId << ':';
                KMPSearch(string, pattern);
                std::cout << std::endl;
                request["ans"] = "ok";
            } else {
                nlohmann::json downRequest = reply;
                nlohmann::json downReply = sendAndRecv(downRequest, nodeSock, 0);
                if (downReply["ans"] == "ok") {
                    request = downReply;
                }
            }
        }
        Send(request, parentSock);
    }
    parentSock.disconnect(("tcp://127.0.0.1:" + std::to_string(PORT + curId)).c_str());
    destroySock(parentCtx, parentSock);
    destroySock(nodeCtx, nodeSock);
    return 0;
}
