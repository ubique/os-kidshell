#include <fstream>

int main(int argc, char** argv) {
    std::ofstream out("output.txt");
    if (argc > 1) {
        out << argv[1] << std::endl;
    }
    out << "Hello world!" << std::endl;
    return 0;
}