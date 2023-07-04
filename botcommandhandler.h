#ifndef BOTCOMMANDHANDLER_H
#define BOTCOMMANDHANDLER_H
#include <dpp/commandhandler.h>
#include <functional>

namespace BotCommandHandler
{
    using commandFunc = std::function<void(dpp::commandhandler&, const std::string&, const dpp::parameter_list_t&, const dpp::command_source&)>;
    void addCommand(dpp::commandhandler& handler, const std::string& command, commandFunc func, const dpp::parameter_registration_t& params = {},
                    const std::string& description = "");
}

#endif // BOTCOMMANDHANDLER_H
