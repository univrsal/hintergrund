#include <iostream>
#include "src/tagging/tagger.hpp"
#include <string>

int main(int argc, char* argv[])
{
    tagger t;
    std::string path;
    if (argc > 1) {
        path = argv[1];
    } else {
        std::cout << "Enter tagging path:\n";
        std::cin >> path;
    }
    std::cout << "Tagging " << path.c_str() << "\n";
    t.auto_tag(path.c_str(), false);
    return 0;
}
