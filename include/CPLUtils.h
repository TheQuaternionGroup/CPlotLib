#ifndef CPL_UTILS_H
#define CPL_UTILS_H

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define BUFFER_SIZE 512

//-------------------------------------
// Internal data
//-------------------------------------

static GLuint s_curveVAO    = 0;
static GLuint s_curveVBO    = 0;
static GLsizei s_curveCount = 0;

static GLuint s_axisVAO   = 0;
static GLuint s_axisVBO   = 0;
static GLsizei s_axisCount = 0;

// Auto-detected domain
static float s_minX = -1.f, s_maxX = 1.f;
static float s_minY = -1.f, s_maxY = 1.f;

static int   s_isDataLoaded = 0;


//------------------------------------------------------------------------------
// Helper funcs for compiling/linking
//------------------------------------------------------------------------------
// You can load GLSL from files. For simplicity, we read them as text here:
static char* ReadFile(const char* filename)
{
    FILE* f = fopen(filename, "rb");
    if(!f) return NULL;
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);
    char* buffer = (char*)malloc(len+1);
    if(!buffer)
    {
        fclose(f);
        return NULL;
    }
    fread(buffer, 1, len, f);
    buffer[len] = '\0';
    fclose(f);
    return buffer;
}

static void CheckError(GLuint object, GLenum statusType, const char* label, void (*getiv)(GLuint, GLenum, GLint*), void (*getInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*))
{
    GLint status;
    getiv(object, statusType, &status);
    if(status == GL_FALSE)
    {
        char buffer[BUFFER_SIZE];
        getInfoLog(object, BUFFER_SIZE, NULL, buffer);
        fprintf(stderr, "Error in %s:\n%s\n", label, buffer);
    }
}

static GLuint CompileShader(const char* src, GLenum type, int isPath)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    CheckError(shader, GL_COMPILE_STATUS, (type == GL_VERTEX_SHADER) ? "Vertex Shader" : "Fragment Shader", glGetShaderiv, glGetShaderInfoLog);

    return shader;
}

static GLuint LinkProgram(GLuint vs, GLuint fs)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    CheckError(program, GL_LINK_STATUS, "Shader Program", glGetProgramiv, glGetProgramInfoLog);
    return program;
}

static void makeOrthoMatrix(float left, float right, float bottom, float top, float* out)
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

static void generateVAOandVBO(GLuint* vao, GLuint* vbo)
{
    if(*vao==0) glGenVertexArrays(1, vao);
    if(*vbo==0) glGenBuffers(1, vbo);
}

#endif // CPL_UTILS_H