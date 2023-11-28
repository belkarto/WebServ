#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>

int main()
{
    // Get the current time
    std::time_t currentTime = std::time(NULL);
    std::tm    *localTime = std::localtime(&currentTime);

    // Format the current time to a string with the desired format
    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(4) << 1900 + localTime->tm_year; // Year
    ss << std::setw(2) << 1 + localTime->tm_mon;                          // Month
    ss << std::setw(2) << localTime->tm_mday;                             // Day
    ss << std::setw(2) << localTime->tm_hour;                             // Hour
    ss << std::setw(2) << localTime->tm_min;                              // Minute
    ss << std::setw(2) << localTime->tm_sec;                              // Second

    // Ask the user for the file extension
    std::string extension;
    std::cout << "Enter the file extension: ";
    std::cin >> extension;

    // Create the filename using the current date and time
    std::string filename = ss.str() + "." + extension;

    // Create and open the file
    std::ofstream file(filename.c_str()); // c_str() is used to convert the filename to a C-style string

    // Check if the file is successfully opened
    if (file.is_open())
    {
        std::cout << "File '" << filename << "' created successfully." << std::endl;
        // You can now use the 'file' object to write data to the file if needed.
    }
    else
    {
        std::cerr << "Error creating file." << std::endl;
    }

    return 0;
}
