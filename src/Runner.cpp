#include "Runner.h"
#include "ExecArgumentsHolder.hpp"
#include "RunnerException.hpp"

#include <cassert>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/types.h>


Runner::Runner(Command cmd) : exited(false) {
    cpid = fork();    
    
    if (cpid < 0) {
        throw RunnerException(strerror(cpid));
    }
    
    if (cpid > 0) {
        return;
    }

    auto arguments = ExecArgumentsHolder(cmd.getArguments());
    auto environment = ExecArgumentsHolder(cmd.getEnvironment());

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

int Runner::wait() {
    int wstatus;
    
    assert(cpid > 0);
    
    int ret = waitpid(cpid, &wstatus, 0);

    if (ret < 0) {
        throw RunnerException(strerror(errno));
    }

    exited = true;

    return WEXITSTATUS(wstatus);
}

Runner::~Runner() {
    if (cpid != 0 && !exited) {
        try {
            wait();
        } catch(const RunnerException& e) {
            /* What can we do about that? */
            /* Actually should never be reached */
            std::cerr << "waitpid() failed: " << strerror(errno) << std::endl;
        }
    }
}

