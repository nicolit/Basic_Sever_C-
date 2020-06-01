/*
 * Logger.cpp
 * Created on: Jun 9, 2016
 * Author: nicole
 */

#include "Logger.h"

Logger::Logger( const std::string fileName)
{
	_logPath = fileName;
	_createLogFile();
}

Logger::~Logger()
{
    closeLog();
}

/**
 * @brief: static function - document an operation to the log file.
 */
void Logger::logCommand( const std::string command)
{
	time_t current_time;
	struct tm * time_info;
	std::string line;
	char timeString[9];  // space for "HH:MM:SS\0"

	if ( _logFile.is_open())
	{
		time(&current_time);
		time_info = localtime( &current_time);

		strftime( timeString, sizeof(timeString), "%H:%M:%S", time_info);
		line = std::string(timeString) + " \t" + command + "\n";
		_logFile << line;
		flushLogFile();
	}
}


void Logger::logEvent(int ID, const std::string title,const std::string time,
						  const std::string description)
{
	std::string command = std::to_string(ID) + "\t" + title + time + "\t";
	command += description;
	logCommand( command);
}


/**
 * @brief: flush the log file. force any unwritten data to be delivered
 * to the host environment to be written to the log file.
 */
void Logger::flushLogFile()
{
	_logFile.exceptions(std::ifstream::failbit| std::ifstream::badbit);

	try {
		_logFile.flush();

	} catch ( std::ifstream::failure& e) {
		std::cerr << e.what() << std::endl;
	}
}


/**
 * @return: the log's path.
 */
std::string Logger::getLogFullPath() const
{
	return _logPath;
}


void Logger::closeLog()
{
    /* close the log file */
    try {
        if ( _logFile.is_open()) {
            _logFile.close();
        }
    } catch ( std::ifstream::failure& e) {
        std::cerr << e.what() << std::endl;
    }
}

/**
 * @brief: create a new log file in the root dir.
 * @param rootDir: log's file root dir.
 */
void Logger::_createLogFile()
{
	_logFile.open( _logPath.c_str(), std::fstream::in | std::fstream::out |
				   std::fstream::app);

	if ( !_logFile.is_open()) {
		std::cout << "Error opening file" << std::endl;
	}
}

