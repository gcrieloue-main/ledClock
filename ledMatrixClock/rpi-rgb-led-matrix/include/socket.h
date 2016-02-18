
#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <pthread.h>

class CommandListener
{
    public:
    CommandListener(std::vector<std::string>& vector);

    void doprocessing (int sock);

    void startSocket();
    
    void* commandListenerThread(void* vector);

    std::string pop_front(std::vector<std::string>& vec);
    
    private:
    std::vector<std::string>& _vector;
};

#endif
