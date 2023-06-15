#include "IRC.hpp"

void IRC::authentification(Client& client, const Parser::Command& cmd) {
    if (!(client.is_pass_valid()) && cmd.ath == Parser::PASS &&
        _password == cmd.args[0]) {
        client.set_pass_valid();
    } else if (!(client.is_pass_valid()) && cmd.ath == Parser::PASS) {
        throw IRCException::ERR_PASSWDMISMATCH();
    }
    if (!client.is_pass_valid()){
        IRCReplay::RPL_PASSNOTICE(client);
        return;}
    if (cmd.ath == Parser::NICK) {
        if (_nickname_pool.find(cmd.args[0]) == _nickname_pool.end()) {
            if (client.is_nick_set()) {
                _nickname_pool.erase(client.get_nick());
            }
            client.set_nick(cmd.args[0]);
            _nickname_pool.insert(make_pair(cmd.args[0], &client));
        } else {
            throw IRCException::ERR_NICKNAMEINUSE(cmd.args[0]);
        }
    }
    if (cmd.ath == Parser::USER) {
        if (_username_pool.find(cmd.args[0]) == _username_pool.end()) {
            if (client.is_user_set()) {
                _username_pool.erase(client.get_user());
            }
            client.set_user(cmd.args[0], cmd.args[1], cmd.args[2], cmd.args[3]);
            _username_pool.insert(cmd.args[0]);
            client.set_local_host(client.get_local_host());
        } else {
            throw IRCException::ERR_ALREADYREGISTRED();
        }
    }
}



