#include "botcommandhandler.h"

namespace BotCommandHandler
{
    void addCommand(dpp::commandhandler& handler, const std::string& command, commandFunc func, const dpp::parameter_registration_t& params,
                    const std::string& description)
    {
        handler.add_command(command, params, [func, &handler](const std::string& command, const dpp::parameter_list_t& params, dpp::command_source src) {
            func(handler, command, params, src);
        });
    }
}
