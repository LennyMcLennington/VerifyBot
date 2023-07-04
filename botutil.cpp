#include "botutil.h"
#include <dpp/dpp.h>

namespace BotUtil
{
    void getAndDeleteBulk(dpp::cluster* client, dpp::snowflake channelId, dpp::snowflake around, dpp::snowflake before, dpp::snowflake after,
                          uint64_t limit)
    {
        client->messages_get(channelId, around, before, after, limit, [channelId, client](const dpp::confirmation_callback_t& callback) {
            if (callback.is_error())
            {
                dpp::error_info error = callback.get_error();
                std::string errorDetails = std::format("[{}] ERROR: Error {} [{}] on getting messages to bulk delete",
                                                       dpp::utility::current_date_time(), error.code, error.message);
                std::cout << errorDetails << std::endl;
                return;
            }

            dpp::message_map messages = std::get<dpp::message_map>(callback.value);
            std::vector<dpp::snowflake> messageIds(messages.size());
            std::ranges::transform(messages, messageIds.begin(), [](const auto& pair) { return pair.first; });
            client->message_delete_bulk(messageIds, channelId);
        });
    }

    dpp::message replyTo(const dpp::user& user, const std::string& message)
    {
        return dpp::message(user.get_mention() + ", " + message);
    }

    void shitCodeException(dpp::commandhandler& handler, const std::string& description, dpp::command_source src)
    {
        dpp::embed embed = dpp::embed()
           .set_color(dpp::colors::red)
           .set_title("Shit Code Exception")
           .set_description(description);
        handler.reply(dpp::message(src.channel_id, embed), src);
    }
}
