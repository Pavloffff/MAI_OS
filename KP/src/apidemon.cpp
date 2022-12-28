#include <iostream>
#include <unistd.h>
#include <thread>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "../include/kptools.h"
#include <vector>
#include <algorithm>
#include <map>
#include <nlohmann/json.hpp>

using namespace gametools;

std::map<std::string, Session> sessions;

int main(int argc, char const *argv[])
{
    sem_unlink(mainSemName.c_str());
    sem_t *mainSem = sem_open(mainSemName.c_str(), O_CREAT, accessPerm, 0);
    int state = 0;
    while (state++ < 1) {
        sem_post(mainSem);
    }
    while (state-- > 2) {
        sem_wait(mainSem);
    }
    sem_getvalue(mainSem, &state);
    while (1) {
        sem_getvalue(mainSem, &state);
        if (state == 0) {
            int mainFd = shm_open(mainFileName.c_str(), O_RDWR | O_CREAT, accessPerm);
            struct stat statBuf;
            fstat(mainFd, &statBuf);
            int sz = statBuf.st_size;
            ftruncate(mainFd, sz);
            char *mapped = (char *) mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, mainFd, 0);
            nlohmann::json createReply;
            std::string strToJson = mapped;
            munmap(mapped, sz);
            close(mainFd);
            createReply = nlohmann::json::parse(strToJson);
            nlohmann::json request;
            if (createReply.contains("type")) {
                // std::cout << state << std::endl;
                // std::cout << "reply: " << createReply <<std::endl;
                Player player;
                player.ans = createReply["ans"];
                player.bulls = createReply["bulls"];
                player.cows = createReply["cows"];
                player.name = createReply["name"];
                if (createReply["type"] == "create") {
                    if (sessions.find(createReply["sessionName"]) == sessions.cend()) {
                        Session session;
                        session.sessionName = createReply["sessionName"];
                        session.cntOfPlayers = createReply["cntOfPlayers"];
                        srand(time(NULL));
                        int answer = 1000 + rand() % (9999 - 1000 + 1);
                        session.hiddenNum = answer;
                        session.playerList.push_back(player);
                        sessions.insert({session.sessionName, session});
                        for (auto i: sessions) {
                            std::cout << i.second << std::endl;
                        }
                        request["check"] = "ok";
                        request["state"] = 0;
                        pid_t serverPid = fork();
                        if (serverPid == 0) {
                            sem_close(mainSem);
                            execl("./server", "./server", strToJson.c_str(), NULL);
                            return 0;
                        }
                    } else {
                        request["check"] = "error";
                    }
                } else if (createReply["type"] == "join") {
                    if (sessions.find(createReply["sessionName"]) != sessions.cend()) {
                        
                    }
                }
            } else {
                // std::cout << state << std::endl;
                sem_post(mainSem);
                continue;
            }
            // std::cout << request << std::endl;
            std::string strFromJson = request.dump();
            char *buffer = (char *) strFromJson.c_str();
            sz = strlen(buffer) + 1;
            mainFd = shm_open(mainFileName.c_str(), O_RDWR | O_CREAT, accessPerm);
            ftruncate(mainFd, sz);
            mapped = (char *) mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, mainFd, 0);
            memset(mapped, '\0', sz);
            sprintf(mapped, "%s", buffer);
            munmap(mapped, sz);
            close(mainFd);
            sem_post(mainSem);
        }
    }
    return 0;
}
