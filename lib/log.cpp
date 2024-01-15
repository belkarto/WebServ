#include "Multiplexer.hpp"

void log(const std::string method, const std::string s)
{
    // get time and date
    std::time_t now = time(NULL);
    std::tm    *tm = localtime(&now);
    if (method == "POST")
        std::cout << YELLOW;
    else if (method == "GET")
        std::cout << GREEN;
    else if (method == "DELETE")
        std::cout << BLUE;
    else
        std::cout << RED;
    std::cout << std::setfill('0') << std::setw(4) << tm->tm_year + 1900 << "-" << std::setw(2) << tm->tm_mon + 1 << "-"
              << std::setw(2) << tm->tm_mday << " ";
    std::cout << "[" << std::setw(2) << tm->tm_hour << ":" << std::setw(2) << tm->tm_min << ":" << std::setw(2)
              << tm->tm_sec << "] ==> " << s << RESET << std::endl;
}