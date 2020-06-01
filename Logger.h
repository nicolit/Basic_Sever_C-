/*
 * Logger.h
 * Created on: Jun 9, 2016
 * Author: nicole
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h> // for stat
#include <iostream>
#include <fstream>   // std::fstream
#include <string>
#include <string.h>
#include <sys/types.h> // for size_t, off_t


class Logger
{
public:


	Logger( const std::string fileName);

	virtual ~Logger();

	/**
	 * @brief: static function - document an operation to the log file.
	 */
	void logCommand( const std::string command);


	void logEvent( int ID, const std::string title, const std::string time,
				   const std::string description);

	/**
	 * @brief: flush the log file. force any unwritten data to be delivered
	 * to the host environment to be written to the log file.
	 */
	void flushLogFile();


	/**
	 * @return: the log's path.
	 */
	std::string getLogFullPath() const;


	void closeLog();


private:

	std::string _logPath; /* absolute log path. */
	std::fstream _logFile;


	/**
	 * @brief: create a new log file in the root dir.
	 * @param rootDir: log's file root dir.
	 */
	void _createLogFile();

};

#endif /* LOGGER_H_ */
