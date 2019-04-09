#include "Environment.h"


void Environment::set(std::string expr) {
    auto pos = expr.find('=');
    auto name = expr.substr(0, pos);
    auto value = expr.substr(pos + 1, expr.size());

    env[name] = value;
}

bool Environment::unset(std::string var) {
    auto it = env.find(var);

    if (it != env.end()) {
        env.erase(it);

        return true;
    }

    return false;
}

std::vector<std::string> Environment::augment(const std::vector<std::string>& add) {
    std::vector<std::string> ret;

    for (auto v : env) {
        ret.push_back(v.first + "=" + v.second);
    }

    for_each(add.begin(), add.end(), [&ret](std::string x){ ret.push_back(x); });

    return ret;
}

int Environment::run(Command cmd) {

    auto arguments = cmd.getArguments();

    if (arguments.empty()) {
        return 0;
    }

    if (arguments[0] == "export") {
        if (arguments.size() != 2) {
            std::cerr << "export expects argument" << std::endl;
            return 1;
        }

        if (arguments[1].find('=') == arguments[0].npos) {
            std::cerr << "Usage: export <VAR>=<VALUE>" << std::endl;
            return 1;
        }

        set(arguments[1]);

    } else if(arguments[0] == "unset") {
        if (arguments.size() != 2) {
            std::cerr << "unset expects 2 arguments" << std::endl;
            return 1;
        }

    unset(arguments[1]);

    } else {
        return run_exec(cmd); 
    }
    return 0;
}

int Environment::run_exec(Command cmd) {
    int cpid;    
    int ret, wstatus;

    cpid = fork();
    
    if (cpid < 0) {
        throw RunnerException(strerror(cpid));
    }
    
    if (cpid == 0) {
        auto arguments = ExecArgumentsHolder(cmd.getArguments());
        auto environment = ExecArgumentsHolder(augment(cmd.getEnvironment()));

        std::string image_name = cmd.getArguments()[0];
        
        int res2 = execve(image_name.c_str(), arguments.arguments, environment.arguments);

        if (res2 == -1) {
            cpid = 0;
            
            std::cerr << "Could not execute: " << strerror(errno) << std::endl;

            exit(1);
            /* In the child process, no point in throwing exceptions */
            /* IRL, we should have been doing IPC here, maybe */
        }
        
    }

    assert(cpid > 0);
    
    while ((ret = waitpid(cpid, &wstatus, 0)) == EINTR);

    if (ret < 0) {
        throw RunnerException(strerror(errno));
        /* Should never happen */
    }

    return WEXITSTATUS(wstatus);
}
