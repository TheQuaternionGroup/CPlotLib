#include "C_headers/utils/cpl_gl_utils.h"

#include <stdio.h>
#include <math.h>

void ShaderCheckError(GLuint object, GLenum statusType, const char* label,
                             void (*getiv)(GLuint, GLenum, GLint*),
                             void (*getInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*))
{
    GLint status;
    getiv(object, statusType, &status);
    if(status == GL_FALSE) {
        char buffer[BUFFER_SIZE];
        getInfoLog(object, BUFFER_SIZE, NULL, buffer);
        fprintf(stderr, "Error in %s:\n%s\n", label, buffer);
    }
}

void SetUpShaders(GLuint* programID, GLuint* vbo, GLuint* vao, GLuint* vs, GLuint* fs)
{
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);

    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    *vs = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* vshader_src[] = { SHADER_VERTEX };
    glShaderSource(*vs, 1, vshader_src, NULL);
    glCompileShader(*vs);
    ShaderCheckError(*vs, GL_COMPILE_STATUS, "Vertex Shader", glGetShaderiv, glGetShaderInfoLog);

    *fs = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fshader_src[] = { SHADER_FRAGMENT };
    glShaderSource(*fs, 1, fshader_src, NULL);
    glCompileShader(*fs);
    ShaderCheckError(*fs, GL_COMPILE_STATUS, "Fragment Shader", glGetShaderiv, glGetShaderInfoLog);

    *programID = glCreateProgram();
    glAttachShader(*programID, *vs);
    glAttachShader(*programID, *fs);
    glLinkProgram(*programID);
    ShaderCheckError(*programID, GL_LINK_STATUS, "Shader Program", glGetProgramiv, glGetProgramInfoLog);

    glDeleteShader(*vs);
    glDeleteShader(*fs);
    // glUseProgram(*programID);
}

void makeOrthoMatrix(float left, float right, float bottom, float top, float* out)
{
    float zNear=-1.f, zFar=1.f;

    if(fabsf(right-left)<1e-14f)  right=left+1.f;
    if(fabsf(top-bottom)<1e-14f)  top=bottom+1.f;

    out[0] =  2.f/(right-left);
    out[1] =  0;
    out[2] =  0;
    out[3] =  0;

    out[4] =  0;
    out[5] =  2.f/(top-bottom);
    out[6] =  0;
    out[7] =  0;

    out[8] =  0;
    out[9] =  0;
    out[10]= -2.f/(zFar-zNear);
    out[11]= 0;

    out[12]= -(right+left)/(right-left);
    out[13]= -(top+bottom)/(top-bottom);
    out[14]= -(zFar+zNear)/(zFar-zNear);
    out[15]= 1.f;
}