#include "botutil.h"
#include "constants.h"
#include <cpr/cpr.h>
#include <dpp/dpp.h>

namespace BotUtil
{
    void epicGamesVerify(dpp::cluster& client, const dpp::form_submit_t& event, const std::string& authCode)
    {
        cpr::Response oauthResponse = cpr::Post(
            cpr::Url("https://account-public-service-prod03.ol.epicgames.com/account/api/oauth/token"),
            cpr::Authentication("34a02cf8f4414e29b15921876da36f9a", "daafbccc737745039dffe53d94fc76cf", cpr::AuthMode::BASIC),
            cpr::Payload{{"grant_type", "authorization_code"}, {"code", authCode}, {"token_type", "eg1"}}
        );

        if (oauthResponse.status_code >= 500)
        {
            event.reply(dpp::message("Failed to start session, status code " + std::to_string(oauthResponse.status_code)));
            return;
        }

        nlohmann::json oauthJson = nlohmann::json::parse(oauthResponse.text);
        if (oauthJson.contains("errorCode"))
        {
            std::string errorCode = oauthJson["errorCode"].template get<std::string>();
            if (errorCode == "errors.com.epicgames.oauth.corrective_action_required")
            {
                std::string continuationUrl = oauthJson["continuationUrl"].template get<std::string>();
                std::string correctiveAction = oauthJson["correctiveAction"].template get<std::string>();
                std::string errorMessage = oauthJson["errorMessage"].template get<std::string>();

                dpp::message msg(std::format("{} {}, open the following URL to take action: {}", errorMessage, correctiveAction, continuationUrl));
                msg.flags |= dpp::m_ephemeral;

                event.reply(msg);
                return;
            }
            else
            {
                event.reply(dpp::message("Failed to start session, error code " + errorCode));
                return;
            }
        }
        else if (oauthResponse.status_code >= 400)
        {
            event.reply(dpp::message("Failed to start session, status code " + std::to_string(oauthResponse.status_code)));
            return;
        }

        std::string accountId = oauthJson["account_id"].template get<std::string>();
        std::string auth = "Bearer " + oauthJson["access_token"].template get<std::string>();

        cpr::Response entitlementsResponse = cpr::Get(
            cpr::Url("https://entitlement-public-service-prod08.ol.epicgames.com/entitlement/api/account/" + accountId + "/entitlements?start=0&count=5000"),
            cpr::Header{{"Authorization", auth}}
        );

        if (entitlementsResponse.status_code >= 500)
        {
            event.reply(dpp::message("Failed to get entitlements, status code " + std::to_string(oauthResponse.status_code)));
            return;
        }

        nlohmann::json entitlementsJson = nlohmann::json::parse(entitlementsResponse.text);
        if (entitlementsJson.contains("errorCode"))
        {
            std::string errorCode = entitlementsJson["errorCode"].template get<std::string>();
            event.reply(dpp::message("Failed to get entitlements, error code " + errorCode));
            return;
        }
        else if (entitlementsResponse.status_code >= 400)
        {
            event.reply(dpp::message("Failed to get entitlements, status code " + std::to_string(oauthResponse.status_code)));
            return;
        }

        bool ownsBtd6 = std::ranges::any_of(entitlementsJson, [](const nlohmann::basic_json<>& item) {
            return item["namespace"].template get<std::string>() == Constants::BTD6EpicId;
        });

        if (!ownsBtd6)
        {
            event.reply(dpp::message(Constants::VerifyNoBTD6EpicTPL));
            return;
        }

        event.reply(dpp::message("You were verified successfully!"));
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        client.guild_member_add_role(event.command.guild_id, event.command.get_issuing_user().id, Constants::VerifiedEpicRoleId);
        BotUtil::getAndDeleteBulk(client, event.command.channel_id, 0, 0, Constants::VerifyInstructMsgId);
    }

    void getAndDeleteBulk(dpp::cluster& client, dpp::snowflake channelId, dpp::snowflake around, dpp::snowflake before, dpp::snowflake after,
                          uint64_t limit)
    {
        client.messages_get(channelId, around, before, after, limit, [channelId, &client](const dpp::confirmation_callback_t& callback) {
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
            client.message_delete_bulk(messageIds, channelId);
        });
    }

    void shitCodeException(const dpp::slashcommand_t& event, const std::string& description)
    {
        dpp::embed embed = dpp::embed()
           .set_color(dpp::colors::red)
           .set_title("Shit Code Exception")
           .set_description(description);
        event.reply(dpp::message(event.command.channel_id, embed));
    }
}
