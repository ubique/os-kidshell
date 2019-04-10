#include <iostream>
#include <cmath>

int main(int argc, char* args[]) {
  std::cout << "[test] args counter: " << argc << std::endl << "[test] ";
  for (int i = 0; (i + 1) < argc; i++) {
    std::cout << "\"" << args[i] << "\"," << ' ';
  }
  if (argc > 0) {
    std::cout << "\"" << args[argc - 1] << "\"";
  }
  std::cout << std::endl;
  return 0;
}