/*
 * Event.h
 * Created on: Jun 12, 2016
 * Author: nicole
 */

#ifndef EVENT_H_
#define EVENT_H_

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h> // for stat
#include <fstream>   // std::fstream
#include <string>
#include <string.h>
#include <sys/types.h> // for size_t, off_t
#include <list>
#include <set>
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream, std::stringbuf
#include <algorithm> //  transform

#define ALREADY_SENT_RSVP " was already sent."

class Event
{
    public:

        Event( const std::string creator, int ID, const std::string title,
               const std::string date, const std::string description);

        virtual ~Event();

        int getEventId() {
            return _eventId;
        }

        std::string _getEventCreator() {
            return _creator;
        }

        std::string _getEventTitle() {
            return _title;
        }

        std::string _getEventDate() {
            return _date;
        }

        std::string _getEventDescription() {
            return _description;
        }

        /**
         * @brief: Convert event data into string in format of:
         * <eventId>\t<eventTitle>\t<eventDate>\t<eventDescription>.\n
         * @return: string event data.
         */
        std::string toString() const;


        bool isNewGuest( const std::string guest);

        std::string registerClient( const std::string guest);

        std::list<std::string> getGuestList();

        void removeGuest( const std::string guest);

    private:

        std::string _creator;
        int _eventId;
        std::string _title;
        std::string _date;
        std::string _description;
        std::set<std::string> _guestsNames;
        std::list<std::string> RSVP_List;

};

#endif /* EVENT_H_ */
