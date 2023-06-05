#include <ostream>
#include "IRCExecptions.hpp"
#include "Parser.hpp"

std::vector<std::pair<std::string, std::string> > Parser::parse_channel(
    std::string::iterator it,
    std::string::iterator end) {
    std::vector<std::pair<std::string, std::string> > chan_key;
    std::vector<std::string> chan;
    std::vector<std::string> args = parse_join(it, end);

    if (args[0].find(',') != std::string::npos)
        chan = spliter(args[0], ',');
    else
        chan.push_back(args[0]);
    for (size_t i = 0; i < chan.size(); i++) {
        if (chan[i][0] != '#')
            throw IRCException::ERR_NOSUCHCHANNEL(chan[i]);
        chan_key.push_back(std::make_pair(chan[i], "none"));
    }
    if (args.size() == 2) {
        std::vector<std::string> key;
        if (args[1].find(',') != std::string::npos)
            key = spliter(args[1], ',');
        else
            key.push_back(args[1]);
        for (size_t i = 0; i < key.size() && key.size() <= chan.size(); i++)
            chan_key[i].second = key[i];
    }
    return chan_key;
}
std::vector<std::string> Parser::parse_join(std::string::iterator it,
                                            std::string::iterator end) {
    if (it == end)
        throw IRCException::ERR_NEEDMOREPARAMS("JOIN");
    std::vector<std::string> args;
    std::pair<std::string, std::string::iterator> p = parse_argument(it, end);
    args.push_back(p.first);
    if (p.second == end)
        return args;
    it = p.second;
    p = parse_argument(it, end);
    args.push_back(p.first);
    if (p.second != end)
        throw IRCException::ERR_NEEDMOREPARAMS("JOIN");
    return args;
}

std::vector<std::string> Parser::parse_all(std::string::iterator it,
                                            std::string::iterator end, std::string str) {
    if (it == end)
        throw IRCException::ERR_NEEDMOREPARAMS(str);
      std::vector<std::string> args;
    if (*it == '#') {
        std::pair<std::string, std::string::iterator> p;
        while (it != end) {
            p = parse_argument(it, end);
            args.push_back(p.first);
            // if (p.second == end)
            //     return args;
            it = p.second;
        }
    }
    else
        throw IRCException::ERR_NEEDMOREPARAMS(str);
    return args;
}

std::vector<std::string> Parser::parse_mode(std::string::iterator it,
                                            std::string::iterator end) {
    if (it == end)
        throw IRCException::ERR_NEEDMOREPARAMS("MODE");
    std::vector<std::string> args;
    if (*it == '#') {
        std::pair<std::string, std::string::iterator> p;
        while (it != end) {
            p = parse_argument(it, end);
            args.push_back(p.first);
            // if (p.second == end)
            //     return args;
            it = p.second;
        }
    }
    for(size_t i = 0; i < args.size() ; i++)
    {
        std::cout << args[i] << std::endl;
    }
    if(args.size() < 2)
        throw IRCException::ERR_NEEDMOREPARAMS("MODE");
    return args;
}
std::vector<std::string> Parser::parse_topic(std::string::iterator it,
                                             std::string::iterator end) {
    if (it == end)
        throw IRCException::ERR_NEEDMOREPARAMS("TOPIC");
    std::vector<std::string> args;
    if (*it == '#') {
        std::pair<std::string, std::string::iterator> p;
        while (it != end) {
            p = parse_argument(it, end);
            args.push_back(p.first);
            if (p.second == end)
                return args;
            it = p.second;
        }
    } else {
        throw IRCException::ERR_NEEDMOREPARAMS("TOPIC");
    }
    return args;
}

std::vector<std::pair<std::string, std::string> > Parser::parse_kick(std::string::iterator it,
                                            std::string::iterator end) {
   std::vector<std::pair<std::string, std::string> > chan_key;
    std::vector<std::string> chan;
    std::vector<std::string> args = parse_all(it,end,"KICK");

    if (args.size() < 2)
        throw IRCException::ERR_NEEDMOREPARAMS("KICK");
    if (args[0].find(',') != std::string::npos)
        chan = spliter(args[0], ',');
    else
        chan.push_back(args[0]);
    for (size_t i = 0; i < chan.size(); i++) {
        if (chan[i][0] != '#')
            throw IRCException::ERR_NOSUCHCHANNEL(chan[i]);
        chan_key.push_back(std::make_pair(chan[i], "none"));
    }
        std::vector<std::string> key;
        if (args[1].find(',') != std::string::npos)
            key = spliter(args[1], ',');
        else
            key.push_back(args[1]);
        for (size_t i = 0; i < key.size() && key.size() <= chan.size(); i++)
            chan_key[i].second = key[i];
    return chan_key;
}

std::vector<std::string> Parser::parse_invite(std::string::iterator it,
                                            std::string::iterator end) {
    if (it == end)
        throw IRCException::ERR_NEEDMOREPARAMS("INVITE");
    std::vector<std::string> args;
    std::pair<std::string, std::string::iterator> p = parse_argument(it, end);
    args.push_back(p.first);
    if (p.second == end)
        return args;
    it = p.second;
    p = parse_argument(it, end);
    args.push_back(p.first);
    if (p.second != end)
        throw IRCException::ERR_NEEDMOREPARAMS("INVITE");

    return args;
}

std::vector<std::string> Parser::parse_part(std::string::iterator it,
                               std::string::iterator end) {
    std::vector<std::string> args = parse_all(it, end, "PART");
    std::vector<std::string> chan;
    if (args[0].find(',') != std::string::npos)
        chan = spliter(args[0], ',');
    else
        chan.push_back(args[0]);
    if (it == end)
        throw IRCException::ERR_NEEDMOREPARAMS("PART");
    return chan;
}
