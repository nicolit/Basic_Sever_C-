/*
 * Client.cpp
 * Created on: Jun 9, 2016
 * Author: nicole
 */

#include "Client.h"

/*************** Private Functions **************************************/


/*************** Public Functions **************************************/

/**
 * @brief: Constructor - default ctor.
 */
Client::Client( const std::string clientName): eventIdSent(0),
        eventIdRequest(0), clientName( clientName), _registered(false),
        _commandType(0)
{
    time_t current_time;
    struct tm * time_info;
    char timeString[9];  // space for "HH:MM:SS\0"
    time(&current_time);
    time_info = localtime( &current_time);
    strftime( timeString, sizeof(timeString), "%H%M%S", time_info);

    std::string logName = clientName + "_" + std::string(timeString) + ".log";
    _logger = new Logger( logName);
}


/**
 * destructor
 */
Client::~Client()
{
    /* close server log and delete instance */
    delete _logger;
}


/**
 * @brief: log response to client log file.
 */
void Client::logToClient( const std::string response)
{
    _logger->logCommand( response);
}


/**
 * @brief: Every error message in the client log(except system call error)
 * should be in the following format:
 * HH:MM:SS \tERROR\t<function name>\t<error description>.\n
 */
void Client::logClientError( std::string func, std::string desc)
{
    std::string errStr;
    errStr = "ERROR\t" + func + '\t' + desc;
    logToClient( errStr);
}


/**
 * @brief: validate that the client command is valid: meaning that the
 * command exist in the server API and the arguments number is valid
 * and the arguments chars length is valid.
 * @return: true is client request command from server is valid.
 */
bool Client::validateCommand( char* buffer)
{
    std::vector<std::string> tokens;
    std::string commLine( buffer);
    std::string errMsg;
    CommandParser::tokenize( commLine, " ", tokens);
    std::string command = tokens[0];
    size_t hasNewLine = 0;
    size_t found = command.find("\n");
    if (found != std::string::npos) {
        command = command.substr( 0, found);
    }
    /* transform first command word to upper case */
    std::transform( command.begin(), command.end(),command.begin(), ::toupper);
    _commandType = CommandParser::getCommandType( command);
    bool valid = true;

    switch( _commandType)
    {
        case REGISTER:
            /* REGISTER has no arguments */
            if (tokens.size() > 1) {
                errMsg = CommandParser::logCommandError( clientName, MISS_ARGS,
                                                         command);
                valid = false;
            }
            if ( _registered) { // if it's true then client already registered
                errMsg = CommandParser::logCommandError( clientName,
                                                         ALREADY_REGISTER);
                valid = false;
            }
            break;

        case CREATE:
            if ( !_registered) { // if false then client didn't register yet
                errMsg =CommandParser::logCommandError(clientName,NO_REGISTER);
                valid = false;
            }

            if (tokens.size() < 4) {
                errMsg = CommandParser::logCommandError( clientName, MISS_ARGS,
                                                         command);
                valid = false;

            } else {
                if (tokens[1].length() > MAX_TITLE) {
                    errMsg = CommandParser::logCommandError( clientName,
                             INVALID_ARG_COMMAND, command, tokens[1]);
                    valid = false;
                }

                if ( tokens[2].length() > MAX_DATE) {
                    errMsg = CommandParser::logCommandError( clientName,
                             INVALID_ARG_COMMAND, command, tokens[2]);
                    valid = false;
                }

                if ( tokens.size() > 4) {
                    CommandParser::joinTokens( tokens, " ", 3);
                }
                if ( tokens[3].length() > MAX_DESC) {
                    errMsg = CommandParser::logCommandError( clientName,
                             INVALID_ARG_COMMAND, command, tokens[3]);
                    valid = false;
                }
            }
            break;

        case UNREGISTER:
            if ( !_registered) { // if false then client didn't register yet
                errMsg =CommandParser::logCommandError(clientName,NO_REGISTER);
                valid = false;
            }

            if (tokens.size() > 1) {
                errMsg = CommandParser::logCommandError( clientName, MISS_ARGS,
                                                         command);
                valid = false;
            }
            break;

        case SEND_RSVP:
            if ( !_registered) { // if false then client didn't register yet
                errMsg =CommandParser::logCommandError(clientName,NO_REGISTER);
                valid = false;
            }
            /* should have an evendId numeric string */
            if (tokens.size() < 2) {
                errMsg = CommandParser::logCommandError( clientName, MISS_ARGS,
                                                         command);
                valid = false;

            } else {
                hasNewLine = tokens[1].find("\n");
                if (hasNewLine != std::string::npos){
                    tokens[1] = tokens[1].substr( 0, hasNewLine);
                }

                if ( CommandParser::isStrNumber( tokens[1])) {
                    eventIdSent = stoi( tokens[1]);
                } else {
                    errMsg = CommandParser::logCommandError( clientName,
                            INVALID_ARG_COMMAND, command, tokens[1]);
                    valid = false;
                }
            }
            break;

        case GET_RSVPS_LIST:
            if ( !_registered) { // if false then client didn't register yet
                errMsg =CommandParser::logCommandError(clientName,NO_REGISTER);
                valid = false;
            }
            /* should have an evendId numeric string */
            if (tokens.size() < 2) {
                errMsg = CommandParser::logCommandError( clientName, MISS_ARGS,
                                                         command);
                valid = false;

            } else {
                hasNewLine = tokens[1].find("\n");
                if (hasNewLine != std::string::npos){
                    tokens[1] = tokens[1].substr( 0, hasNewLine);
                }
                if ( CommandParser::isStrNumber( tokens[1])) {
                    eventIdRequest = stoi( tokens[1]);
                } else {
                    errMsg = CommandParser::logCommandError( clientName,
                             INVALID_ARG_COMMAND, command, tokens[1]);
                    valid = false;
                }
            }
            break;

        case GET_TOP_5:
            if ( !_registered) { // if false then client didn't register yet
                errMsg =CommandParser::logCommandError(clientName,NO_REGISTER);
                valid = false;
            }
            if (tokens.size() > 1) {
                errMsg = CommandParser::logCommandError( clientName, MISS_ARGS,
                                                         command);
                valid = false;
            }
            break;

        case ILLEGAL:
            errMsg = CommandParser::logCommandError( clientName,
                                                     COMMAND_NOTEXIST);
            valid = false;
            break;

        default:
            errMsg = CommandParser::logCommandError( clientName,
                                                     COMMAND_NOTEXIST);
            valid = false;
            break;
    }

    logToClient( errMsg);

    return valid;
}


/**
 * @brief: get server response for client request - then identify it's
 * command type and pass it to the relevant log response method for final
 * logging in the client log file.
 */
void Client::log_response( const std::string response)
{
    /* log server response according to the last command type user requested*/
    switch( _commandType)
    {
        case REGISTER:
            logRegister_response( response);
            break;

        case CREATE:
            logCreate_response( response);
            break;

        case UNREGISTER:
            logUnregister_response( response);
            break;

        case SEND_RSVP:
            logRSVP_response( response);
            break;

        case GET_RSVPS_LIST:
            logEventList_response( response);
            break;

        case GET_TOP_5:
            logGetTop5_response( response);
            break;
    }
}


/**
 * @brief: log in client log the server response about REGISTER.
 */
void Client::logRegister_response( const std::string response)
{
    std::string logClient;

    if ( response.find( "ERROR: ") != std::string::npos) {
        logClient = "ERROR: the client " + clientName + CLIENT_REGISTERED;
        _registered = false;
        delete _logger; /* first close and delete logger instance */
        /*Tal said in the forum if client tries to register with taken
         * client name, it's program should exit */
        exit(0);

    } else if ( response.find( "SUCCESS") != std::string::npos) {
        _registered = true;
        logClient = "Client " + clientName + REGISTER_SUCCESS;
    }

    logToClient( logClient);
}


/**
 * @brief: log in client log the server response about UNREGISTER.
 */
void Client::logUnregister_response( const std::string response)
{
    std::string logClient;

    if (response.find( "ERROR: ") != std::string::npos) {
        logClient = "ERROR: failed to unregister client " + clientName + ".";
        logToClient( logClient);

    } else if (response.find( "SUCCESS") != std::string::npos) {
        logClient = "Client " + clientName + " was unregistered successfully.";
        logToClient( logClient);
        _registered = false;
        delete _logger;
        exit(0);
    }
}


/**
 * @brief: log in client log the server response about CREATE.
 */
void Client::logCreate_response( const std::string response)
{
    std::string eventId = response;
    std::string logClient, errMsg;
    std::size_t foundError;

    foundError = response.find( "ERROR: ");

    if (foundError != std::string::npos) {
        errMsg = response.substr( foundError);
        logClient = "ERROR: failed to create the event:" + errMsg;

    } else if ( response.find("REGISTER") != std::string::npos) {
        logClient = NOT_REGISTERED;

    } else {
        logClient = response;
    }

    logToClient( logClient);
}


/**
 * @brief: log in client log the server response about SEND_RSVP.
 */
void Client::logRSVP_response( const std::string response)
{
    std::string eventID = std::to_string( eventIdSent);
    std::string eventId = response;
    std::string logClient, errMsg;
    std::size_t foundError;

    foundError = response.find( "ERROR: ");

    if (foundError != std::string::npos) {
        errMsg = response.substr( foundError);
        logClient="ERROR: failed to send RSVP to event id "+eventID+": "+errMsg;
    } else if ( response.find("REGISTER") != std::string::npos) {
        logClient = NOT_REGISTERED;

    } else if (response.find("SUCCESS") != std::string::npos) {
        logClient = "RSVP to event id "+eventID+" was received successfully.";

    } else {
        logClient = "RSVP to event id " + eventID + " was already sent.";
    }

    logToClient( logClient);
}


/**
 * @brief: log in client log the server response about GET_RSVPS_LIST.
 */
void Client::logEventList_response( const std::string response)
{
    std::string eventID = std::to_string( eventIdSent);
    std::string eventId = response;
    std::string logClient, errMsg;
    std::size_t foundError;

    foundError = response.find( "ERROR: ");

    if (foundError != std::string::npos) {
        errMsg = response.substr( foundError);
        logClientError("GET_RSVPS_LIST", errMsg);

    } else if (response.find("REGISTER") != std::string::npos) {
        logClient = NOT_REGISTERED;
        logToClient( logClient);

    } else {
        logClient="The RSVP's list for event id "+eventID+" is: "+response+".";
        logToClient( logClient);
    }
}


/**
 * @brief: log in client log the server response about GET_TOP_5.
 */
void Client::logGetTop5_response( const std::string response)
{
    std::string eventID = std::to_string( eventIdSent);
    std::string eventId = response;
    std::string logClient, errMsg;
    std::size_t foundError;
    logClient = "Top 5 newest events are:\n<list of events>.\n";

    foundError = response.find( "ERROR: ");

    if (foundError != std::string::npos) {
        errMsg = response.substr( foundError);
        logClient = "ERROR: failed to receive top 5 newest events: " + errMsg;

    } else if (response.find("REGISTER") != std::string::npos) {
        logClient = NOT_REGISTERED;

    } else {
        logClient = "Top 5 newest events are:\n" + response;
    }

    logToClient( logClient);
}
