#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <cstdlib>

class shader_loader{
public:

    bool readFile(const char* filename, std::string& outFile);
    void add_shader(GLuint ShaderProgram, const char* filename, GLenum ShaderType);
};

