#ifndef __LABTOOLS_H__
#define __LABTOOLS_H__

#include <iostream>
#include <map>
#include <set>
#include <zmq.hpp>
#include <nlohmann/json.hpp>

#define CHECK_ERROR(expr, err, message) \
    do { \
        auto __result = (expr); \
        if (__result == err) { \
            fprintf(stderr, "Error: %s\n", message); \
            fprintf(stderr, "errno = %s, file %s, line %d\n", strerror(errno), \
                    __FILE__, __LINE__); \
            exit(-1); \
        } \
    } while (0)

const int WAIT_TIME = 1000;
const int PORT = 9000;

class NTree
{
public:
    using Node = std::map<int, std::set<int>>;
    Node node;
    NTree();
    void print();
    int dfs(int child, int curChild);
    int find(int child);
    int insert(int parent, int child);
    int erase(int parent, int child);
    ~NTree();
};

NTree::NTree()
{
    std::set<int> root;
    this->node.insert({-1, root});
}

NTree::~NTree()
{
}

void NTree::print()
{
    for (auto it = this->node.begin(); it != this->node.end(); it++) {
        std::cout << it->first << ": ";
        for (auto it1 = it->second.begin(); it1 != it->second.end(); it1++) {
            std::cout << *it1 << " ";
        }
        std::cout << "\n";
    }
}

int NTree::dfs(int child, int curChild) {
    for (auto i: this->node[curChild]) {
        // std::cout << i << "\n";
        if (i == child) {
            return 1;
        }
        return NTree::dfs(child, i);
    }
    return -1;
}

int NTree::find(int child)
{
    // auto curParent = this->node.find(parent);
    // if (curParent != this->node.cend()) {
    //     
    // }
    int ans = 0;
    for (auto curChild: this->node[-1]) {
        // std::cout << curChild << "\n";
        if (curChild == child) {
            return ans;
        } else if (dfs(child, curChild) != -1) {
            // std::cout << dfs(child, curChild) << "\n";
            return ans;
        }
        ans++;
    }
    return -1;
}

int NTree::insert(int parent, int child)
{
    auto curParent = this->node.find(parent);
    if (curParent != this->node.cend()) {
        auto curChild = curParent->second.find(child);
        if (curChild != curParent->second.cend()) {
            return 0;
        }
        curParent->second.insert(child);
        std::set<int> childVec;
        this->node.insert({child, childVec});
        return 1;
    }
    return 0;
}

int NTree::erase(int parent, int child) {
    auto curParent = this->node.find(parent);
    if (curParent != this->node.cend()) {
        auto curChild = curParent->second.find(child);
        if (curChild != curParent->second.cend()) {
            curParent->second.erase(child);
            auto p = this->node.find(child);
            this->node.erase(p);
            return 1;
        }
    }
    return 0;
}

namespace advancedZMQ
{
    nlohmann::json sendAndRecv(nlohmann::json &request, void *socket, int debug)
    {
        std::string strFromJson = request.dump();
        if (debug) {
            std::cout << strFromJson << std::endl;
        }
        void *arg = malloc(strlen(strFromJson.c_str()) + 1);
        memcpy(arg, strFromJson.c_str(), strlen(strFromJson.c_str()) + 1);
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        zmq_msg_init_size(&msg, strlen(strFromJson.c_str()) + 1);
        zmq_msg_init_data(&msg, arg, strlen(strFromJson.c_str()) + 1, NULL, NULL);
        zmq_msg_send(&msg, socket, 0);
        nlohmann::json reply;
        std::string strToJson;
        zmq_msg_t msg2;
        zmq_msg_init(&msg2);
        zmq_msg_recv(&msg2, socket, 0);
        strToJson.assign(static_cast<char *>(zmq_msg_data(&msg2)), zmq_msg_size(&msg2));
        if (!strToJson.empty()) {
            reply = nlohmann::json::parse(strToJson);
        } else {
            reply["ans"] = "error";
        }
        if (debug) {
            std::cout << reply << "\n";
        }
        zmq_msg_close(&msg);
        free(arg);
        return reply;
    }

    nlohmann::json Recv(void *socket)
    {
        nlohmann::json reply;
        std::string strToJson;
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        zmq_msg_recv(&msg, socket, 0);
        strToJson.assign(static_cast<char *>(zmq_msg_data(&msg)), zmq_msg_size(&msg));
        reply = nlohmann::json::parse(strToJson);
        zmq_msg_close(&msg);
        return reply;
    }

    void Send(nlohmann::json &request, void *socket)
    {
        std::string strFromJson = request.dump();
        void *arg = malloc(strlen(strFromJson.c_str()) + 1);
        memcpy(arg, strFromJson.c_str(), strlen(strFromJson.c_str()) + 1);
        zmq_msg_t msg2;
        zmq_msg_init(&msg2);
        zmq_msg_init_size(&msg2, strlen(strFromJson.c_str()) + 1);
        zmq_msg_init_data(&msg2, arg, strlen(strFromJson.c_str()) + 1, NULL, NULL);
        zmq_msg_send(&msg2, socket, 0);
    }
}


#endif