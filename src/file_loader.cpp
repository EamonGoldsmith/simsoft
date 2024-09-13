#include "file_loader.h"

#include <iostream>
#include <fstream>
#include <sstream>

std::string loadShaderCodeFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    
    // Check if the file opened successfully
    if (!file.is_open()) {
        std::cerr << "Error: Could not open shader file: " << filePath << std::endl;
        return "";
    }

    // Read the file content into a stringstream
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    // Close the file
    file.close();
    
    // Return the shader code as a string
    return buffer.str();
}
