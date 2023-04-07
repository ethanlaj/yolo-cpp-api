#include "utils.h"
#include <fstream>

std::vector<std::string> readClassNames(const std::string& classNamesPath) {
    std::vector<std::string> classNames;
    std::ifstream classNamesFile(classNamesPath);
    if (classNamesFile.is_open()) {
        std::string className;
        while (std::getline(classNamesFile, className)) {
            classNames.push_back(className);
        }
        classNamesFile.close();
    }
    return classNames;
}
