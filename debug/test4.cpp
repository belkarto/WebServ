#include <iostream>
#include <sstream>

int main(void)
{
    char buffer[8] = "ilham";
    buffer[3] = '\0';

    std::stringstream ss;

    ss << buffer;
    std::cout << ss.str() << std::endl;

    const char *str = "ilham";
    const char *str2 = str + 2;
    int dist = str2 - str;
    std::cout << dist << std::endl;
}