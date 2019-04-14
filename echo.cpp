#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    for (size_t i = 1; i < argc; ++i)
    {
        std::cout << argv[i] << ' ';
    }
    std::cout << '\n';
    return 1;
}
