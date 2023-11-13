#include <iostream>
#include <sstream>

void    func1(void)
{

}

void    func2(void)
{   
    return (func1());
}

int main(void)
{
    func2();
    return 0;
}