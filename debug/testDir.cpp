#include <dirent.h>
#include <iostream>

int main() {
  const char *directoryName = "/nfs/homes/belkarto/Desktop/"; // Current directory

  DIR *dir = opendir(directoryName);

  if (dir) {
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
      std::cout << entry->d_name;
      if (entry->d_type == DT_REG)
        std::cout << " regular";
      else if (entry->d_type == DT_DIR)
        std::cout << " Dir";
      else if (entry->d_type == DT_LNK)
        std::cout << " symbol_link";
      std::cout << std::endl;
      // << " " << entry->d_ino << " " << entry->d_off << std::endl;
    }

    closedir(dir);
  } else {
    std::cerr << "Error opening directory." << std::endl;
    return 1;
  }

  return 0;
}
