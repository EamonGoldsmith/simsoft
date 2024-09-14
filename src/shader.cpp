#include "shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

std::string load_from_file(const std::string& path)
{
    std::ifstream file(path);
    
    if (!file.is_open()) {
        std::cerr << "err: couldn't open file: " << path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf(); // read to buffer
    
    file.close();
    
    return buffer.str();
}
