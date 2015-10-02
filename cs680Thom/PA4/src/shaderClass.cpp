#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include "shaderClass.h"

bool shader_loader::readFile(const char* filename, std::string& outFile)
{
    std::ifstream fin(filename);
    bool check = false;
    
    if (fin.is_open())
        {
        std::string line;
        while (getline(fin, line))
        {
            outFile.append(line);
            outFile.append("\n");
        }

        fin.close();
        check = true;
    }
    else
    {
        fprintf(stderr, "Could not open shader file\n");
    }
    
    return check;
}
void shader_loader::add_shader(GLuint ShaderProgram, const char* filename, GLenum ShaderType)
{   
    // Read in from file
    std::string shaderText;
    if (!readFile(filename, shaderText))
    {
        exit(1);
    }

    // Create Shader
    GLuint ShaderObj = glCreateShader(ShaderType);

    // Error Checking
    if (ShaderObj == 0)
    {
        fprintf(stderr, "Could not create shader.\n");
        exit(0);
    }

    // Setup for Shader
    const GLchar* p[1];
    p[0] = shaderText.c_str();
    GLint Lengths[1];
    Lengths[0]= strlen(shaderText.c_str());
    glShaderSource(ShaderObj, 1, p, Lengths);
    glCompileShader(ShaderObj);

    // Error Checking
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Could not create Shader: '%s'\n", InfoLog);
        exit(1);
    }
    glAttachShader(ShaderProgram, ShaderObj);
}