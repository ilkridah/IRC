#ifndef PARSER_HPP
#define PARSER_HPP
#include <iostream>
#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <string>
#include "IRCExecptions.hpp"

// std::pair<std::string, std::string::iterator> get_command(
//     std::string::iterator it,
//     std::string::iterator end) {
//     std::string command;
//     for (; it != end; it++) {
//         if (*it == ' ') {
//             return make_pair(command, it);
//         }
//         command += *it;
//     }
//     return make_pair(command, it);
// }

class Parser {
   public:
    typedef enum CommandEnum {

        PRIVMSG,
        QUIT,
        JOIN,
        MODE,
        TOPIC,
        PART,
        KICK,
        INVITE,
        NAMES,
        WEATHER,
        NOT_FOUND
    } CommandEnum;

    typedef enum ath {
        PASS,
        NICK,
        USER,
        NOT_REGISTRED
    } ath;
    struct Command {
        CommandEnum command;
        ath ath;
        std::vector<std::string> args;
        std::vector<std::pair<std::string, std::string> > chan_key;
        Command() {}
    };

    Command operator()(std::string str);

   private:
    static std::pair<std::string, std::string::iterator> get_command(
        std::string::iterator it,
        std::string::iterator end);
    static std::string parse_pass(std::string::iterator it,
                                  std::string::iterator end);
    static std::string parse_nick(std::string::iterator it,
                                  std::string::iterator end);
    static std::string parse_quit(std::string::iterator it,
                                  std::string::iterator end);
    static std::vector<std::string> parse_user(std::string::iterator it,
                                               std::string::iterator end);
    static std::vector<std::string> parse_privmsg(std::string::iterator it,
                                                  std::string::iterator end);
    static std::vector<std::string> parse_join(std::string::iterator it,
                                               std::string::iterator end);
    static std::vector<std::pair<std::string, std::string> > parse_channel(
        std::string::iterator it,
        std::string::iterator end);
    static std::pair<std::string, std::string::iterator> parse_argument(
        std::string::iterator it,
        std::string::iterator end);
    static std::vector<std::string> parse_mode(std::string::iterator it,
                                               std::string::iterator end);
    static std::vector<std::string> parse_topic(std::string::iterator it,
                                                std::string::iterator end);
    static std::vector<std::string> parse_part(std::string::iterator it,
                                  std::string::iterator end);
    static std::vector<std::pair<std::string, std::string> > parse_kick(std::string::iterator it,
                                            std::string::iterator end);
    static std::vector<std::string> parse_invite(std::string::iterator it,
                                            std::string::iterator end);
    static std::vector<std::string> parse_all(std::string::iterator it,
                                            std::string::iterator end, std::string str);
    static std::vector<std::string> parse_names(std::string::iterator it,
                                std::string::iterator end);

    

    // TODO:
    static void check_nick_rules(std::string str);
    static void check_name_rules(std::string str);
};
 std::vector<std::string> spliter(std::string str, char delim);

#endif