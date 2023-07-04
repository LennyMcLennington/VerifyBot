#include "commands.h"
#include "botconfig.h"
#include "botutil.h"
#include "constants.h"
#include <cpr/cpr.h>
#include <dpp/dpp.h>

namespace Commands
{
    enum class VerifyState { NoSteamConnected, NoBtd6, Success };

    void makeFaq(dpp::commandhandler& handler, const std::string& command, const dpp::parameter_list_t& params, const dpp::command_source& src)
    {
        if (params[0].second.index() == 0)
        {
            handler.reply(BotUtil::replyTo(src.issuer, "need a channel."), src);
            return;
        }

        dpp::channel c = std::get<dpp::channel>(params[0].second);
        if (!c.is_text_channel())
        {
            handler.reply(BotUtil::replyTo(src.issuer, "channel must be a text channel."), src);
            return;
        }

        dpp::embed embed = dpp::embed()
           .set_color(dpp::colors::blue)
           .set_title("FAQ")
           .add_field("How do I get the mods?", std::format(Constants::FAQA1TPL, c.get_mention()))
           .add_field("How do I make mods?", Constants::FAQA2TPL)
           .add_field("Will I get banned for using mods?", Constants::FAQA3TPL)
           .add_field("How can I test upcoming mods?", Constants::FAQA4TPL)
           .add_field("How do I get the YouTubers role?", std::format(Constants::FAQA5TPL, dpp::channel::get_mention(Constants::YTAnnounceChanId)))
           .add_field("How do I get the Modders role?", Constants::FAQA6TPL);
        handler.owner->message_create(dpp::message(c.id, embed));
    }

    void verify(dpp::commandhandler& handler, const std::string& command, const dpp::parameter_list_t& params, const dpp::command_source& src)
    {
        if (src.channel_id != Constants::VerifyChanId)
        {
            std::string failReply = std::format(Constants::VerifyWrongChannelTPL, dpp::channel::get_mention(Constants::VerifyChanId));
            handler.reply(BotUtil::replyTo(src.issuer, failReply), src);
            return;
        }

        cpr::Response response = cpr::Get(
            cpr::Url(std::format("https://discordapp.com/api/v6/users/{}/profile", (uint64_t)src.issuer.id)),
            cpr::Header{ { "Authorization", BotConfig::instance().userToken() } }
        );

        if (response.status_code >= 400)
        {
            BotUtil::shitCodeException(handler, std::format(Constants::VerifyDAPIFailTPL, response.status_code, response.status_line), src);
            return;
        }

        nlohmann::json responseJson = nlohmann::json::parse(response.text);
        VerifyState verifyState = VerifyState::NoSteamConnected;
        for (const nlohmann::basic_json<>& item : responseJson["connected_accounts"])
        {
            std::string type = item["type"].template get<std::string>();
            if (type != "steam")
                continue;

            verifyState = VerifyState::NoBtd6;
            std::string id = item["id"].template get<std::string>();
            std::string ownedUrl = std::format(
                "https://api.steampowered.com/IPlayerService/GetOwnedGames/v1/?key={}&steamid={}",
                BotConfig::instance().steamApiKey(), id
            );

            cpr::Response ownedResponse = cpr::Get(cpr::Url(ownedUrl));
            if (ownedResponse.status_code >= 400)
            {
                BotUtil::shitCodeException(handler, std::format(Constants::VerifySAPIFailTPL, response.status_code, response.status_line), src);
                return;
            }

            nlohmann::json ownedResponseJson = nlohmann::json::parse(ownedResponse.text);
            if (ownedResponseJson.empty())
                continue;

            bool ownsBtd6 = std::ranges::any_of(ownedResponseJson["response"]["games"], [](const nlohmann::basic_json<>& item) {
                return item["appid"].template get<uint32_t>() == Constants::BTD6SteamId;
            });

            if (ownsBtd6)
            {
                verifyState = VerifyState::Success;
                break;
            }
        }

        switch (verifyState)
        {
        case VerifyState::NoBtd6:
            handler.reply(BotUtil::replyTo(src.issuer, Constants::VerifyNoBTD6TPL), src);
            handler.owner->message_delete(src.message_event->msg.id, src.channel_id);
            break;
        case VerifyState::NoSteamConnected:
            handler.reply(BotUtil::replyTo(src.issuer, Constants::VerifyNoSteamTPL), src);
            handler.owner->message_delete(src.message_event->msg.id, src.channel_id);
            break;
        case VerifyState::Success:
            handler.reply(BotUtil::replyTo(src.issuer, "you were verified successfully!"), src);
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            handler.owner->guild_member_add_role(src.guild_id, src.issuer.id, Constants::VerifiedRoleId);
            BotUtil::getAndDeleteBulk(handler.owner, src.channel_id, 0, 0, Constants::VerifyInstructMsgId);
            break;
        }
    }
}
