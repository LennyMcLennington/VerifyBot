#ifndef BOTUTIL_H
#define BOTUTIL_H
#include <dpp/commandhandler.h>

namespace BotUtil
{
    void getAndDeleteBulk(dpp::cluster* client, dpp::snowflake channelId, dpp::snowflake around, dpp::snowflake before, dpp::snowflake after,
                          uint64_t limit = 50);
    dpp::message replyTo(const dpp::user& user, const std::string& message);
    void shitCodeException(dpp::commandhandler& handler, const std::string& description, dpp::command_source src);
}

#endif // BOTUTIL_H
