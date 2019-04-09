#ifndef H_PARSER
#define H_PARSER

#include <optional>
#include <regex>

#include "command.h"

namespace cli
{

enum PARSE_STATE
{
    NEED_EXEC,
    NEED_ARG
};

std::vector<std::string> get_tokens(const std::string &line);

std::optional<command> parse_command_line(const std::string &s);

} // namespace cli

#endif // H_PARSER
