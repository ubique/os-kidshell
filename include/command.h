#ifndef H_COMMAND
#define H_COMMAND

#include <string>
#include <vector>

namespace cli {

struct command
{

  std::string executable;

  std::vector<std::string> arguments;

};

} // namespace cli

#endif // H_COMMAND
