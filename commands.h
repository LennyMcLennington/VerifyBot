#ifndef COMMANDS_H
#define COMMANDS_H
#include <dpp/commandhandler.h>

namespace Commands
{
    void makeFaq(dpp::commandhandler& handler, const std::string& command, const dpp::parameter_list_t& params, const dpp::command_source& src);
    void verify(dpp::commandhandler& handler, const std::string& command, const dpp::parameter_list_t& params, const dpp::command_source& src);
}

#endif // COMMANDS_H
