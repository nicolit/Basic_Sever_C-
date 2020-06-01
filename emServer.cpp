/*
 * emServer.cpp
 * Created on: Jun 9, 2016
 * Author: nicole
 */
/* A simple server in the internet domain using TCP
 The port number is passed as an argument
 This version runs forever, forking off a separate
 process for each connection
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>  // inet_ntoa
#include <thread>

#include "Server.h"

/* max number of pending connections */
#define MAX_PEND_CONNECT 10
#define MAX_CLIENTNAME 10
#define MAXLEN 99999
#define TRUE 1
#define STDIN 0
std::vector<std::thread> threads;
bool exitServer = false;


void handleRequest( int newSockFD)
{
    Server::handleClient( newSockFD);
}


/**
 * @brief: In case of a system call error in the server side, don’t include
 * the client name. error message in the server log for system call error
 * should be in the following format:
 * HH:MM:SS \tERROR\t<system call function name.
 * For example: accept>\t<errno>.
 */
void serverSystemError( int errCode, const char *func)
{
    std::string funcSys( func);
    std::string err = "ERROR" +  '\t' +funcSys +'\t' + std::to_string(errCode);
    Server::logServer( err);
    exit(1);
}

/**
 * command line for running server:
 * ./emServer portNum
 */
int main( int argc, char *argv[])
{
    int masterSocket, newSockFD, portNum;
    struct sockaddr_in servAddress, client_info;
    char readbuf[MAXLEN];
    int optval = TRUE, activity;
    int max_sd;
    fd_set readfds; //set of socket descriptors

    if ( argc < 2) {
        fprintf( stdout, "Usage: emServer portNum");
        exit( 1);
    }

    Server& server = Server::getInstance();
    server.initServer();

    masterSocket = socket( AF_INET, SOCK_STREAM, 0);
    if ( masterSocket < 0)
    {
        serverSystemError( masterSocket, "socket");
    }

    /* set master socket to allow multiple connections */
    int res = setsockopt( masterSocket, SOL_SOCKET, SO_REUSEADDR,
                          (char *) &optval, (int) sizeof(optval));
     if( res < 0 )
     {
         serverSystemError( res, "setsockopt");
     }


    memset( (char *) &servAddress, 0, sizeof(struct sockaddr_in));

    portNum = atoi( argv[1]);
    servAddress.sin_family = AF_INET;
    servAddress.sin_addr.s_addr = INADDR_ANY;
    servAddress.sin_port = htons( portNum);

    res = bind( masterSocket,(struct sockaddr *) &servAddress,
                sizeof(servAddress));
    if ( res < 0)
    {
        serverSystemError( res, "bind");
    }

    res = listen( masterSocket, MAX_PEND_CONNECT);
    if ( res < 0 ){
        serverSystemError( res, "listen");
    }
    printf("Listener on port %d. Waiting for connections :)\n", portNum);

    while ( !exitServer)
    {
        //clear the socket set
        FD_ZERO(&readfds);
        FD_SET( STDIN, &readfds); /* add STDIN to readfds*/

        //add master socket to set
        FD_SET( masterSocket, &readfds); /* add sockfd to readfds*/
        max_sd = masterSocket;

        /*wait for activity on one of the sockets,timeout is NULL,
         * wait indefinitely*/
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

        if ((activity < 0) && (errno!=EINTR)) {
            serverSystemError( errno, "select");
        }

        if ( FD_ISSET( masterSocket, &readfds)) {

            socklen_t clientSize = sizeof(client_info);
            newSockFD = accept( masterSocket, (struct sockaddr *) &client_info,
                                &clientSize);

            if ( newSockFD < 0) {
                serverSystemError( newSockFD, "accept");
            }

            /* inform user of connected_ip and port*/
            /*
            char *connected_ip = inet_ntoa(client_info.sin_addr);
            int port = ntohs( client_info.sin_port);
            printf("New connection on socket fd is: %d, ip is: %s, port: %d\n",
                    newSockFD , connected_ip , port);
            */
            // spawn new thread to handle request
            threads.push_back( std::thread( handleRequest, newSockFD));
        }

        else if ( FD_ISSET( STDIN, &readfds)) {
            memset( readbuf, 0, MAXLEN);
            fgets( readbuf, MAXLEN, stdin);
            std::string command( readbuf);
            size_t hasExit = command.find("EXIT");
            if ( hasExit != std::string::npos) {
                exitServer = true;
            }
        }
    }

    /* if EXIT was types: call join() on each thread in turn*/
    if ( exitServer) {
        /* end of while */

        std::for_each( threads.begin(), threads.end(),
                       std::mem_fn( &std::thread::join));
        close( masterSocket);
        Server::exitServer();
        exit(0);
    }

    return 0; /* we never get here */
}
