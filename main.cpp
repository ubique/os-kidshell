#include <iostream>
#include <sstream>
#include <memory>
#include <cerrno>
#include <cassert>
#include <cstring>

#include <unistd.h>
#include <wordexp.h>
#include <sys/wait.h>

namespace
{
	struct wordexp_guard
	{
		wordexp_guard(const char* words, const int flags)
		{
			retval_ = wordexp(words, &we_, 0);
		}

		~wordexp_guard()
		{
			if (retval_ == 0)
			{
				wordfree(&we_);
			}
		}

		const wordexp_t& get_wordexp() const
		{
			return we_;
		}

		int get_retval() const
		{
			return retval_;
		}

		wordexp_guard(const wordexp_guard& other) = delete;
		wordexp_guard& operator=(const wordexp_guard& other) = delete;

	private:
		int retval_;
		wordexp_t we_;
	};
}

int main()
{
	for (std::string line; std::cout << '>', std::getline(std::cin, line);)
	{
		auto we = wordexp_guard(line.c_str(), 0);
		if (const auto we_res = we.get_retval(); we_res != 0)
		{
			const char* err_msg;
			switch (we_res)
			{
			case WRDE_BADCHAR:
				err_msg = "Illegal occurence of a metacharacter";
				break;
			case WRDE_SYNTAX:
				err_msg = "Syntax error";
				break;
			case WRDE_NOSPACE:
				err_msg = "Out of memory";
				break;
			default:
				return EXIT_FAILURE;
			}
			std::cout << "Could not parse the input string: " << err_msg << std::endl;
			continue;
		}
		if (we.get_wordexp().we_wordc == 0)
		{
			continue;
		}

		const auto pid = fork();
		if (pid == -1)
		{
			std::cout << "Could not create a child process: " << std::strerror(errno) << std::endl;
			continue;
		}
		if (pid == 0)
		{
			extern char** environ;
			const auto path = getenv("PATH");
			auto path_stream = std::istringstream(path);
			auto had_eacces = false;
			for (std::string subpath; std::getline(path_stream, subpath, ':'); )
			{
				const auto file = subpath + '/' + we.get_wordexp().we_wordv[0];
				[[maybe_unused]] const auto exec_res = execve(file.c_str(), we.get_wordexp().we_wordv, environ);
				assert(exec_res == -1);
				had_eacces |= errno == EACCES;
				if (!(errno == EACCES || errno == ENOENT || errno == ESTALE || errno == ENOTDIR || errno == ENODEV || errno == ETIMEDOUT))
				{
					break;
				}
			}
			[[maybe_unused]] const auto exec_res = execve(we.get_wordexp().we_wordv[0], we.get_wordexp().we_wordv, environ);
			assert(exec_res == -1);
			const auto err = had_eacces ? EACCES : errno;
			std::cout << "Execution has failed: " << std::strerror(err) << std::endl;
			return EXIT_FAILURE;
		}
		int status;
		if (waitpid(pid, &status, 0) == -1)
		{
			std::cout << "Waiting for the child process has failed: " << std::strerror(errno) << std::endl;
			continue;
		}
		if (WIFEXITED(status))
		{
			std::cout << "The child process has finished with status code " << WEXITSTATUS(status) << std::endl;
		}
		else if (WIFSIGNALED(status))
		{
			std::cout << "The child process was killed by signal " << WTERMSIG(status) << std::endl;
		}
		else if (WIFSTOPPED(status))
		{
			std::cout << "The child process was stopped by signal " << WSTOPSIG(status) << std::endl;
		}
		else
		{
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}
