
#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <queue>
#include <pthread.h>

class CommandListener
{
    public:
    CommandListener(std::queue<std::string>& queue);

    void doprocessing (int sock);

    void startSocket();
    
    void* commandListenerThread(void* queue);

    private:
    std::queue<std::string>& _queue;
};

#endif
