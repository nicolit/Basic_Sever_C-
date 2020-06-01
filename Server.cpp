/*
 * Server.cpp
 * Created on: Jun 10, 2016
 * Author: nicole
 */

#include "Server.h"

/*************** Private Functions **************************************/

/**
 * @brief: private Constructor - default ctor.
 */
Server::Server(): _logger( nullptr), _nextEventId( 1)
{}


/**
 * @brief: checks if exists and event with given eventId.
 * @return: true if there is event with this id.
 */
bool Server::_isEventExist( int eventId)
{
    return (_eventsMap.find( eventId) != _eventsMap.end());
}


/**
 * @brief: checks if exists a client with given client name.
 * @return: true if there is client with this client name.
 */
bool Server::_isClientExist( const std::string client)
{
    std::string clientUp( client);
    /* transform first command word to upper case */
    std::transform(clientUp.begin(),clientUp.end(),clientUp.begin(),::toupper);
    return ( _clientsSet.find( clientUp) != _clientsSet.end());
}


/**
 * @brief: in case a client chose to unregister- the server removes
 * it's name from the guests list of each event that he might have sent
 * a RSVP request. events with empty guests list will remain.
 */
void Server::_removeClientFromEvents( const std::string client)
{
    std::map<int, Event*>::iterator it;

    /* iterate over all events and remove client guest that was unregistered*/
    for (it = _eventsMap.begin(); it != _eventsMap.end(); ++it)
    {
        (it->second)->removeGuest( client);
    }
}

/*************** Public Functions **************************************/

void Server::logServer( const std::string response)
{
    Server& server = Server::getInstance();
    server._logger->logCommand( response);
}

/**
 * @brief: Every error message in the server log (except system call error)
 * should be in the following format:
 * HH:MM:SS \tERROR\t<function name>\t<error description>.
 */
void Server::logServerError( std::string func, std::string desc)
{
    std::string errStr;
    errStr = "ERROR\t" + func + '\t' + desc;
    Server::logServer( errStr);
}


/**
 * destructor
 */
Server::~Server() {
}

/**
 * @brief: public getter - gets the singleton instance each call.
 * @return: CachManager instance.
 */
Server& Server::getInstance()
{
	/* first time it is created, and even since, stays same static variable*/
	static Server server;
	return server;
}


void Server::initServer()
{
	_logger = new Logger( LOGNAME);
}


/**
 * @brief: read client request from given socket into the buffer and
 * write server response to this socket.
 */
void Server::handleClient( int sock)
{
    Server& server = Server::getInstance();
   int numRead;
   int bytesToRead = 99999;
   std::string request = "", response = "";
   char* sendbuf;
   std::string client;
   char readbuf[bytesToRead];

   memset( readbuf, 0, bytesToRead);

   numRead = read( sock, readbuf, bytesToRead);
   if (numRead < 0) {
       Server::logServerError("read", "read buffer received empty");
   }

   /* the response of the server: */
   request = std::string( readbuf);

   std::size_t found = request.find("\n");
   if (found != std::string::npos) {
       client = request.substr( 0, found);
       request = request.substr( found+1);
       found = request.find("\n");
       if (found != std::string::npos) {
           request = request.substr( 0, found);
       }
       response = server.parseCommand( client, request);
   }

   /* write a response to client's socket */
   sendbuf = (char *)malloc(sizeof(char)*(response.length()+1));
   memset( sendbuf, 0, response.length()+1);
   const char* resp = response.c_str();
   strcpy( sendbuf, resp);

   write( sock, sendbuf, strlen( sendbuf));
   free( sendbuf);

   close( sock);
}


/**
 * @brief: the server receives the user request that begins with the
 * command and it's arguments, parses request into tokens, identifies
 * the command type and passes next to the relevant command function
 * that will return the response string.
 * @return: response string.
 */
std::string Server::parseCommand(const std::string client, std::string request)
{
    std::string token;
    std::string response;
    std::vector<std::string> tokens;
    CommandParser::tokenize( request, " ", tokens);
    std::string command = tokens[0];
    size_t hasNewLine = 0;
    /* transform first command word to upper case */
    std::transform( command.begin(), command.end(),command.begin(),::toupper);
    int eventId;
    int commType = CommandParser::getCommandType( command);

    switch( commType)
    {
        case REGISTER:
           response = registerClient( client);
           break;

        case CREATE:
            if ( tokens.size() > 4) {
                CommandParser::joinTokens( tokens, " ", 3);
            }
           response = createEvent( client, tokens[1], tokens[2], tokens[3]);
           break;

        case UNREGISTER:
            response = unregisterClient( client);
            break;

        case SEND_RSVP:
            hasNewLine = tokens[1].find("\n");
            if (hasNewLine != std::string::npos){
                tokens[1] = tokens[1].substr( 0, hasNewLine);
            }
            if ( CommandParser::isStrNumber( tokens[1])) {
                eventId = stoi( tokens[1]);
                response = sendRSVP( client, eventId);
            } else {
                response = CommandParser::logCommandError( client,
                           INVALID_ARG_COMMAND, command, tokens[1]);
            }
            break;

        case GET_RSVPS_LIST:
            hasNewLine = tokens[1].find("\n");
            if (hasNewLine != std::string::npos){
                tokens[1] = tokens[1].substr( 0, hasNewLine);
            }
            if ( CommandParser::isStrNumber( tokens[1])) {
                eventId = stoi( tokens[1]);
                /* get list of guests as string */
                response = getRSVP_List( client, eventId);
            } else {
                response = CommandParser::logCommandError( client,
                           INVALID_ARG_COMMAND, command, tokens[1]);
            }
            break;

        case GET_TOP_5:
            response = getTop5Events( client);
            break;

        case ILLEGAL:
            response = ILLEGAL_COMMAND;
            break;
    }

    return response;
}


/**
 * @brief: register new client.
 * @return: result of registration.
 */
std::string Server::registerClient( const std::string client)
{
    std::string response;
    std::string serverLog;
    std::string clientUp( client);
    /* transform first command word to upper case */
    std::transform(clientUp.begin(),clientUp.end(),clientUp.begin(),::toupper);

    if ( !_isClientExist( clientUp))
    {
        _clientsSet.insert( clientUp);
        serverLog = client + "\t" + REGISTER_SUCCESS;
        response = "SUCCESS";
    } else {
        serverLog = "ERROR: " + client + "\t" + ALREADY_REGISTERED;
        response = "ERROR: the client " + client + CLIENT_REGISTERED;
    }

    Server::logServer( serverLog);
    return response;
}


/**
 * @brief: unregister existing client.
 * @return: result of registration.
 */
std::string Server::unregisterClient( const std::string client)
{
    std::string serverLog;
    std::string response;
    std::string clientUp( client);
    /* transform first command word to upper case */
    std::transform(clientUp.begin(),clientUp.end(),clientUp.begin(),::toupper);

    if ( _isClientExist( clientUp))
    {
        _clientsSet.erase(clientUp);
        _removeClientFromEvents( clientUp);

        serverLog = client + "\t" + " was unregistered successfully.";
        Server::logServer( serverLog);
        response = "SUCCESS";
        return response;
    } else {
        response = NOT_REGISTERED;
    }

    return response;
}


/**
 * @brief: create new event
 * @return: new event id.
 */
std::string Server::createEvent( const std::string client,
                                 const std::string title,
                                 const std::string date,
                                 const std::string description)
{
    Event* event;
    int eventId = _nextEventId;
    std::string serverLog;
    std::string response;
    std::string eventIdStr = std::to_string(eventId);

    if ( !_isClientExist( client)) {
        response = NOT_REGISTERED;
        return response;
    }

    if ( _isEventExist( _nextEventId)) {
        _nextEventId++;
        eventId = _nextEventId;
    }

    try {
        event = new Event( client, eventId, title, date, description);
        _eventList.push_back( event);
        _eventsMap[eventId] = event;
        response ="Event id " + eventIdStr +" was created successfully.";

    } catch ( std::bad_alloc& e) {
        _nextEventId--;
        response = ERROR_EVENT_ALLOC;
        return response;
    }

    serverLog = client + "\t" + " event id " + eventIdStr;
    serverLog += " was assigned to the event with title " + title + ".";
    _nextEventId++;
    Server::logServer( serverLog);
    return response;
}


/**
 * @brief: send RSVP to event
 * @return: result of trying to register to event.
 */
std::string Server::sendRSVP( const std::string client, int eventId)
{
    std::string response;
    std::string serverLog;
    std::string eventID = std::to_string( eventId);
    std::string errorDesc;

    if ( !_isClientExist( client)) {
         response = NOT_REGISTERED;
         return response;
     }

    if ( _isEventExist( eventId)) {
        response = _eventsMap[eventId]->registerClient( client);
        serverLog = client + "\t" + " is RSVP to event with id " + eventID+".";
    } else {
        /* if client tries to register to event that does not exist - error */
        response = "ERROR: " + EVENT_NOT_EXIST;
        serverLog = "ERROR: " + EVENT_NOT_EXIST;
    }

    Server::logServer( serverLog);
    return response;
}


/**
 * @brief: EXIT command from client that terminates the server.
 * exit command is legal even if no client was registered to the server yet.
 */
void Server::exitServer()
{
    Server& server = Server::getInstance();
    std::string serverLog;
    serverLog = "EXIT command is typed: server is shutdown";
    Server::logServer( serverLog);
    /* close server log and delete instance */
    sleep(2); // time out 2 seconds
    delete server._logger;

    std::vector<Event*>::iterator it;

    server._clientsSet.clear();
    server._eventsMap.clear();

    it = server._eventList.begin();
    while ( it != server._eventList.end()) {
        if (*it != nullptr){
            delete *it;
        }
        server._eventList.erase(it);
    }
    server._eventList.clear();
}


/**
 * @brief: tries to get the guests list associated with the given eventId.
 * @return: guests list response or and error response if invalid id.
 */
std::string Server::getRSVP_List( const std::string client, int eventId)
{
    std::list<std::string> list;
    std::string serverLog, listStr = "";

    if ( !_isClientExist( client)) {
        listStr = NOT_REGISTERED;
         return listStr;
     }


    /* add each guest in the event guests list to listStr*/
    if ( _isEventExist( eventId)) {
        list = _eventsMap[eventId]->getGuestList();
        if ( list.size() > 0) {
            listStr = CommandParser::convertListToString( list, ",");
        }
        serverLog = client + "\t" + " requests the RSVP'S list for event with id ";
        serverLog += std::to_string( eventId) + ".";
        Server::logServer( serverLog);

    } else {
        listStr = "ERROR: " + EVENT_NOT_EXIST;
        Server::logServerError("GET_RSVPS_LIST", EVENT_NOT_EXIST);
    }

    return listStr;
}


/**
 * @brief: gets the top five most recent new added events. in case there
 * are less then five it returns those events.
 * @return: events list as string.
 */
std::string Server::getTop5Events( const std::string client)
{
    std::vector<Event*>::iterator it;
    size_t firstIdx = 0;
    size_t listSize = _eventList.size();
    std::string serverLog, listStr = "";
    serverLog = client + "\t" + " requests the top 5 newest events.";
    /* ech event should be printed in the following format:
     * <eventId>\t<eventTitle>\t<eventDate>\t<eventDescription>.\n*/

    if ( !_isClientExist( client)) {
        listStr = NOT_REGISTERED;
         return listStr;
     }

    if (listSize > 5) {
        firstIdx = listSize - 5;
    }

    for ( it = _eventList.begin() + firstIdx; it != _eventList.end(); ++it)
    {
        listStr += (*it)->toString();
    }
    listStr += ".";

    Server::logServer( serverLog);
    return listStr;
}
