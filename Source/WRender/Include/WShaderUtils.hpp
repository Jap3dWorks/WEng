#pragma once

#include "WCore/WStringUtils.hpp"

#include <cstdint>
#include <vector>
#include <string>
#include <regex>
#include <fstream>

namespace WShaderUtils {

    inline std::vector<char> ReadShader(const std::string & in_shader_path) {
        // std::string systempath = WStringUtils::SystemPath(in_shader_path);

        std::string shadercompiled;
        std::string extension;

        std::regex extension_pattern("(.*)(\\.[^\\.]+)$");
        std::smatch extension_match;

        if (!std::regex_search(
                in_shader_path,
                extension_match,
                extension_pattern
                ))
        {
            return {};
        }

        shadercompiled = extension_match[1].str() + ".spv";
        extension = extension_match[2].str();

        if (extension == ".glsl") {

            std::string stage_flag="vert";
            
            if (in_shader_path.contains(".frag.")) {
                stage_flag = "frag";
            }
            else if(in_shader_path.contains(".comp.")) {
                stage_flag = "comp";
            }

            std::string cmd = std::string("glslc ") +
                " -fshader-stage=" + stage_flag + " " +
                in_shader_path + " -o " + shadercompiled;
            
            if(system(cmd.c_str()) != 0) {
                throw std::runtime_error("FAIL while using glslc command!");
            }
        }
        else if (extension == ".hlsl") {
            // TODO: compile hlsl
        }
        else if (extension == ".slang") {
            // TODO: compile slang
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
