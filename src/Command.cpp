#include <algorithm>
#include <memory>

#include "Command.h"

std::vector<std::string> Command::getArguments() {
    return args;
}

std::vector<std::string> Command::getEnvironment() {
    return env;
}

std::ostream& operator<<(std::ostream& os, const Command& cmd) {
    os << "(";
    std::for_each(cmd.env.begin(), cmd.env.end(), 
            [&os](const std::string& s){ os << s << ", "; });
    os << ")";
        
    os << " (";
    std::for_each(cmd.args.begin(), cmd.args.end(), 
            [&os](const std::string& s){ os << s << ", "; });
    os << ")" << std::endl;

    return os;
}

int Command::parse(std::string line) {
    line += ' '; // Let the state machine finish

    enum state {
        STATE_START = 0,
        STATE_READING,
        STATE_READING_ESC,
        STATE_READING_QUOTE,
        STATE_READING_QUOTE_ESC
    };
    
    state s = STATE_START;
    std::string cmd;

    bool had_command = false;
    bool is_env_var = false;

    for (char c : line) {
restart:
        switch(s) {
            case STATE_START:
                cmd.clear();
                is_env_var = false;

                if (c != ' ') {
                    s = STATE_READING;
                    goto restart;
                }
                break;
            case STATE_READING:
                if (c == ' ') {
                    if (!is_env_var || had_command) {
                        had_command = true;
                        args.push_back(cmd);
                    } else {
                        env.push_back(cmd);
                    }
                    s = STATE_START;
                } else if (c == '\\') {
                    s = STATE_READING_ESC;
                } else if (c =='"') {
                    s = STATE_READING_QUOTE;
                } else {
                    if (c == '=') {
                        is_env_var = true;
                    }
                    cmd.push_back(c);
                }
                break;
            case STATE_READING_ESC:
                cmd.push_back(c);
                s = STATE_READING;
                break;
            case STATE_READING_QUOTE:
                if (c == '\\') {
                    s = STATE_READING_QUOTE_ESC;
                } else if (c == '"') {
                    s = STATE_READING;
                } else {
                    if (c == '=') {
                        is_env_var = true;
                    }
                    cmd.push_back(c);
                }
                break;
            case STATE_READING_QUOTE_ESC:
                cmd.push_back(c);
                s = STATE_READING_QUOTE;
                break;
        }
    }

    if (s != STATE_START || args.empty()) {
        return args.empty() ? -1 : -2;
    }
    return 0;
}

