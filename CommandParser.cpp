/*
 * CommandParser.cpp
 * Created on: Jun 13, 2016
 * Author: nicole
 */

#include "CommandParser.h"

/**
 * @return: the command type according to the Commands enum.
 */
int CommandParser::getCommandType( std::string command)
{
    commandsMap commTypesMap = {
                    { "REGISTER" , 0},
                    { "CREATE" , 1},
                    { "UNREGISTER" , 2},
                    { "EXIT" , 3},
                    { "SEND_RSVP" , 4},
                    { "GET_RSVPS_LIST" , 5},
                    { "GET_TOP_5" , 6},
                    { "ILLEGAL", 7}
                  };

    if ( commTypesMap.find( command) != commTypesMap.end() ) {
        return commTypesMap[command];
    }
    return commTypesMap["ILLEGAL"];
}


/**
 * @brief: function splitsline into tokens vector by sep separator.
 * @param line: the command line to split into tokens.
 * @param sep: separator- usually a space char.
 * @param tokens: empty vector of strings - to store tokens result.
 */
void CommandParser::tokenize( const std::string &line, const std::string sep,
                              std::vector<std::string> &tokens)
{
    std::size_t start = 0, end = 0, hasNewLine = 0;
    std::string temp;

    while ((end = line.find(sep, start)) != std::string::npos)
    {
        temp = line.substr( start, end - start);
        if (temp != "") {
            hasNewLine = temp.find("\n");
            if (hasNewLine != std::string::npos){
                temp = temp.substr( 0, hasNewLine);
            }
            tokens.push_back(temp);
        }
        start = end + 1;
    }

    temp = line.substr(start);
    if (temp != "") tokens.push_back(temp);
}


/**
 * @brief: function joins string tokens from tokens vector from idx index
 * with sep separator into one string instead of the previous tokens.
 * @param tokens: vector of string tokens - to get tokens to join.
 * @param sep: separator- usually a space char.
 * @param idx: index from where to join tokens.
 */
void CommandParser::joinTokens( std::vector<std::string>& tokens,
                                const std::string sep, int idx)
{
    std::string description;
    std::stringstream stream;
    std::vector<std::string>::iterator it;

    if ( tokens.size() >= 4) {
        it = tokens.begin() + idx;
        while ( it != tokens.end()) {
          stream << *it;
          stream << sep;
          // delete current token and advance to next
          tokens.erase(it);
        }

        description = stream.str();
        description.erase( description.size() - 1);
        tokens.push_back( description);
    }
}


/**
 * @bried: Check whether the given string represent a valid number.
 * meaning the str string can be converted to a valid number (integer
 * or a double).
 * @return: true if string is numeric or false otherwise.
 */
bool CommandParser::isNumeric( const char *str)
{
    unsigned int i = 0;
    unsigned int strLen = 0;

    if (str == NULL) {
        return false;
    }
    strLen = strlen(str);

    while (i < strLen) {
        if (!isdigit( *(str+i))) {
            return false;
        }
        i++;
    }

    return true;
}


/**
 * @return: true if string represents number.
 */
bool CommandParser::isStrNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}


/**
 * @brief: none sensitive comparison.
 * @return: true is strings are equal in not sensitive case
 * comparison.
 */
bool CommandParser::compare_nocase( const std::string& first,
                                    const std::string& second)
{
  unsigned int i=0;
  while (( i < first.length()) && ( i < second.length()) )
  {
    if ( tolower( first[i]) < tolower( second[i])) return true;
    else if ( tolower( first[i]) > tolower( second[i])) return false;
    ++i;
  }
  return ( first.length() < second.length() );
}


/**
 * @brief: converts the given list of strings into one string, while
 * the string tokens will be separated by sepatator string sep.
 * @return: string of the tokens in the list.
 */
std::string CommandParser::convertListToString( std::list<std::string> list, const std::string sep)
{
    std::string listStr;
    std::list<std::string>::iterator it;

    /* first sort list by names */
    list.sort( CommandParser::compare_nocase);

    /* append each guest name to string list */
    for (it = list.begin(); it != list.end(); ++it) {
        listStr += *it + sep;
    }

    /* remove last comma char */
    listStr.erase( listStr.size() - 1);
    return listStr;
}

std::string CommandParser::logCommandError( const std::string clientName,
                                            CommandResult errType,
                                            const std::string command,
                                            const std::string arg)
{
    std::string errMsg;

    switch( errType)
    {
        case CommandResult::COMMAND_NOTEXIST:
            errMsg = ILLEGAL_COMMAND;
           break;

        case CommandResult::MISS_ARGS:
            errMsg = "ERROR: missing arguments in command " + command + ".";
           break;

        case CommandResult::INVALID_ARG_COMMAND:
            errMsg ="ERROR: invalid argument "+arg+" in command "+command + ".";
            break;

        case CommandResult::NO_REGISTER:
            errMsg = NOT_REGISTERED;
            break;

        case CommandResult::ALREADY_REGISTER:
            errMsg = "ERROR: " + clientName + "\t" + ALREADY_REGISTERED;
            break;
    }

    return errMsg;
}

/*************** Private Functions **************************************/

/**
 * private constructor - since this class only serves as an interface
 * of static helper functions thus no need for instance creation.
 */
CommandParser::CommandParser()
{}

/**
 * private distructor.
 */
CommandParser::~CommandParser()
{}
