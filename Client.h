/*
 * Client.h
 * Created on: Jun 9, 2016
 * Author: nicole
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h> // for stat
#include <fstream>   // std::fstream
#include <string>
#include <string.h>
#include <sys/types.h> // for size_t, off_t
#include <vector>
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream, std::stringbuf
#include <algorithm> // sort,distance, find_if, copy_if

#include "Logger.h"
#include "CommandParser.h"

/* LIMIT of chars of event command on client side - check it's valid command*/
#define MAX_TITLE 30
#define MAX_DATE 30
#define MAX_DESC 256
/* client log name: clientName_HHMMSS.log*/


class Client
{

public:
        int eventIdSent; /* the event id that this client sent RSVP*/
        int eventIdRequest; /* event id that this client requested RSVP list*/
        std::string clientName; /* the client name */

    /**
     * @brief: Constructor - default ctor.
     */
	Client( const std::string clientName);


    /**
     * destructor
     */
	virtual ~Client();

    /**
     * @brief: log response to client log file.
     */
    void logToClient( const std::string response);

    /**
     * @brief: Every error message in the client log(except system call error)
     * should be in the following format:
     * HH:MM:SS \tERROR\t<function name>\t<error description>.\n
     */
    void logClientError( std::string func, std::string desc);


    /**
     * @brief: validate that the client command is valid: meaning that the
     * command exist in the server API and the arguments number is valid
     * and the arguments chars length is valid.
     * @return: true is client request command from server is valid.
     */
    bool validateCommand( char* buffer);


    /**
     * @brief: get server response for client request - then identify it's
     * command type and pass it to the relevant log response method for final
     * logging in the client log file.
     */
    void log_response( const std::string response);


    /**
     * @brief: log in client log the server response about REGISTER.
     */
    void logRegister_response( const std::string response);

    /**
     * @brief: log in client log the server response about UNREGISTER.
     */
    void logUnregister_response( const std::string response);

    /**
     * @brief: log in client log the server response about CREATE.
     */
    void logCreate_response( const std::string response);

    /**
     * @brief: log in client log the server response about SEND_RSVP.
     */
    void logRSVP_response( const std::string response);

    /**
     * @brief: log in client log the server response about GET_RSVPS_LIST.
     */
    void logEventList_response( const std::string response);

    /**
     * @brief: log in client log the server response about GET_TOP_5.
     */
    void logGetTop5_response( const std::string response);


private:

	Logger *_logger; /* instance of logger for the client */
	bool _registered; /*flag to indicate if client already was registered*/
	int _commandType; /* the last request command type code */

};

#endif /* CLIENT_H_ */
