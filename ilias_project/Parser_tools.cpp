#include "Parser.hpp"


std::pair<std::string, std::string::iterator> Parser::parse_argument(
    std::string::iterator it,
    std::string::iterator end) {
    if (it == end)
        throw IRCException::ERR_UNKNOWNCOMMAND("");
    std::string arg;
    if (*it == ':') {
        ++it;
        for (; it != end; it++) {
            arg += *it;
        }
    } else {
        for (; it != end; it++) {
            if (*it == ' ') {
                if (arg.empty())
                    throw IRCException::ERR_UNKNOWNCOMMAND("");
                return make_pair(arg, ++it);
            }
            arg += *it;
        }
    }
    if (arg.empty())
        throw IRCException::ERR_UNKNOWNCOMMAND("");
    return make_pair(arg, it);
}

std::vector<std::string> spliter(std::string str, char delim) {
    std::istringstream iss(str);
    std::string token;

    std::vector<std::string> tokens;
    while (std::getline(iss, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}

