#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class CShader
{
public:
    uint32_t ID;
    
    CShader(const char* VertexPath, const char* FragmentPath)
    {
        std::string VertexCodeString;
        std::string FragmentCodeString;
        std::ifstream VertexShaderFile;
        std::ifstream FragmentShaderFile;

        VertexShaderFile.exceptions(
            std::ifstream::failbit | std::ifstream::badbit
        );
        FragmentShaderFile.exceptions(
            std::ifstream::failbit | std::ifstream::badbit
        );

        try
        {
            VertexShaderFile.open(VertexPath);
            FragmentShaderFile.open(FragmentPath);
            std::stringstream VertexShaderStream, FragmentShaderStream;

            VertexShaderStream << VertexShaderFile.rdbuf();
            FragmentShaderStream << FragmentShaderFile.rdbuf();

            VertexShaderFile.close();
            FragmentShaderFile.close();

            VertexCodeString = VertexShaderStream.str();
            FragmentCodeString = FragmentShaderStream.str();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        const char* VertexCode = VertexCodeString.c_str();
        const char* FragmentCode = FragmentCodeString.c_str();

        uint32_t Vertex, Fragment;

        // Vertex Shader
        Vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(Vertex, 1, &VertexCode, NULL);
        glCompileShader(Vertex);
        CheckCompileErrors(Vertex, "VERTEX");

        // Fragment Shader
        Fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(Fragment, 1, &FragmentCode, NULL);
        glCompileShader(Fragment);
        CheckCompileErrors(Fragment, "FRAGMENT");

        // Shader Program
        ID = glCreateProgram();
        glAttachShader(ID, Vertex);
        glAttachShader(ID, Fragment);
        glLinkProgram(ID);
        CheckCompileErrors(ID, "PROGRAM");
        glDeleteShader(Vertex);
        glDeleteShader(Fragment);
    }

    void Use()
    {
        glUseProgram(ID);
    }
    // Utility Uniform Functions
    // -------------------------
    void SetBool(const std::string& Name, bool Value) const
    {
        glUniform1i(glGetUniformLocation(ID, Name.c_str()), (int)Value);
    }
    // --------------------------
    void SetInt(const std::string& Name, int Value) const
    {
        glUniform1i(glGetUniformLocation(ID, Name.c_str()), Value);
    }
    // --------------------------
    void SetFloat(const std::string& Name, float Value) const
    {
        glUniform1f(glGetUniformLocation(ID, Name.c_str()), Value);
    }
    // --------------------------
    void SetVec2(const std::string& Name, const glm::vec2& Value) const
    {
        glUniform2fv(glGetUniformLocation(ID, Name.c_str()), 1, &Value[0]);
    }
    void SetVec2(const std::string& Name, float X, float Y) const
    {
        glUniform2f(glGetUniformLocation(ID, Name.c_str()), X, Y);
    }
    // --------------------------
    void SetVec3(const std::string& Name, const glm::vec3& Value) const
    {
        glUniform3fv(glGetUniformLocation(ID, Name.c_str()), 1, &Value[0]);
    }
    void SetVec3(const std::string& Name, float X, float Y, float Z) const
    {
        glUniform3f(glGetUniformLocation(ID, Name.c_str()), X, Y, Z);
    }
    // --------------------------
    void SetVec4(const std::string& Name, const glm::vec4& Value) const
    {
        glUniform4fv(glGetUniformLocation(ID, Name.c_str()), 1, &Value[0]);
    }
    void SetVec4(const std::string& Name, float X, float Y, float Z, float W) const
    {
        glUniform4f(glGetUniformLocation(ID, Name.c_str()), X, Y, Z, W);
    }
    // --------------------------
    void SetMat2(const std::string& Name, const glm::mat2& Mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, Name.c_str()), 1, GL_FALSE, &Mat[0][0]);
    }
    // --------------------------
    void SetMat3(const std::string& Name, const glm::mat3& Mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, Name.c_str()), 1, GL_FALSE, &Mat[0][0]);
    }
    // --------------------------
    void SetMat4(const std::string& Name, const glm::mat4& Mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, Name.c_str()), 1, GL_FALSE, &Mat[0][0]);
    }

private:
    void CheckCompileErrors(GLuint Shader, std::string Type)
    {
        GLint Success;
        GLchar InfoLog[1024];

        if(Type != "PROGRAM")
        {
            glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);
            if(!Success)
            {
                glGetShaderInfoLog(Shader, 1024, NULL, InfoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << Type << "\n" << InfoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(Shader, GL_LINK_STATUS, &Success);
            if(!Success)
            {
                glGetProgramInfoLog(Shader, 1024, NULL, InfoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << Type << "\n" << InfoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
