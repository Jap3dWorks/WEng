#pragma once

#include "WCore/WStringUtils.hpp"

#include <cstdint>
#include <vector>
#include <string>
#include <regex>
#include <fstream>

namespace WShaderUtils {

    inline std::vector<char> ReadShader(const std::string & in_shader_path) {
        std::string systempath = WStringUtils::SystemPath(in_shader_path);

        std::string shadercompiled;

        std::regex extension_pattern("(.*)(\\.[^\\.]+)$");
        std::smatch extension_match;

        if (!std::regex_search(
                systempath,
                extension_match,
                extension_pattern
                ))
        {
            return {};
        }

        shadercompiled = extension_match[0].str() + ".spv";

        if (extension_match[1].str() == ".glsl" || extension_match[1].str() == ".hlsl") {
            std::string cmd = std::string("glslc ") + systempath + ".spv" + " -o " + shadercompiled;
            
            if(system(cmd.c_str()) != 0) {
                throw std::runtime_error("FAIL while using glslc command!");
            }
        }
        else if (extension_match[1].str() == ".slang") {
            // compile slang
        }
        
        // std::ios::ate -> at the end of the file
        // std::ios::binary -> as binary avoid text transformations
        std::ifstream  file(shadercompiled, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("FAIL while reading shader file!");
        }

        size_t file_size = (size_t) file.tellg();

        std::vector<char> result(file_size);
        // result.code.resize(file_size);

        file.seekg(0);

        file.read(result.data(), file_size);

        file.close();

        return result;
    }
}
