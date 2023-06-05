#include "Channel.hpp"
#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

ChannelHandler::ChannelHandler() : _user_channels(), _channel_users(){};

std::map<std::string, std::vector<std::string> > const&
ChannelHandler::get_users() {
    return _user_channels;
}

std::map<std::string, std::vector<std::string> > const&
ChannelHandler::get_channels() {
    return _channel_users;
}

std::pair<bool, std::vector<std::string> const&> ChannelHandler::get_users(
    std::string const& channel_name) {
    std::map<std::string, std::vector<std::string> >::iterator users_iter =
        this->_channel_users.find(channel_name);

    if (users_iter == this->_channel_users.end())
        return std::make_pair(false, std::vector<std::string>());
    return std::make_pair(true, users_iter->second);
};

std::pair<bool, std::vector<std::string> const&> ChannelHandler::get_channels(
    std::string const& user_nickname) {
    std::map<std::string, std::vector<std::string> >::iterator channels_iter =
        this->_user_channels.find(user_nickname);

    if (channels_iter == this->_user_channels.end())
        return std::make_pair(false, std::vector<std::string>());
    return std::make_pair(true, channels_iter->second);
};

bool ChannelHandler::add_user(std::string const& channel_name,
                             std::string const& user_name,
                             std::string const& password) {
    if (this->does_channel_exist(channel_name) == false) {
        _channels[channel_name] = Channel(channel_name, password);
        this->_channel_users[channel_name].push_back(user_name);
        this->_user_channels[user_name].push_back(channel_name);
        this->_is_admin[std::make_pair(channel_name, user_name)] = true;
        return true;
    } else if (_channels[channel_name].password != password) {
        return false;
    }
    this->_channel_users[channel_name].push_back(user_name);
    this->_user_channels[user_name].push_back(channel_name);
    this->_is_admin[std::make_pair(channel_name, user_name)] = false;
    return true;
};

void ChannelHandler::remove_user(std::string const& channel,
                                 std::string const& user) {
    std::vector<std::string>& users = this->_channel_users[channel];
    std::vector<std::string>::iterator found =
        std::find(users.begin(), users.end(), user);
    if (found == users.end())
        return;
    users.erase(found);
};

void ChannelHandler::remove_user(std::string const& user) {
    for (std::map<std::string, std::vector<std::string> >::iterator it =
             this->_channel_users.begin();
         it != this->_channel_users.end(); ++it) {
        this->remove_user(it->first, user);
    };
    this->_user_channels[user].clear();
};

void ChannelHandler::remove_channel(std::string const& channel) {
    this->_channel_users.erase(channel);
};

bool ChannelHandler::is_admin(std::string channel, std::string user) {
    return _is_admin[std::make_pair(channel, user)];
}

bool ChannelHandler::set_is_admin(std::string channel,
                                  std::string user,
                                  bool admin) {
    return _is_admin[std::make_pair(channel, user)] = admin;
}

std::vector<std::string> ChannelHandler::list_users() {
    std::vector<std::string> users;
    for (std::map<std::string, std::vector<std::string> >::iterator it =
             _user_channels.begin();
         it != _user_channels.end(); ++it) {
        users.push_back(it->first);
    }
    return users;
};

std::vector<std::string> ChannelHandler::list_channels() {
    std::vector<std::string> channels;
    for (std::map<std::string, std::vector<std::string> >::iterator it =
             _channel_users.begin();
         it != _channel_users.end(); ++it) {
        channels.push_back(it->first);
    }
    return channels;
};

bool ChannelHandler::does_channel_exist(std::string const& channel_name) const {
    std::map<std::string, std::vector<std::string> >::const_iterator it =
        _channel_users.find(channel_name);

    if (it != _channel_users.end())
        return false;
    return true;
};

bool ChannelHandler::does_user_exist(std::string const& channel_name) const {
    std::map<std::string, std::vector<std::string> >::const_iterator it =
        _user_channels.find(channel_name);

    if (it != _user_channels.end())
        return false;
    return true;
};

// void ChannelHandler::CreateChannel(const std::string& channel_name,
//                                  const std::string& user , std::string&
//                                  password) {
//     this->addUser(channel_name, user, true)
//     _user_channels[user].push_back(channel_name);
//     _is_admin[std::make_pair(channel_name, user)] = true;
// };


    bool ChannelHandler::is_member(const std::string &channel, const std::string &user) {
        std::map<std::string, std::vector<std::string>>::iterator res = _channel_users.find(channel);
        if (res == _channel_users.end()) {
            return false;
        };
        std::vector<std::string> users = res->second;
        for(std::vector<std::string>::iterator it = users.begin(); it != users  .end(); ++it) 
            {
                if (*it == user)
                    return true;
            }
        return false;
    }
