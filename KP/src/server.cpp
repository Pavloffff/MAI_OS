#include <iostream>
#include <unistd.h>
#include <thread>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "../include/kptools.h"
#include <vector>
#include <algorithm>
#include <map>
#include <nlohmann/json.hpp>

// написать мемори мап типа апи семафор сюда передавать json-ки чтобы сервер ждал когда зайдут все игроки (ой не апи семафор а семафор с именем сессии карл)

using namespace gametools;

Session session;

void server(std::string &gameSemName, std::string &gameFdName)
{
    sem_t *gameSem = sem_open(gameSemName.c_str(), O_CREAT, accessPerm, 0);
    int state = 0;
    semSetvalue(gameSem, 0);
    sem_getvalue(gameSem, &state);
    session.curPlayerIndex = state;
    // std::cout << state << std::endl;
    semSetvalue(gameSem, session.playerList.size() - 1);
    sem_getvalue(gameSem, &state);
    std::cout << state << " <-> " << session.playerList.size() << std::endl;
    while (1) {
        sem_getvalue(gameSem, &state);
        if (state != session.playerList.size() - 1) {
            int gameFd = shm_open(gameFdName.c_str(), O_RDWR | O_CREAT, accessPerm);
            struct stat statBuf;
            fstat(gameFd, &statBuf);
            int sz = statBuf.st_size;
            ftruncate(gameFd, sz);
            char *mapped = (char *) mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, gameFd, 0);
            std::string strToJson = mapped;
            nlohmann::json reply;
        }
    }
}

void waitAlllPlayers(std::string &sessionName)
{
    // std::cout << session << std::endl;
    session.curPlayerIndex = 0;
    std::string joinSemName = (sessionName + ".semaphore");
    sem_unlink(joinSemName.c_str());
    sem_t *joinSem = sem_open(joinSemName.c_str(), O_CREAT, accessPerm, 0);
    int state = 0;
    // while (state++ < 0) {
    //     sem_post(joinSem);
    // }
    // while (state-- > 1) {
    //     sem_wait(joinSem);
    // }
    semSetvalue(joinSem, 0);
    sem_getvalue(joinSem, &state);
    std::cout << "server state: " << state << std::endl;
    std::string joinFdName;
    while (state < session.cntOfPlayers - 1) {
        sem_getvalue(joinSem, &state);
        if (state > session.curPlayerIndex) {
            std::cout << state << " " << session.curPlayerIndex << std::endl;
            joinFdName = sessionName.c_str();
            int joinFd = shm_open(joinFdName.c_str(), O_RDWR | O_CREAT, accessPerm);
            struct stat statBuf;
            fstat(joinFd, &statBuf);
            int sz = statBuf.st_size;
            ftruncate(joinFd, sz);
            char *mapped = (char *) mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, joinFd, 0);
            std::string strToJson = mapped;
            nlohmann::json joinReply;
            munmap(mapped, sz);
            close(joinFd);
            joinReply = nlohmann::json::parse(strToJson);
            Player playerN;
            playerN.name = joinReply["name"];
            playerN.ans = joinReply["ans"];
            playerN.bulls = joinReply["bulls"];
            playerN.cows = joinReply["cows"];
            session.playerList.push_back(playerN);
            std::cout << session << std::endl;
            session.curPlayerIndex = state;
        }
    }
    sem_close(joinSem);
    server(joinSemName, joinFdName);
}

int main(int argc, char const *argv[])
{
    std::string strToJson = argv[1];
    nlohmann::json reply;
    reply = nlohmann::json::parse(strToJson);
    // std::cout << reply << std::endl;
    session.sessionName = reply["sessionName"];
    session.cntOfPlayers = reply["cntOfPlayers"];
    session.hiddenNum = reply["hiddenNum"];
    session.curPlayerIndex = 0;
    Player player0;
    player0.ans = reply["ans"];
    player0.bulls = reply["bulls"];
    player0.cows = reply["cows"];
    player0.name = reply["name"];
    session.playerList.push_back(player0);
    waitAlllPlayers(session.sessionName);
    return 0;
}
