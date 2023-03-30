#include <unistd.h>
#include <vector>
#include <regex>
#include "messages.hpp"
#include "list.hpp"

std::vector<std::pair<zmq::context_t, zmq::socket_t>> childCtxAndSock;
std::vector<int> pingNodes;
NList topology;

void remove(int id)
{
    int curInd;
    curInd = topology.find(id);
    if (curInd != -1) {
        nlohmann::json request;
        request["type"] = "remove";
        request["parentId"] = -1;
        request["id"] = id;
        nlohmann::json reply = sendAndRecv(request, childCtxAndSock[curInd].second, 0);
        if (reply["ans"] == "ok") {
            topology.erace(id);
            std::cout << "OK: " << id << std::endl;
        } else {
            std::cout << "Error: parent " << id << " is unavailable" << std::endl;
        }
    } else {
        std::cout << "Error: node " << id << " not found" << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    bool check = true;
    std::string command;
    std::cout << "\nWrite:\n command [arg1] ... [argn]\n";
    std::cout << "\ncreate [id] [parentId] to create calculation node\n";
    std::cout << "\nremove [id] to remove calculation node\n";
    std::cout << "\nexec [string] [pattern] to find indexes of patterns finded in string\n";
    std::cout << "\nping [id] to check node\n";
    std::cout << "\nprint to print topology\n";
    std::cout << "\nhelp to see commands again\n\n";
    while (std::cin >> command) {
        if (command == "create") {
            int id, parentId, curInd;
            std::cin >> id >> parentId;
            curInd = topology.find(parentId);
            if (parentId == -1) {
                zmq::context_t newCtx;
                zmq::socket_t newSock;
                initSock(newCtx, newSock);
                newSock.bind(("tcp://127.0.0.1:" + std::to_string(PORT + id)).c_str());
                pid_t pid = fork();
                if (pid == 0) {
                    execl("./calculation", "./calculation", std::to_string(id).c_str(), nullptr);
                    return 0;
                } else {
                    nlohmann::json request;
                    request["type"] = "ping";
                    request["id"] = id;
                    nlohmann::json reply = sendAndRecv(request, newSock, 0);
                    if (reply["ans"] == "ok") {
                        childCtxAndSock.emplace_back(std::make_pair(std::move(newCtx), std::move(newSock)));
                        topology.insert(id);
                    } else {
                        destroySock(newCtx, newSock);
                    }
                }
            } else if (curInd == -1) {
                std::cout << "Error: parent " << parentId << " not found" << std::endl;
                continue;
            } else {
                if (topology.find(id) != -1) {
                    std::cout << "Error: child " << id << " is already exists" << std::endl;
                    continue;
                }
                nlohmann::json request;
                request["type"] = "create";
                request["parentId"] = parentId;
                request["id"] = id;
                nlohmann::json reply = sendAndRecv(request, childCtxAndSock[curInd].second, 0);
                if (reply["ans"] == "ok") {
                    topology.insert(parentId, id);
                } else {
                    std::cout << "Error: parent " << parentId << " is unavailable" << std::endl;
                }
            }
        } else if (command == "remove") {
            int id;
            std::cin >> id;
            remove(id);
        } else if (command == "print") {
            std::cout << topology;
        } else if (command == "ping") {
            int id, curInd;
            std::cin >> id;
            curInd = topology.find(id);
            if (curInd != -1) {
                nlohmann::json request;
                request["type"] = "ping";
                request["id"] = id;
                nlohmann::json reply = sendAndRecv(request, childCtxAndSock[curInd].second, 0);
                if (reply["ans"] == "ok") {
                    std::cout << "OK: " << id << std::endl;
                } else {
                    std::cout << "Error: node " << id << " is unavailable" << std::endl;
                }
            } else {
                std::cout << "Error: node " << id << " not found" << std::endl;
            }
        } else if (command == "exec") {
            int id, curInd;
            std::string string, pattern;
            std::cin >> id >> string >> pattern;
            if (topology.find(id) != -1) {
                curInd = topology.find(id);
                nlohmann::json request;
                request["type"] = "exec";
                request["id"] = id;
                request["string"] = string;
                request["pattern"] = pattern;
                nlohmann::json reply = sendAndRecv(request, childCtxAndSock[curInd].second, 0);
                if (reply["ans"] != "ok") {
                    std::cout << "Error: node " << id << " is unavailable" << std::endl;
                }
            } else {
                std::cout << "Error: node " << id << " not found" << std::endl;
            }
        }
    }
    while (topology.data.size() > 0) {
        auto i = topology.data.begin();
        int indTmp = topology.data.size() - 1;
        while (indTmp--) {
            ++i;
        }
        auto j = i->begin();
        indTmp = i->size() - 1;
        while (indTmp--) {
            ++j;
        }
        int id = *j;
        remove(id);
        if (topology.find(id) != -1) {
            topology.erace(id);
        }
    }
    return 0;
}
