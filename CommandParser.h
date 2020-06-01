/*
 * CommandParser.h
 * Created on: Jun 13, 2016
 * Author: nicole
 */

#ifndef COMMANDPARSER_H_
#define COMMANDPARSER_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h> // for stat
#include <fstream>   // std::fstream
#include <string>
#include <string.h>
#include <sys/types.h> // for size_t, off_t
#include <vector>
#include <map>
#include <list>
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream, std::stringbuf
#include <algorithm> // sort,distance, find_if, copy_if, transform

#define EQUAL 0
/* LIMIT of chars of event command on client side - check it's valid command*/
#define MAX_TITLE 30
#define MAX_DATE 30
#define MAX_DESC 256
#define EQUAL 0

const std::string NOT_REGISTERED = "ERROR: first command must be REGISTER.";
const std::string ILLEGAL_COMMAND = "ERROR: illegal command.";
const std::string EXIT_COMMAND = "EXIT";
const std::string ALREADY_REGISTERED = " is already exists.";
const std::string CLIENT_REGISTERED = " was already registered.";
const std::string REGISTER_SUCCESS = " was registered successfully.";
const std::string EVENT_NOT_EXIST = "event does not exist.";
const std::string ERROR_EVENT_ALLOC = "ERROR: cannot allocate new event.";


enum Commands
{
    REGISTER = 0,
    CREATE = 1,
    UNREGISTER = 2,
    EXIT = 3,
    SEND_RSVP = 4,
    GET_RSVPS_LIST = 5,
    GET_TOP_5 = 6,
    ILLEGAL = 7
};

enum CommandResult
{
    COMMAND_NOTEXIST = 0,
    NO_REGISTER = 1,
    MISS_ARGS = 2,
    INVALID_ARG_COMMAND = 3,
    ALREADY_REGISTER = 4
};


class CommandParser
{
        typedef std::map<std::string, int> commandsMap;

    public:


        /**
         * @return: the command type according to the Commands enum.
         */
        static int getCommandType( std::string command);


        /**
         * @brief: function splitsline into tokens vector by sep separator.
         * @param line: the command line to split into tokens.
         * @param sep: separator- usually a space char.
         * @param tokens: empty vector of strings - to store tokens result.
         */
        static void tokenize( const std::string &line, const std::string sep,
                              std::vector<std::string> &tokens);


        /**
         * @brief: function joins string tokens from tokens vector from index
         * with sep separator into one string instead of the previous tokens.
         * @param tokens: vector of string tokens - to get tokens to join.
         * @param sep: separator- usually a space char.
         * @param idx: index from where to join tokens.
         */
        static void joinTokens( std::vector<std::string>& tokens,
                                const std::string sep, int idx);

        /**
         * @bried: Check whether the given string represent a valid number.
         * meaning the str string can be converted to a valid number (integer
         * or a double).
         * @return: true if string is numeric or false otherwise.
         */
        static bool isNumeric( const char *str);

        /**
         * @return: true if string represents number.
         */
        static bool isStrNumber( const std::string& s);


        /**
         * @brief: none sensitive comparison.
         * @return: true is strings are equal in not sensitive case
         * comparison.
         */
        static bool compare_nocase( const std::string& first,
                                    const std::string& second);

        /**
         * @brief: converts the given list of strings into one string, while
         * the string tokens will be separated by sepatator string sep.
         * @return: string of the tokens in the list.
         */
        static std::string convertListToString( std::list<std::string> list,
                                                const std::string sep);


        static std::string logCommandError( const std::string clientName,
                                            CommandResult errType,
                                            const std::string command = "",
                                            const std::string arg = "");

    private:

        /**
         * private constructor - since this class only serves as an interface
         * of static helper functions thus no need for instance creation.
         */
        CommandParser();

        /**
         * private distructor.
         */
        virtual ~CommandParser();

};


#endif /* COMMANDPARSER_H_ */
