#pragma once
#include <sys/_types/_size_t.h>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

struct Channel {
    std::string name;
    std::string password;
    std::string topic;
    bool InviteOnly;
    size_t limit;
    Channel(std::string const& channel_name, std::string const& password)
        : name(channel_name),
          password(password),
          topic(""),
          InviteOnly(false),
          limit(0) {}
};

class ChannelHandler {
   private:
    std::map<std::string, Channel> _channels;
    std::map<std::string, std::vector<std::string> > _user_channels;
    std::map<std::string, std::vector<std::string> > _channel_users;
    std::map<std::pair<std::string, std::string>, bool> _is_admin;
    std::map<std::pair<std::string, std::string>, bool> _is_invited;

   public:
    ChannelHandler();

    std::vector<std::string> list_users();
    std::vector<std::string> list_channels();

    std::map<std::string, std::vector<std::string> > const& get_users();
    std::map<std::string, std::vector<std::string> > const& get_channels();
    Channel& get_channel(std::string const& channel_name);

    std::pair<bool, std::vector<std::string> const&> get_users(
        std::string const& channel_name);
    std::pair<bool, std::vector<std::string> const&> get_channels(
        std::string const& user_nickname);

    bool is_admin(std::string channel, std::string user);
    bool set_is_admin(std::string channel, std::string user, bool admin);

    bool is_invited(std::string channel, std::string user);
    bool set_is_invited(std::string channel, std::string user, bool admin);

    bool add_user(std::string const& channel_name,
                 std::string const& user_name,
                 std::string const& password);

    // void add_channel(std::string const& channel);
    void remove_user(std::string const& channel, std::string const& user);
    void remove_user(std::string const& user);
    void remove_channel(std::string const& channel);
    ////
    bool does_channel_exist(std::string const& channel_name) const;
    bool does_user_exist(std::string const& nickname) const;
    ////
    void made_channel(std::string const& channel_name);
    void CreateChannel(const std::string& channel_name,
                       const std::string& user,
                       std::string& password);
    void get_key(std::string const& channel_name, std::string const& key);
};
