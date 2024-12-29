#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CPLFigure.h"
#include "CPLUtils.h" // For ReadFile, CompileShader, LinkProgram, etc.

#define SHADER_VERTEX_PATH   "shaders/vertex.glsl"
#define SHADER_FRAGMENT_PATH "shaders/fragment.glsl"

Figure* CreateFigure(int width, int height, const char* title)
{
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to init GLFW.\n");
        return NULL;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Figure* fig = (Figure*)calloc(1, sizeof(Figure));
    if (!fig)
    {
        fprintf(stderr,"Failed to allocate Figure.\n");
        glfwTerminate();
        return NULL;
    }

    fig->window = glfwCreateWindow(width, height, title, NULL, NULL);
    if(!fig->window)
    {
        fprintf(stderr,"Failed to create GLFW window.\n");
        free(fig);
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(fig->window);

    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
    {
        fprintf(stderr,"Failed to init GLEW.\n");
        glfwDestroyWindow(fig->window);
        free(fig);
        glfwTerminate();
        return NULL;
    }
    glViewport(0,0,width,height);

    // Compile default line-drawing shaders 
    char* vsrc = ReadFile(SHADER_VERTEX_PATH);
    char* fsrc = ReadFile(SHADER_FRAGMENT_PATH);
    if(!vsrc || !fsrc)
    {
        fprintf(stderr,"Failed to load shader files.\n");
        if(vsrc) free(vsrc);
        if(fsrc) free(fsrc);
        glfwDestroyWindow(fig->window);
        free(fig);
        glfwTerminate();
        return NULL;
    }
    GLuint vs = CompileShader(vsrc, GL_VERTEX_SHADER, 0);
    GLuint fs = CompileShader(fsrc, GL_FRAGMENT_SHADER, 0);
    free(vsrc);
    free(fsrc);

    fig->programID = LinkProgram(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    fig->loc_u_projection = glGetUniformLocation(fig->programID, "u_projection");
    if(fig->loc_u_projection < 0)
    {
        fprintf(stderr, "Warning: uniform 'u_projection' not found.\n");
    }

    // Enable alpha blending if needed
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    fig->width = width;
    fig->height= height;

    // default axis labels
    strncpy(fig->xLabel, "X-Axis", sizeof(fig->xLabel)-1);
    fig->xLabel[sizeof(fig->xLabel)-1] = '\0';
    strncpy(fig->yLabel, "Y-Axis", sizeof(fig->yLabel)-1);
    fig->yLabel[sizeof(fig->yLabel)-1] = '\0';

    return fig;
}

int FigureIsOpen(Figure* fig)
{
    if(!fig) return 0;
    return !glfwWindowShouldClose(fig->window);
}

void FigureClear(Figure* fig, Color bg)
{
    if(!fig) return;

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(fig->window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    glClearColor(bg.r, bg.g, bg.b, bg.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void FigureSwapBuffers(Figure* fig)
{
    if(!fig) return;
    glfwSwapBuffers(fig->window);
    glfwPollEvents();
}

void FreeFigure(Figure* fig)
{
    if(!fig) return;

    if(fig->xTicks) { free(fig->xTicks); fig->xTicks=NULL; }
    if(fig->yTicks) { free(fig->yTicks); fig->yTicks=NULL; }
    if(fig->zTicks) { free(fig->zTicks); fig->zTicks=NULL; }

    if(fig->programID) glDeleteProgram(fig->programID);
    if(fig->window)    glfwDestroyWindow(fig->window);

    glfwTerminate();
    free(fig);
}

void SetAxisLabels(Figure* fig, const char* xLabel, const char* yLabel)
{
    if(!fig) return;
    if(xLabel)
    {
        strncpy(fig->xLabel, xLabel, sizeof(fig->xLabel)-1);
        fig->xLabel[sizeof(fig->xLabel)-1] = '\0';
    }
    if(yLabel)
    {
        strncpy(fig->yLabel, yLabel, sizeof(fig->yLabel)-1);
        fig->yLabel[sizeof(fig->yLabel)-1] = '\0';
    }
}

void SetXaxis(Figure* fig, double *xTicks, int numTicks)
{
    if(!fig || !xTicks || numTicks<=0) return;

    if(fig->xTicks) { free(fig->xTicks); fig->xTicks=NULL; }

    fig->xTicks = (double*)malloc(numTicks*sizeof(double));
    if(!fig->xTicks) return;
    memcpy(fig->xTicks, xTicks, numTicks*sizeof(double));
    fig->xTicksCount = numTicks;

    // We won't assume sorted or that [0..count-1] is min..max
    // We just store the user data. The next code can interpret it as needed.
}

void SetYaxis(Figure* fig, double *yTicks, int numTicks)
{
    if(!fig || !yTicks || numTicks<=0) return;

    if(fig->yTicks) { free(fig->yTicks); fig->yTicks=NULL; }

    fig->yTicks = (double*)malloc(numTicks*sizeof(double));
    if(!fig->yTicks) return;
    memcpy(fig->yTicks, yTicks, numTicks*sizeof(double));
    fig->yTicksCount = numTicks;
}
