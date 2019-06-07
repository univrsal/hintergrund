#include <iostream>
#include <string>
#include "src/util/config.hpp"
#include "src/util/argument_parser.hpp"

int main(int argc, const char* argv[])
{
    config::init_config();
    return arguments::parse(argc, argv);
}
