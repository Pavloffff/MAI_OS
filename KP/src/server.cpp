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

// написать мемори мап типа апи семафор сюда передавать json-ки чтобы сервер ждал когда зайдут все игроки (ой не апи семафор а семафор с именем сессии карл)

using namespace gametools;

Session session;

int main(int argc, char const *argv[])
{
    std::cout << argv[1] << " hello " << std::endl;
    return 0;
}
