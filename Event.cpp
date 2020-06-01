/*
 * Event.cpp
 * Created on: Jun 12, 2016
 * Author: nicole
 */

#include "Event.h"

Event::Event( const std::string creator, int ID, const std::string title,
              const std::string date, const std::string description):
        _creator( creator), _eventId( ID), _title( title), _date( date),
        _description( description)
{
    // TODO Auto-generated constructor stub

}

Event::~Event()
{
    RSVP_List.clear();
}

/**
 * @brief: Convert event data into string in format of:
 * <eventId>\t<eventTitle>\t<eventDate>\t<eventDescription>.\n
 * @return: string event data.
 */
std::string Event::toString() const
{
    std::string eventInfo = "";
    std::string fileName = std::to_string(_eventId) + '\t' + _title + '\t';
    eventInfo += _date + '\t' + _description + ".\n";
    return eventInfo;
}

bool Event::isNewGuest( const std::string guest)
{
    std::set<std::string>::iterator it;
    std::string guestIn( guest);
    /* transform first command word to upper case */
    std::transform( guestIn.begin(), guestIn.end(),guestIn.begin(), ::toupper);

    it = _guestsNames.find( guestIn);

    if ( it != _guestsNames.end()) {
        return false;
    }
    return true;
}


std::string Event::registerClient( const std::string guest)
{
    bool isNew;
    std::string guestIn( guest);
    /* transform first command word to upper case */
    std::transform( guestIn.begin(), guestIn.end(),guestIn.begin(), ::toupper);

    isNew = isNewGuest( guestIn);
    std::string response;

    if ( isNew) {
        _guestsNames.insert( guestIn);
        RSVP_List.push_back( guestIn);
        response = "SUCCESS";
    } else {
        response = "RSVP to event id " + std::to_string(_eventId) + ALREADY_SENT_RSVP;
    }

    return response;
}

std::list<std::string> Event::getGuestList()
{
    return RSVP_List;
}

void Event::removeGuest( const std::string guest)
{
    bool isNew;
    std::list<std::string>::iterator it;
    std::string guestIn( guest);
    /* transform first command word to upper case */
    std::transform( guestIn.begin(), guestIn.end(),guestIn.begin(), ::toupper);

    isNew = isNewGuest( guestIn);

    if (!isNew) {
        _guestsNames.erase( guestIn);
        it = RSVP_List.begin();
        while ( it != RSVP_List.end()) {
            if ( (*it).compare( guestIn) == 0) {
                RSVP_List.erase(it);
                break;
            }
            it++;
        }
    }
}

