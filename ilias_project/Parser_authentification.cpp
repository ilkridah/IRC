
#include "Parser.hpp"

std::string Parser::parse_pass(std::string::iterator it,
                               std::string::iterator end) {
    if (it == end)
        throw IRCException::ERR_NEEDMOREPARAMS("PASS");
    try {
        std::pair<std::string, std::string::iterator> p =
            parse_argument(it, end);
        return p.first;
    } catch (std::exception& e) {
        throw IRCException::ERR_NEEDMOREPARAMS("PASS");
    }
}

void Parser::check_nick_rules(std::string str) {
    if (str.size() < 4 || str.size() > 30)
        throw IRCException::ERR_ERRONEUSNICKNAME(str);
    int i = 0;
    while (str[i]) {
        if (isspace(str[i]) ||
            (!isdigit(str[i]) && !isalpha(str[i]) && str[i] != '_' &&
             str[i] != '-' && str[i] != '[' && str[i] != ']')) {
            throw IRCException::ERR_ERRONEUSNICKNAME(str);
        }
        i++;
    }
}

std::string Parser::parse_nick(std::string::iterator it,
                               std::string::iterator end) {
    if (it == end)
        throw IRCException::ERR_NONICKNAMEGIVEN();
    std::pair<std::string, std::string::iterator> p =
            parse_argument(it, end);
        Parser::check_nick_rules(p.first);
        return p.first;

}

std::vector<std::string> Parser::parse_user(std::string::iterator it,
                                            std::string::iterator end) {
    std::vector<std::string> args;
    if (it == end)
        throw IRCException::ERR_NEEDMOREPARAMS("USER");
    while (true) {
        std::pair<std::string, std::string::iterator> p =
            parse_argument(it, end);
        args.push_back(p.first);
        it = p.second;
        if (p.second == end)
            break;
        }
    if (args.size() != 4)
        throw IRCException::ERR_NEEDMOREPARAMS("USER");
    return args;
}