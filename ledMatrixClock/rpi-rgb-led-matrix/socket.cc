#include "socket.h"

CommandListener::CommandListener(std::vector<std::string>& vector):_vector(vector)
{
}

void CommandListener::doprocessing (int sock)
{
    while (1)
    {
        int n;
        char buffer[256];

        bzero(buffer,256);
        n = read(sock,buffer,255);

        if (n < 0)
        {
            perror("ERROR reading from socket");
            exit(1);
        }

        if (n == 0)
        {
            std::cout<<"Client disconnected"<<std::endl;
            break;
        }

        //n = write(sock,"I got your message",18);
        std::string str(buffer);
        _vector.push_back(str);

        if (n < 0)
        {
            perror("ERROR writing to socket");
            exit(1);
        }
    }
}

void CommandListener::startSocket()
{
    int sockfd, newsockfd, portno;
    socklen_t  clilen;
    struct sockaddr_in serv_addr, cli_addr;

    int optval=1; 

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 5001;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding");
        exit(1);
    }

    /* Now start listening for the clients, here
     * process will go in sleep mode and will wait
     * for the incoming connection
     */

    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            exit(1);
        }
        else
        {
            std::cout<<"Connection accepted"<<std::endl;
        }

        doprocessing(newsockfd);
        close(newsockfd);
    }
    close(sockfd);
}

