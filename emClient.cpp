/*
 * emClient.cpp
 * Created on: Jun 9, 2016
 * Author: nicole
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>
/*contains definitions of a number of data types used in system calls.
 * These types are used in the next two include files.*/
#include <sys/types.h>
/*includes a number of definitions of structures needed for sockets.*/
#include <sys/socket.h>
#include <netinet/in.h> /* defines sockaddr_in struct */
#include <netdb.h> /*defines the structure hostent*/
#include <arpa/inet.h>
#include <vector>
#include <iostream>  // std::cout
#include <errno.h>
extern int h_errno;

#include "Client.h"
#define MAX_CLIENTNAME 10
#define TRUE 1
#define MAXLEN 99999
// N.B.O – network bytes order

/**
 * @brief: In case of a system call error in the server side, don’t include
 * the client name. error message in the server log for system call error
 * should be in the following format:
 * HH:MM:SS \tERROR\t<system call function name.
 * For example: accept>\t<errno>.
 */
void clientSystemCallError( Client* client, const char *func, int errCode)
{
    std::string errStr;
    std::string funcSys( func);
    errStr = "ERROR" + '\t' + funcSys+ '\t' + std::to_string(errCode);
    client->logToClient( errStr);
    exit(1);
}


/* client main
 * Start the server first.
 * To run the client you need to pass in two arguments, the name of the host
 * on which the server is running and the port number on which the server
 * is listening for connections.
 * The command line to connect to the server described above:
 * ./emClient clientName serverAddress serverPort :
 * ./emClient nicole 127.0.0.1 1024 */
int main( int argc, char *argv[])
{
    /*sockfd is file descriptors, i.e. array subscripts into
     * the file descriptor table . These two variables store the values
     * returned by the socket system call and the accept system call.*/
    int sockFD;
    /*n is the return value for the read() and write() calls;
     * i.e. it contains the number of characters read or written.*/
    int n;
    /*portno stores the port number on which the server accepts connections.*/
    int portNum;

    /*variable serv_addr will contain the address of the server*/
    struct sockaddr_in serv_addr;
    /*The variable server is a pointer to a structure of type hostent.*/
    struct hostent *server;
    /* server reads characters from the socket connection into this buffer*/
    char userCommandBuff[MAXLEN];
    char requestBuff[MAXLEN];
    char responseBuff[MAXLEN];
    std::string clientname;
    std::string request;
    Client* client;
    bool isValidCommand;

    if ( argc < 3) {
        fprintf( stdout,"Usage: emClient clientName serverAddress serverPort");
        exit(1);
    }

    memset( &serv_addr, 0, sizeof(struct sockaddr_in));
    clientname = std::string( argv[1]);
    client = new Client( clientname);

    /*gethostbyname returns a struct hostent named server */
    server = gethostbyname( argv[2]);
    if ( !server) {
        clientSystemCallError( client, "gethostbyname", h_errno);
    }

    portNum = atoi( argv[3]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons( (u_short) portNum);

    memcpy((char *)&serv_addr.sin_addr,(char *)server->h_addr,server->h_length);


    while (TRUE)
    {
        memset( userCommandBuff, 0, MAXLEN); // init buffer with 0
        memset( requestBuff, 0, MAXLEN);

        fgets( userCommandBuff, MAXLEN-1, stdin);

        isValidCommand = client->validateCommand( userCommandBuff);
        if (isValidCommand)
        {
            sockFD = socket( AF_INET, SOCK_STREAM, 0);
            if ( sockFD < 0)
            {
                clientSystemCallError( client, "socket", sockFD);
            }
            /* called by the client to establish a connection to the server*/
            int res = connect( sockFD,(struct sockaddr *) &serv_addr,
                               sizeof(serv_addr));
            if (res < 0)
            {
                clientSystemCallError( client, "connect", res);
            } else {
                strcpy( requestBuff, clientname.c_str());
                strncat(requestBuff, "\n", strlen("\n"));
                strncat(requestBuff, userCommandBuff, strlen(userCommandBuff));

                /* write client request to the socket */

                n = write( sockFD, requestBuff, strlen(requestBuff));
                if ( n < 0) {
                    clientSystemCallError( client, "write", n);
                }


                memset( responseBuff, 0, MAXLEN); // init buffer with 0 for reading
                /*reads the response from the server */

                n = read( sockFD, responseBuff, MAXLEN-1);
                if ( n < 0) {
                    clientSystemCallError( client, "connect", n);
                } else {
                    /* log server response in client log */
                    client->log_response( std::string(responseBuff));
                }
            }

            close( sockFD);
        }

    }

    delete client;

    return 0;
}
