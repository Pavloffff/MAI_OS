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

void ggame(int ans)
{

}

void server(std::string &sessionName)
{
    std::string gameSemName = sessionName + "game.semaphore";
    sem_t *gameSem = sem_open(gameSemName.c_str(), O_CREAT, accessPerm, 0);
    semSetvalue(gameSem, 0);
    int state = 0, firstIt = 0;
    while (1) {
        sem_getvalue(gameSem, &state);
        if (state % (session.cntOfPlayers + 1) == 0 && firstIt == 1) {
            int gameFd = shm_open((sessionName + "game.back").c_str(), O_RDWR | O_CREAT, accessPerm);
            struct stat statBuf;
            fstat(gameFd, &statBuf);
            int sz = statBuf.st_size;
            ftruncate(gameFd, sz);
            char *mapped = (char *) mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, gameFd, 0);
            nlohmann::json reply;
            std::string strToJson = mapped;
            reply = nlohmann::json::parse(strToJson);
            std::cout << reply << std::endl;
            sem_post(gameSem);
        } else if (state % (session.cntOfPlayers + 1) == 0 && firstIt == 0) {
            firstIt = 1;
            sem_post(gameSem);
        }
    }
}

void waitAlllPlayers(std::string &sessionName)
{
    session.curPlayerIndex = 0;
    std::string joinSemName = (sessionName + ".semaphore");
    
    sem_t *joinSem = sem_open(joinSemName.c_str(), O_CREAT, accessPerm, 0);
    int state = 0;
    
    sem_getvalue(joinSem, &state);
    std::cout << "server state: " << state << std::endl;
    std::string joinFdName;
    while (state < session.cntOfPlayers) {
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
    // sem_close(joinSem);
}

int main(int argc, char const *argv[])
{
    std::string strToJson = argv[1];
    nlohmann::json reply;
    reply = nlohmann::json::parse(strToJson);
    session.sessionName = reply["sessionName"];
    session.cntOfPlayers = reply["cntOfPlayers"];
    session.hiddenNum = reply["hiddenNum"];
    session.curPlayerIndex = 0;
    
    std::string apiSemName = session.sessionName + "api.semaphore";
    sem_unlink(apiSemName.c_str());
    sem_t *apiSem = sem_open(apiSemName.c_str(), O_CREAT, accessPerm, 0);
    semSetvalue(apiSem, session.cntOfPlayers);
    int f = 0;
    sem_getvalue(apiSem, &f);
    std::cout << "apiState " << f << std::endl;
    
    waitAlllPlayers(session.sessionName);

    std::cout << "hello\n";
    while (f > 0) {
        sem_wait(apiSem);
        sem_getvalue(apiSem, &f);
        std::cout << "apiState " << f << std::endl;
        sleep(1);
    }
    sem_getvalue(apiSem, &f);
    std::cout << "apiState " << f << std::endl;
    server(session.sessionName);
    return 0;
}
