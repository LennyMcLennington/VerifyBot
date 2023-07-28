#include "commands.h"
#include "botconfig.h"
#include "botutil.h"
#include "constants.h"
#include <cpr/cpr.h>
#include <dpp/dpp.h>

namespace Commands
{
    enum class VerifyState { NoSteamConnected, NoBtd6, Success };

    void makeFaq(dpp::cluster& client, const dpp::slashcommand_t& event)
    {
        dpp::command_value parameter = event.get_parameter("channel");
        if (parameter.index() == 0)
        {
            event.reply(dpp::message("Need a channel."));
            return;
        }

        dpp::channel c = client.channel_get_sync(std::get<dpp::snowflake>(parameter));
        if (!c.is_text_channel())
        {
            event.reply(dpp::message("Channel must be a text channel."));
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
        client.message_create(dpp::message(c.id, embed));
    }

    void verifyEpicGames(dpp::cluster& client, const dpp::slashcommand_t& event)
    {
        if (event.command.channel_id != Constants::VerifyChanId)
        {
            std::string failReply = std::format(Constants::VerifyWrongChannelTPL, dpp::channel::get_mention(Constants::VerifyChanId));
            event.reply(dpp::message(failReply));
            return;
        }

        dpp::interaction_modal_response modal("verify_modal", "Epic Games Verification");
        modal.add_component(
            dpp::component()
                .set_label("Authorization code")
                .set_id("auth_code")
                .set_required(true)
                .set_type(dpp::cot_text)
                .set_placeholder("https://legendary.gl/epiclogin")
                .set_min_length(32)
                .set_max_length(32)
                .set_text_style(dpp::text_short)
        );
        event.dialog(modal);
    }

    void verifySteam(dpp::cluster& client, const dpp::slashcommand_t& event)
    {
        const dpp::user& issuer = event.command.get_issuing_user();

        if (event.command.channel_id != Constants::VerifyChanId)
        {
            std::string failReply = std::format(Constants::VerifyWrongChannelTPL, dpp::channel::get_mention(Constants::VerifyChanId));
            event.reply(dpp::message(failReply));
            return;
        }

        cpr::Response response = cpr::Get(
            cpr::Url(std::format("https://discordapp.com/api/v6/users/{}/profile", (uint64_t)issuer.id)),
            cpr::Header{{"Authorization", BotConfig::instance().userToken()}}
        );

        if (response.status_code >= 400)
        {
            BotUtil::shitCodeException(event, std::format(Constants::VerifyDAPIFailTPL, response.status_code, response.status_line));
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
                BotUtil::shitCodeException(event, std::format(Constants::VerifySAPIFailTPL, response.status_code, response.status_line));
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
            event.reply(dpp::message(Constants::VerifyNoBTD6SteamTPL));
            break;
        case VerifyState::NoSteamConnected:
            event.reply(dpp::message(Constants::VerifyNoSteamTPL));
            break;
        case VerifyState::Success:
            event.reply(dpp::message("You were verified successfully!"));
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            client.guild_member_add_role(event.command.guild_id, issuer.id, Constants::VerifiedSteamRoleId);
            BotUtil::getAndDeleteBulk(client, event.command.channel_id, 0, 0, Constants::VerifyInstructMsgId);
            break;
        }
    }
}
