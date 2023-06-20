#pragma once
#include <stdexcept>
#include <string>

namespace IRCException {

struct ERR_SELFKICK :public std::runtime_error{
    ERR_SELFKICK(std::string nickname)
        :std::runtime_error(":irc.1337.com * * " + nickname + " :You cannot kick your self\r\n"){};
};


struct ERR_NOSUCHNICK : public std::runtime_error {
    ERR_NOSUCHNICK(std::string nickname)
        : std::runtime_error(":irc.1337.com 401 * " + nickname + " :No such nick/channel\r\n"){};
};

struct ERR_NOSUCHCHANNEL : public std::runtime_error {
    ERR_NOSUCHCHANNEL(std::string channel)
        : std::runtime_error(":irc.1337.com 403 * " + channel + " :No such channel\r\n"){};
};
// cannot send message
struct ERR_CANNOTSENDTOCHAN : public std::runtime_error {
    ERR_CANNOTSENDTOCHAN(std::string channelname)
        : std::runtime_error(":irc.1337.com 404 * " + channelname +
                             " :Cannot send to channel\r\n"){};
};


// for privmsg
struct ERR_NORECIPIENT : public std::runtime_error {
    ERR_NORECIPIENT(std::string command)
        : std::runtime_error(":irc.1337.com 411 * " + command + " :No recipient given\r\n"){};
};
// for privmsg  - 412 - 414 are returned by PRIVMSG to indicate that
//    the message wasn't delivered for some reason.
//    ERR_NOTOPLEVEL and ERR_WILDTOPLEVEL are errors that
//    are returned when an invalid use of
//    "PRIVMSG $<server>" or "PRIVMSG #<host>" is attempted.
struct ERR_NOTEXTTOSEND : public std::runtime_error {
    ERR_NOTEXTTOSEND(std::string command)
        : std::runtime_error(":irc.1337.com 412 * " + command + " :No text to send\r\n"){};
};
// Returned to a registered client to indicate that the command sent is unknown
// by the server.
struct ERR_UNKNOWNCOMMAND : public std::runtime_error {
    ERR_UNKNOWNCOMMAND(std::string command)
        : std::runtime_error(":irc.1337.com 421 * " + command + " :Unknown command\r\n"){};
};
// Returned when a nickname parameter expected for a
//   command and isn't found. //TODO: fot NICK command
struct ERR_NONICKNAMEGIVEN : public std::runtime_error {
    ERR_NONICKNAMEGIVEN() : std::runtime_error(":irc.1337.com 431 * :No nickname given\r\n"){};
};
// Returned after receiving a NICK message which contains
//   characters which do not fall in the defined set.  See
//   section x.x.x for details on valid nicknames.
struct ERR_ERRONEUSNICKNAME : public std::runtime_error {
    ERR_ERRONEUSNICKNAME(std::string nickname)
        : std::runtime_error(":irc.1337.com 432 * " + nickname + " :Erroneous nickname\r\n"){};
};
// Returned when a NICK message is processed that results
//   in an attempt to change to a currently existing
//   nickname.
struct ERR_NICKNAMEINUSE : public std::runtime_error {
    ERR_NICKNAMEINUSE(std::string nickname)
        : std::runtime_error(":irc.1337.com 433 * " + nickname +
                             " :Nickname is already in use\r\n"){};
};
//  - Returned by the server to indicate that the target
//   user of the command is not on the given channel.
struct ERR_USERNOTINCHANNEL : public std::runtime_error {
    ERR_USERNOTINCHANNEL(std::string nickname, std::string channel)
        : std::runtime_error(":irc.1337.com 441 * " + nickname + " " + channel +
                             " :They aren't on that channel\r\n"){};
};
// use this for the commands for channel
// Returned by the server whenever a client tries to
//   perform a channel effecting command for which the
//   client isn't a member.
struct ERR_NOTONCHANNEL : public std::runtime_error {
    ERR_NOTONCHANNEL(std::string channel)
        : std::runtime_error(":irc.1337.com 442 * " + channel +
                             " :You're not on that channel\r\n"){};
};
// for invit
//  Returned when a client tries to invite a user to a
//    channel they are already on.
struct ERR_USERONCHANNEL : public std::runtime_error {
    ERR_USERONCHANNEL(std::string user, std::string channel)
        : std::runtime_error(":irc.1337.com 443 * " + user + " " + channel +
                             " :is already on channel\r\n"){};
};

// Returned by the server to indicate that the client
//   must be registered before the server will allow it
//   to be parsed in detail.
struct ERR_NOTREGISTERED : public std::runtime_error {
    ERR_NOTREGISTERED()
        : std::runtime_error(":irc.1337.com 451 * :You have not registered\r\n"){};
};
// Returned by the server by numerous commands to
//    indicate to the client that it didn't supply enough
//    parameters.
struct ERR_NEEDMOREPARAMS : public std::runtime_error {
    ERR_NEEDMOREPARAMS(std::string command)
        : std::runtime_error(":irc.1337.com 461 * " + command + " :Not enough parameters\r\n"){};
};
// Returned by the server to any link which tries to
//   change part of the registered details (such as
//   password or user details from second USER message).

struct ERR_ALREADYREGISTRED : public std::runtime_error {
    ERR_ALREADYREGISTRED()
        : std::runtime_error(":irc.1337.com 462 * ::You may not reregister\r\n"){};
};
// Returned to indicate a failed attempt at registering
//   a connection for which a password was required and
//   was either not given or incorrect.
struct ERR_PASSWDMISMATCH : public std::runtime_error {
    ERR_PASSWDMISMATCH() : std::runtime_error(":irc.1337.com 464 * :Password incorrect\r\n"){};
};
// for channel with a key that alredy been setup
struct ERR_KEYSET : public std::runtime_error {
    ERR_KEYSET(std::string channel)
        : std::runtime_error(":irc.1337.com 467 * " + channel +
                             " :Channel key already set\r\n"){};
};
// for mode +l alredy limited
struct ERR_CHANNELISFULL : public std::runtime_error {
    ERR_CHANNELISFULL(std::string channel)
        : std::runtime_error(":irc.1337.com 471 * " + channel +
                             " :Cannot join channel (+l)\r\n"){};
};
// unkonw mode
struct ERR_UNKNOWNMODE : public std::runtime_error {
    ERR_UNKNOWNMODE(std::string mode)
        : std::runtime_error(":irc.1337.com 472 * " + mode +
                             " :is unknown mode char to me\r\n"){};
};
// invit only
struct ERR_INVITEONLYCHAN : public std::runtime_error {
    ERR_INVITEONLYCHAN(std::string channel)
        : std::runtime_error(":irc.1337.com 473 * " + channel +
                             " :Cannot join channel (+i)\r\n"){};
};
// bad key to access channel
struct ERR_BADCHANNELKEY : public std::runtime_error {
    ERR_BADCHANNELKEY(std::string channel)
        : std::runtime_error(":irc.1337.com 475 * " + channel +
                             " :Cannot join channel (+k)\r\n"){};
};
//  Any command requiring 'chanop' privileges (such as
//   MODE messages) must return this error if the client
//   making the attempt is not a chanop on the specified
//   channel
struct ERR_CHANOPRIVSNEEDED : public std::runtime_error {
    ERR_CHANOPRIVSNEEDED(std::string channel)
        : std::runtime_error(":irc.1337.com 482 * " + channel +
                             " :You're not channel operator\r\n"){};
};
//- Returned by the server to indicate that a MODE
//   message was sent with a nickname parameter and that
//   the a mode flag sent was not recognized.
struct ERR_UMODEUNKNOWNFLAG : public std::runtime_error {
    ERR_UMODEUNKNOWNFLAG() : std::runtime_error(":irc.1337.com 501 * :Unknown MODE flag\r\n"){};
};
// Error sent to any user trying to view or change the
//   user mode for a user other than themselves.
struct ERR_USERSDONTMATCH : public std::runtime_error {
    ERR_USERSDONTMATCH()
        : std::runtime_error(":irc.1337.com 502 * :Cannot change mode for other users\r\n"){};
};
};