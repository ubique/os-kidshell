#include <fstream>

int main(int argc, char** argv) {
    std::ofstream out("output.txt");
    out << "Hello world!" << std::endl;
    return 0;
}