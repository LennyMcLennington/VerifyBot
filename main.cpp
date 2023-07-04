#include <fstream>
#include <dpp/dpp.h>
#include <iostream>
#include "botcommandhandler.h"
#include "botconfig.h"
#include "commands.h"

int main()
{
    BotConfig::instance().initialize();
    dpp::cluster client(BotConfig::instance().botToken(), dpp::i_default_intents | dpp::i_message_content);
    client.on_log(dpp::utility::cout_logger());

    dpp::commandhandler cmdHandler(&client);
    cmdHandler.add_prefix("$");

    client.on_ready([&client, &cmdHandler](const dpp::ready_t& event)
    {
        client.set_presence(dpp::presence(dpp::ps_online, dpp::at_game, "(〃´∀｀〃)ε｀●)chu♪"));
        BotCommandHandler::addCommand(cmdHandler, "mkfaq", Commands::makeFaq, { { "targetChannel", dpp::param_info(dpp::pt_channel, false, "") } });
        BotCommandHandler::addCommand(cmdHandler, "verify", Commands::verify);
    });

    client.start(dpp::st_wait);
}
