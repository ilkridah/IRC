
#include "Parser.hpp"

std::vector<std::pair<std::string, std::string> > Parser::parse_channel(
    std::string::iterator it,
    std::string::iterator end) {
    std::vector<std::pair<std::string, std::string> > chankey;
    std::vector<std::string> chan;
    std::vector<std::string> args = parse_join(it, end);

    if (args[0].find(',') != std::string::npos)
        chan = spliter(args[0], ',');
    else
        chan.push_back(args[0]);
    for (size_t i = 0; i < chan.size(); i++) {
        if (chan[i][0] != '#')
            throw IRCException::ERR_NOSUCHCHANNEL(chan[i]);
        chankey.push_back(std::make_pair(chan[i], ""));
    }
    if (args.size() == 2) {
        std::vector<std::string> key;
        if (args[1].find(',') != std::string::npos)
            key = spliter(args[1], ',');
        else
            key.push_back(args[1]);
        for (size_t i = 0; i < key.size() && key.size() <= chan.size(); i++)
            chankey[i].second = key[i];
    }
    return chankey;
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
                                           std::string::iterator end,
                                           std::string str) {
    if (it == end)
        throw IRCException::ERR_NEEDMOREPARAMS(str);
    std::vector<std::string> args;
    if (*it == '#') {
        std::pair<std::string, std::string::iterator> p;
        while (it != end) {
            p = parse_argument(it, end);
            args.push_back(p.first);
            it = p.second;
        }
    } else
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
            it = p.second;
        }
    } else {
        std::pair<std::string, std::string::iterator> p;
        while (it != end) {
            p = parse_argument(it, end);
            args.push_back(p.first);
            it = p.second;
        }
    }
    if (args.size() < 2)
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

std::vector<std::pair<std::string, std::string> > Parser::parse_kick(
    std::string::iterator it,
    std::string::iterator end) {
    std::vector<std::pair<std::string, std::string> > chan_key;
    std::vector<std::string> args = parse_all(it, end, "KICK");

    if (args.size() < 2)
        throw IRCException::ERR_NEEDMOREPARAMS("KICK");

    std::vector<std::string> chan;
    if (args[0].find(',') != std::string::npos)
        chan = spliter(args[0], ',');
    else
        chan.push_back(args[0]);

    std::vector<std::string> key;
    if (args[1].find(',') != std::string::npos)
        key = spliter(args[1], ',');
    else
        key.push_back(args[1]);

    std::vector<std::string>::iterator chanIter = chan.begin();
    std::vector<std::string>::iterator keyIter;
    for (; chanIter != chan.end(); chanIter++) {
        keyIter = key.begin();
        if ((*chanIter)[0] != '#')
            throw IRCException::ERR_NOSUCHCHANNEL(*chanIter);
        for(;keyIter != key.end(); keyIter++)
            chan_key.push_back(std::make_pair(*chanIter, *keyIter));
    }

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
std::vector<std::string> Parser::parse_api(std::string::iterator it,
                                           std::string::iterator end) {
    if (it == end)
        throw IRCException::ERR_NEEDMOREPARAMS("/WEATHER/API");
    std::vector<std::string> args;
    std::pair<std::string, std::string::iterator> p = parse_argument(it, end);
    args.push_back(p.first);
    if (p.second == end)
        return args;
    it = p.second;
    p = parse_argument(it, end);
    args.push_back(p.first);
    if (p.second != end)
        throw IRCException::ERR_NEEDMOREPARAMS("/WEATHER/API");

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
