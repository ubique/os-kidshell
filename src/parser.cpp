#include <iostream>

#include "parser.h"

namespace cli
{

const std::regex delim{"[\\s+\\t+]"};

std::vector<std::string> get_tokens(const std::string &line)
{
    return {std::sregex_token_iterator(line.begin(), line.end(), delim, -1),
            std::sregex_token_iterator()};
}

std::optional<command> parse_command_line(const std::string &s)
{
    PARSE_STATE parse_state = NEED_EXEC;
    std::optional<command> cmd;

    const auto tokens = get_tokens(s);

    for (auto &token : tokens)
    {
        switch (parse_state)
        {
        case NEED_ARG:
            cmd.value().arguments.push_back(token);
            break;
        case NEED_EXEC:
            if (tokens.size() == 1 && tokens[0] == "")
            {
                break;
            }
            cmd = command{};
            cmd.value().executable = token;
            cmd.value().arguments.push_back(token);
            parse_state = NEED_ARG;
            break;
        }
    }

    return cmd;
}

} // namespace cli
