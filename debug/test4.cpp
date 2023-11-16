#include <iostream>
#include <sstream>
#include <algorithm>

int	not_space(int c)
{
	return !isspace(c);
}

void	trim(std::string &str)
{
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), not_space));
	str.erase(std::find_if(str.rbegin(), str.rend(), not_space).base(), str.end());
}

int main(void)
{
    std::string str = "";
    trim(str);
    std::cout << "|" << str << "|" << std::endl;
}