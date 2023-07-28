#ifndef BOTUTIL_H
#define BOTUTIL_H
#include <dpp/cluster.h>

namespace BotUtil
{
    void epicGamesVerify(dpp::cluster& client, const dpp::form_submit_t& event, const std::string& authCode);
    void getAndDeleteBulk(dpp::cluster& client, dpp::snowflake channelId, dpp::snowflake around, dpp::snowflake before, dpp::snowflake after,
                          uint64_t limit = 50);
    void shitCodeException(const dpp::slashcommand_t& event, const std::string& description);
}

#endif // BOTUTIL_H
