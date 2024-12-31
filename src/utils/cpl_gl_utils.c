#include "utils/cpl_gl_utils.h"

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