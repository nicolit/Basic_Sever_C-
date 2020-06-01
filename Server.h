/*
 * Server.h
 * Created on: Jun 10, 2016
 * Author: nicole
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h> // for stat
#include <fstream>   // std::fstream
#include <string>
#include <string.h>
#include <sys/types.h> // for size_t, off_t
#include <vector>
#include <list>
#include <map>
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream, std::stringbuf
#include <algorithm> // sort,distance, find_if, copy_if, transform
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>

#include "Logger.h"
#include "Event.h"
#include "CommandParser.h"

/* server log name - includes all the commands it received */
#define LOGNAME "emServer.log"


/**
 * A singleton Server in charge over management of incoming client commands.
 * (similar to the my CachManager implementation in ex4).
 */
class Server
{
public:

    static void logServer( const std::string response);

    /**
     * @brief: Every error message in the server log (except system call error)
     * should be in the following format:
     * HH:MM:SS \tERROR\t<function name>\t<error description>.\n
     */
    static void logServerError( std::string func, std::string desc);


	/**
	 * destructor
	 */
	virtual ~Server();

	/**
	 * @brief: copy constructor - disabled in order to avoid accidental
	 * copies creation of the singleton instance.
	 */
	Server( Server const &other) = delete;

	/**
	 * @brief: copy assignment copy constructor - disabled in order to avoid
	 * accidental copies creation of the singleton instance.
	 */
    void operator=( Server const &other) = delete;

    /**
     * @brief: move assignment - disabled
     */
    Server& operator=( Server& other) = delete;

	/**
	 * @brief: public getter - gets the singleton instance each call.
	 * @return: Cache manager instance.
	 */
	static Server& getInstance();


	/**
	 * @brief: Initialize the cache manager.
	 */
	void initServer();


	/**
	 * @brief: read client request from given socket into the buffer and
	 * write server response to this socket.
	 */
	static void handleClient( int sock);


	/**
	 * @brief: the server receives the user request that begins with the
	 * command and it's arguments, parses request into tokens, identifies
	 * the command type and passes next to the relevant command function
	 * that will return the response string.
	 * @return: response string.
	 */
	std::string parseCommand( const std::string client, std::string request);

	/* client requests from server */

    /**
     * @brief: register new client.
     * @return: result of registration.
     */
	std::string registerClient( const std::string client);


    /**
     * @brief: unregister existing client.
     * @return: result of registration.
     */
	std::string unregisterClient( const std::string client);

	/**
	 * @brief: create new event
	 * @return: new event id.
	 */
    std::string createEvent( const std::string client, const std::string title,
	                         const std::string date,const std::string description);

    /**
     * @brief: send RSVP to event
     * @return: result of trying to register to event.
     */
    std::string sendRSVP( const std::string client, int eventId);


	/**
	 * @brief: EXIT command from client that terminates the server.
	 * exit command is legal even if no client was registered to the server yet.
	 */
	static void exitServer();


    /**
     * @brief: tries to get the guests list associated with the given eventId.
     * @return: guests list response or and error response if invalid id.
     */
	std::string getRSVP_List( const std::string client, int eventId);

    /**
     * @brief: gets the top five most recent new added events. in case there
     * are less then five it returns those events.
     * @return: events list as string.
     */
	std::string getTop5Events( const std::string client);

private:

	Logger *_logger;
	int _nextEventId;
	std::vector<Event*> _eventList;
	std::set<std::string /*client name*/> _clientsSet;
	std::map<int /*eventId*/, Event*> _eventsMap;

	/**
	 * @brief: private Constructor - default ctor.
	 */
	Server();

    /**
     * @brief: checks if exists an event with given eventId.
     * @return: true if there is event with this id.
     */
    bool _isEventExist( int eventId);

    /**
     * @brief: checks if exists a client with given client name.
     * @return: true if there is client with this client name.
     */
    bool _isClientExist( const std::string client);

    /**
     * @brief: in case a client chose to unregister- the server removes
     * it's name from the guests list of each event that he might have sent
     * a RSVP request. events with empty guests list will remain.
     */
    void _removeClientFromEvents( const std::string client);
};

#endif /* SERVER_H_ */
