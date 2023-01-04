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

using namespace gametools;

void client(std::string &playerName, std::string &sessionName, int state, int cnt)
{ 
    std::cout << playerName << " " << sessionName << " " << state << " " << cnt << std::endl;
    std::string apiSemName = sessionName + "api.semaphore";
    std::cout << apiSemName << "\n";
    sem_t *apiSem = sem_open(apiSemName.c_str(), O_CREAT, accessPerm, 0);
    
    semSetvalue(apiSem, cnt);
    int apiState = 0;
    sem_getvalue(apiSem, &apiState);
    std::cout << apiState << "\n";
    if (state > 1) {
        while (apiState != state) {
            sem_getvalue(apiSem, &apiState);
            // std::cout << apiState << " " << state << "\n";
        }
    }
    sem_getvalue(apiSem, &apiState);
    std::cout << apiState << "\n";
    sem_close(apiSem);
    std::string gameSemName = sessionName + "game.semaphore";
    sem_t *gameSem;
    std::cout << playerName << " " << sessionName << " " << state << " " << cnt << std::endl;
    if (state == 1) {
        sem_unlink(gameSemName.c_str());
        gameSem = sem_open(gameSemName.c_str(), O_CREAT, accessPerm, 0);
    } else {
        gameSem = sem_open(gameSemName.c_str(), O_CREAT, accessPerm, 0);
    }
    sem_getvalue(gameSem, &apiState);
    std::cout << apiState << std::endl;
    int firstIt = 1, cntOfBulls = 0, cntOfCows = 0, flag = 1;
    while (flag) {
        sem_getvalue(gameSem, &apiState);
        if (apiState % (cnt + 1) == state) {
            int gameFd = shm_open((sessionName + "game.back").c_str(), O_RDWR | O_CREAT, accessPerm);
            char *mapped;
            if (firstIt == 0) {
                struct stat statBuf;
                fstat(gameFd, &statBuf);
                int sz = statBuf.st_size;
                ftruncate(gameFd, sz);
                mapped = (char *) mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, gameFd, 0);
                nlohmann::json reply;
                std::string strToJson = mapped;
                reply = nlohmann::json::parse(strToJson);
                cntOfBulls = reply["bulls"];
                cntOfCows = reply["cows"];
                std::cout << "Bulls: " << cntOfBulls << std::endl;
                std::cout << "Cows: " << cntOfCows << std::endl;
                munmap(mapped, sz);
            } else {
                firstIt = 0;
            }
            int answer = 0;
            std::cout << "Input number between 1000 and 9999: ";
            std::cin >> answer;
            if (answer > 999 && answer < 10000) {
                std::cout << std::endl;
            } else {
                std::cout << "\nWrong number. Try again" << std::endl;
                continue;
            }
            nlohmann::json request;
            request["name"] = playerName;
            request["ans"] = answer;
            std::string strFromJson = request.dump();
            char *buffer = (char *) strFromJson.c_str();
            int sz = strlen(buffer) + 1;
            ftruncate(gameFd, sz);
            mapped = (char *) mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, gameFd, 0);
            memset(mapped, '\0', sz);
            sprintf(mapped, "%s", buffer);
            munmap(mapped, sz);
            close(gameFd);
            sem_post(gameSem);
        }
    }
}

void createSession(std::string &playerName, std::string &sessionName, int cntOfPlayers)
{
    int state2 = 0;
    nlohmann::json createRequest;
    createRequest["type"] = "create";
    createRequest["name"] = playerName;
    createRequest["bulls"] = 0;
    createRequest["cows"] = 0;
    createRequest["ans"] = 0;
    createRequest["sessionName"] = sessionName;
    createRequest["cntOfPlayers"] = cntOfPlayers;
    srand(time(NULL));
    int answer = 1000 + rand() % (9999 - 1000 + 1);
    createRequest["hiddenNum"] = answer;
    sem_t *mainSem = sem_open(mainSemName.c_str(), O_CREAT, accessPerm, 0);
    int state = 0;
    while (state != 1) {
        sem_getvalue(mainSem, &state);
    }
    int mainFd = shm_open(mainFileName.c_str(), O_RDWR | O_CREAT, accessPerm);
    std::string strFromJson = createRequest.dump();
    char *buffer = (char *) strFromJson.c_str();
    int sz = strlen(buffer) + 1;
    ftruncate(mainFd, sz);
    char *mapped = (char *) mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, mainFd, 0);
    memset(mapped, '\0', sz);
    sprintf(mapped, "%s", buffer);
    munmap(mapped, sz);
    close(mainFd);
    sem_wait(mainSem);
    sem_getvalue(mainSem, &state);
    while (state != 1) {
        sem_getvalue(mainSem, &state);
    }
    mainFd = shm_open(mainFileName.c_str(), O_RDWR | O_CREAT, accessPerm);
    struct stat statBuf;
    fstat(mainFd, &statBuf);
    sz = statBuf.st_size;
    ftruncate(mainFd, sz);
    mapped = (char *) mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, mainFd, 0);
    nlohmann::json reply;
    std::string strToJson = mapped;
    reply = nlohmann::json::parse(strToJson);
    if (reply["check"] == "ok") {
        std::cout << "Session " << sessionName << " created" << std::endl; 
        state2 = reply["state"];
    } else {
        std::cout << "Fail: name " << sessionName << " is already exists" << std::endl;
    }
    sem_wait(mainSem);
    // if (reply["check"] == "ok") {
    //     client(playerName, sessionName, state2, cntOfPlayers);
    // }
}

void joinSession(std::string &playerName, std::string &sessionName)
{
    nlohmann::json joinRequest;
    joinRequest["type"] = "join";
    joinRequest["name"] = playerName;
    joinRequest["bulls"] = 0;
    joinRequest["cows"] = 0;
    joinRequest["ans"] = 0;
    joinRequest["sessionName"] = sessionName;
    sem_t *mainSem = sem_open(mainSemName.c_str(), O_CREAT, accessPerm, 0);
    int state = 0;
    while (state != 1) {
        sem_getvalue(mainSem, &state);
    }
    int mainFd = shm_open(mainFileName.c_str(), O_RDWR | O_CREAT, accessPerm);
    std::string strFromJson = joinRequest.dump();
    char *buffer = (char *) strFromJson.c_str();
    int sz = strlen(buffer) + 1;
    ftruncate(mainFd, sz);
    char *mapped = (char *) mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, mainFd, 0);
    memset(mapped, '\0', sz);
    sprintf(mapped, "%s", buffer);
    munmap(mapped, sz);
    close(mainFd);
    sem_wait(mainSem);
    sem_getvalue(mainSem, &state);
    while (state != 1) {
        sem_getvalue(mainSem, &state);
    }
    mainFd = shm_open(mainFileName.c_str(), O_RDWR | O_CREAT, accessPerm);
    struct stat statBuf;
    fstat(mainFd, &statBuf);
    sz = statBuf.st_size;
    ftruncate(mainFd, sz);
    mapped = (char *) mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, mainFd, 0);
    nlohmann::json reply;
    std::string strToJson = mapped;
    reply = nlohmann::json::parse(strToJson);
    int state2 = 0, cnt = 0;
    if (reply["check"] == "ok") {
        std::cout << "Session " << sessionName << " joined" << std::endl; 
        state2 = reply["state"];
        cnt = reply["cnt"];
    } else {
        std::cout << "Fail: name " << sessionName << " is not exists" << std::endl;
    }
    sem_wait(mainSem);
    if (reply["check"] == "ok") {
        client(playerName, sessionName, state2, cnt);
    }
}

void findSession(std::string &playerName)
{
    nlohmann::json findRequest;

}

int main(int argc, char const *argv[])
{
    std::cout << "Input your name: ";
    std::string playerName;
    std::cin >> playerName;
    std::cout << std::endl;
    std::vector<int> v;
    Player player;
    std::string command;
    std::cout << "Write:\n command [arg1] ... [argn]\n";
    std::cout << "\ncreate [name] [cntOfPlayers] to create new game session by name and max count of players\n";
    std::cout << "\njoin [name] to join exists game session by name\n";
    std::cout << "\nfind to find game session\n\n";
    int flag = 1;
    while (flag) {
        std::cin >> command;
        if (command == "create") {
            std::string name;
            int cntOfPlayers;
            std::cin >> name >> cntOfPlayers;
            if (cntOfPlayers < 2) {
                std::cout << "Error: count of players must be greater then 1\n";
            }
            createSession(playerName, name, cntOfPlayers);
            joinSession(playerName, name);
            flag = 0;
        } else if (command == "join") {
            std::string name;
            std::cin >> name;
            joinSession(playerName, name);
            flag = 0;
        } else if (command == "find") {
            
            flag = 0;
        } else {
            std::cout << "Wrong command!\n"; 
            continue;
        }
    }
    return 0;
}
