#include <cstdint>
#include <filesystem>
#include <iostream>

int main(void)
{
    const char *path = "/nfs/sgoinfre/goinfre/Perso/isalhi/WebServ/www/test1";
    int n = std::filesystem::remove_all(path);
    std::cout << n << std::endl;
}