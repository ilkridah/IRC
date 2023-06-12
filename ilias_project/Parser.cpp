#include "Parser.hpp"
#include <string>

std::string to_upper(const std::string& str) {
    std::string res;
    for (std::size_t i = 0; i < str.length(); ++i) {
        res += std::toupper(str[i]);
    }
    return res;
}

std::pair<std::string, std::string::iterator> Parser::get_command(
    std::string::iterator it,
    std::string::iterator end) {
    return parse_argument(it, end);
}

Parser::Command Parser::operator()(std::string str) {
    Command cmd;
    cmd.command = NOT_FOUND;
    cmd.ath = NOT_REGISTRED;
    std::pair<std::string, std::string::iterator> p =
        get_command(str.begin(), str.end());
    // check '\r\n' do please ????!!!?!?!!??!
    if (to_upper(p.first) == "PASS") {
        cmd.ath = PASS;
        std::string arg = parse_pass(p.second, str.end());
        cmd.args.push_back(arg);
    } else if (to_upper(p.first) == "NICK") {
        cmd.ath = NICK;
        std::string arg = parse_nick(p.second, str.end());
        cmd.args.push_back(arg);
    } else if (to_upper(p.first) == "USER") {
        cmd.ath = USER;
        cmd.args = parse_user(p.second, str.end());
    } else if (to_upper(p.first) == "PRIVMSG") {
        cmd.command = PRIVMSG;
        cmd.args = parse_privmsg(p.second, str.end());
    } else if (to_upper(p.first) == "QUIT") {
        cmd.command = QUIT;
        cmd.args.push_back(parse_quit(p.second, str.end()));
    } else if (to_upper(p.first) == "JOIN") {
        cmd.command = JOIN;
        cmd.args = parse_join(p.second, str.end());
        if (cmd.args[0] != "0")
            cmd.chan_key = parse_channel(p.second, str.end());
    } else if (to_upper(p.first) == "MODE") {
        cmd.command = MODE;
        cmd.args = parse_mode(p.second, str.end());
    } else if (to_upper(p.first) == "TOPIC") {
        cmd.command = TOPIC;
        cmd.args = parse_topic(p.second, str.end());
    } else if (to_upper(p.first) == "PART") {
        cmd.command = PART;
        cmd.args = parse_part(p.second, str.end());
    } else if (to_upper(p.first) == "KICK") {
        cmd.command = KICK;
        cmd.args = parse_all(p.second, str.end(),"KICK");
        cmd.chan_key = parse_kick(p.second, str.end());
    } else if (to_upper(p.first) == "INVITE") {
        cmd.command = INVITE;
        cmd.args = parse_invite(p.second, str.end());
    } else if (to_upper(p.first) == "/WEATHER") {
        cmd.command = WEATHER;
        cmd.args = parse_invite(p.second, str.end());
    } else if (to_upper(p.first) == "NAMES") {
        cmd.command = NAMES;
        cmd.args.push_back(parse_names(p.second, str.end()));
    } else
        throw IRCException::ERR_UNKNOWNCOMMAND(p.first);
    return cmd;
};

std::vector<std::string> Parser::parse_privmsg(std::string::iterator it,
                                               std::string::iterator end) {
    std::vector<std::string> args;
    if (it == end)
        throw IRCException::ERR_NEEDMOREPARAMS("PRIVMSG");
    while (true) {
        std::pair<std::string, std::string::iterator> p =
            parse_argument(it, end);
        args.push_back(p.first);
        it = p.second;
        if (p.second == end)
            break;
    }
    if (args.size() < 2)
        throw IRCException::ERR_NEEDMOREPARAMS("PRIVMSG");
    return args;
}

std::string Parser::parse_quit(std::string::iterator it,
                               std::string::iterator end) {
    try {
        std::pair<std::string, std::string::iterator> p =
            parse_argument(it, end);
        return p.first;
    } catch (std::exception& e) {
    }
    return "";
}

std::string Parser::parse_names(std::string::iterator it,
                                std::string::iterator end) {
    try {
        std::pair<std::string, std::string::iterator> p =
            parse_argument(it, end);
        return p.first;
    } catch (std::exception& e) {
    }
    return "";
}
