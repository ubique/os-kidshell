//
// Created by jetbrains on 29.03.19.
//

#include "Executor.h"

#include <cassert>

void init();

void test_empty();

void test_simple();

void test_man();

void test_pipe();

void test_error();

void test_exit();

void test_cd();

int main() {
    init();

    test_empty();

    test_simple();

    test_man();

    test_pipe();

    test_error();

    test_exit();

    test_cd();
}

void init() {
    os::Logger::state = os::Logger::Level::TRACE;
}

os::Executor executor;

void try_execute(std::string_view s, int expected) {
    int actual = executor.execute(s);
    if (actual != expected) {
        fprintf(stderr, "ASSERT %.*s returns %d instead of %d\n", static_cast<int>(s.size()), s.data(), actual, expected);
    }
}

void test_empty() {
    std::cerr << std::endl << "TEST_EMPTY" << std::endl;

    try_execute("", 0);
    try_execute("  ", 0);
}

void test_simple() {
    std::cerr << std::endl << "TEST_SIMPLE" << std::endl;

    try_execute("/bin/ls", 0);
}

void test_man() {
    std::cerr << std::endl << "TEST_MAN" << std::endl;

    try_execute("/usr/bin/man /bin/ls", 0);
}

void test_pipe() {
    std::cerr << std::endl << "TEST_PIPE" << std::endl;

    try_execute("/usr/bin/ls -l | /bin/more ", -1);
}

void test_error() {
    std::cerr << std::endl << "TEST_ERROR" << std::endl;

    try_execute("abacaba", -1);
}

void test_exit() {
    std::cerr << std::endl << "TEST_EXIT" << std::endl;

    try_execute("exit", +1);
}

void test_cd() {
    std::cerr << std::endl << "TEST_CD" << std::endl;

    try_execute("/bin/mkdir tmp", 0);
    try_execute("/bin/ls", 0);
    try_execute("cd tmp", 0);
}
