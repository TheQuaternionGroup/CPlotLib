#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "CPLPlot.h"
#include "CPLUtils.h"

//-------------------------------------
// Simple text placeholder
//-------------------------------------
static void RenderText2D(const char *text, float x, float y, float r, float g, float b)
{
    (void)text; (void)x; (void)y; (void)r; (void)g; (void)b;
}

//-------------------------------------
// Axis lines
//-------------------------------------
static void BuildAxisData(void)
{
    float axisData[4 * 5]; // 4 vertices => (x,y,r,g,b)

    // X-axis from (s_minX,0) -> (s_maxX,0)
    axisData[0] = s_minX;
    axisData[1] = 0.f;
    axisData[2] = 0.7f;
    axisData[3] = 0.7f;
    axisData[4] = 0.7f;

    axisData[5] = s_maxX;
    axisData[6] = 0.f;
    axisData[7] = 0.7f;
    axisData[8] = 0.7f;
    axisData[9] = 0.7f;

    // Y-axis from (0, s_minY) -> (0, s_maxY)
    axisData[10] = 0.f;
    axisData[11] = s_minY;
    axisData[12] = 0.7f;
    axisData[13] = 0.7f;
    axisData[14] = 0.7f;

    axisData[15] = 0.f;
    axisData[16] = s_maxY;
    axisData[17] = 0.7f;
    axisData[18] = 0.7f;
    axisData[19] = 0.7f;

    s_axisCount = 4; // 2 lines => 4 vertices

    if (s_axisVAO == 0)
        glGenVertexArrays(1, &s_axisVAO);
    if (s_axisVBO == 0)
        glGenBuffers(1, &s_axisVBO);

    glBindVertexArray(s_axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, s_axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisData), axisData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1); // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//-------------------------------------
// Ticks
//-------------------------------------
static void DrawAxisTicks(Figure *fig, const double* ticks, int numTicks, char axis)
{
    if(!fig || !ticks || numTicks<=0) 
        return;

    float domainSize = (axis=='x') ? (s_maxY - s_minY) : (s_maxX - s_minX);
    if(domainSize < 1e-14f) domainSize = 1.f;
    float lineLen = 0.02f * domainSize;

    // we store 2 vertices (x,y,r,g,b) per tick
    float* data = (float*)malloc(numTicks * 2 * 5 * sizeof(float));
    if(!data) return;

    int idx=0;
    for(int i=0; i<numTicks; i++)
    {
        float val = (float)ticks[i];
        if(axis=='x')
        {
            float x=val;
            float y1=-lineLen*0.5f;
            float y2= lineLen*0.5f;

            // vertex1
            data[idx++] = x;
            data[idx++] = y1;
            data[idx++] = 0.8f;
            data[idx++] = 0.8f;
            data[idx++] = 0.8f;
            // vertex2
            data[idx++] = x;
            data[idx++] = y2;
            data[idx++] = 0.8f;
            data[idx++] = 0.8f;
            data[idx++] = 0.8f;
        }
        else if(axis=='y')
        {
            float y=val;
            float x1=-lineLen*0.5f;
            float x2= lineLen*0.5f;

            // vertex1
            data[idx++] = x1;
            data[idx++] = y;
            data[idx++] = 0.8f;
            data[idx++] = 0.8f;
            data[idx++] = 0.8f;
            // vertex2
            data[idx++] = x2;
            data[idx++] = y;
            data[idx++] = 0.8f;
            data[idx++] = 0.8f;
            data[idx++] = 0.8f;
        }
    }

    // draw immediately
    GLuint tmpVAO=0, tmpVBO=0;
    glGenVertexArrays(1,&tmpVAO);
    glGenBuffers(1,&tmpVBO);

    glBindVertexArray(tmpVAO);
    glBindBuffer(GL_ARRAY_BUFFER,tmpVBO);
    glBufferData(GL_ARRAY_BUFFER, idx*sizeof(float), data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(2*sizeof(float)));

    glDrawArrays(GL_LINES,0,idx/5);

    glBindVertexArray(0);
    glDeleteBuffers(1,&tmpVBO);
    glDeleteVertexArrays(1,&tmpVAO);
    free(data);
}

//-------------------------------------
// Param Plot
//-------------------------------------
static void computeParamBounds(
    double (*fx)(double),
    double (*fy)(double),
    double tMin,
    double tMax,
    int numSamples,
    double* minX,
    double* maxX,
    double* minY,
    double* maxY
)
{
    *minX = 1e30; *maxX = -1e30;
    *minY = 1e30; *maxY = -1e30;
    double dt = (tMax - tMin)/(numSamples - 1);
    for(int i=0; i<numSamples; i++)
    {
        double t = tMin + i*dt;
        double x = fx(t);
        double y = fy(t);
        if(x < *minX) *minX = x;
        if(x > *maxX) *maxX = x;
        if(y < *minY) *minY = y;
        if(y > *maxY) *maxY = y;
    }
}

static void expandBoundingBox(double minX, double maxX, double minY, double maxY)
{
    double rangeX = (maxX - minX), rangeY = (maxY - minY);
    double marginX= fmax(0.1*rangeX, 0.1);
    double marginY= fmax(0.1*rangeY, 0.1);
    s_minX=(float)(minX - marginX);
    s_maxX=(float)(maxX + marginX);
    s_minY=(float)(minY - marginY);
    s_maxY=(float)(maxY + marginY);
}

static void generateTicksIfNeeded(Figure* fig)
{
    if(!fig->xTicks)
    {
        fig->xTicks = (double*)calloc(8,sizeof(double));
        fig->xTicksCount=7;
        double step=(s_maxX - s_minX)/6.0;
        for(int i=0;i<7;i++)
        {
            fig->xTicks[i]= s_minX + i*step;
        }
    }
    if(!fig->yTicks)
    {
        fig->yTicks = (double*)calloc(8,sizeof(double));
        fig->yTicksCount=7;
        double step=(s_maxY - s_minY)/6.0;
        for(int i=0;i<7;i++)
        {
            fig->yTicks[i]= s_minY + i*step;
        }
    }
}

static float* buildParam2DData(
    double (*fx)(double),
    double (*fy)(double),
    double tMin,
    double tMax,
    int numSamples,
    Color lineColor,
    ColorCallback colorFn,
    void* userData
)
{
    float* data = (float*)malloc(numSamples * 5 * sizeof(float));
    if(!data) return NULL;

    double dt = (tMax - tMin)/(numSamples-1);
    for(int i=0; i<numSamples; i++)
    {
        double t = tMin + i*dt;
        double x = fx(t);
        double y = fy(t);
        Color c = (colorFn) ? colorFn(t,userData) : lineColor;

        int idx = i*5;
        data[idx]   = (float)x;
        data[idx+1] = (float)y;
        data[idx+2] = c.r;
        data[idx+3] = c.g;
        data[idx+4] = c.b;
    }
    return data;
}

static void setupParam2DVAOandVBO(int numSamples, float* data)
{
    if(s_curveVAO==0) glGenVertexArrays(1,&s_curveVAO);
    if(s_curveVBO==0) glGenBuffers(1,&s_curveVBO);

    glBindVertexArray(s_curveVAO);
    glBindBuffer(GL_ARRAY_BUFFER, s_curveVBO);
    glBufferData(GL_ARRAY_BUFFER, numSamples*5*sizeof(float), data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(2*sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    s_curveCount=(GLsizei)numSamples;
}

void PlotParam2D(
    Figure* fig,
    double (*fx)(double),
    double (*fy)(double),
    double tMin,
    double tMax,
    int numSamples,
    Color lineColor,
    ColorCallback colorFn,
    void* userData
)
{
    if(!fig || !fx || !fy || (numSamples<2) || (tMax<=tMin))
        return;

    double minX, maxX, minY, maxY;
    computeParamBounds(fx, fy, tMin, tMax, numSamples, &minX, &maxX, &minY, &maxY);
    expandBoundingBox(minX, maxX, minY, maxY);
    generateTicksIfNeeded(fig);

    float* data = buildParam2DData(fx, fy, tMin, tMax, numSamples, lineColor, colorFn, userData);
    if(!data) return;

    setupParam2DVAOandVBO(numSamples, data);
    free(data);

    BuildAxisData();
    s_isDataLoaded=1;
}

//-------------------------------------
// PlotDraw / ShowPlot
//-------------------------------------

void PlotDraw(Figure *fig)
{
    if(!fig||!s_isDataLoaded) return;

    glUseProgram(fig->programID);

    // 1) aspect ratio fix
    int fbW, fbH;
    glfwGetFramebufferSize(fig->window,&fbW,&fbH);
    if(fbH<1) fbH=1;
    float windowAspect = (float)fbW/(float)fbH;

    float domainW=(s_maxX-s_minX);
    float domainH=(s_maxY-s_minY);
    if(domainH<1e-14f) domainH=1.f;

    float domainAspect= domainW/domainH;

    // letterbox or pillarbox
    float left=s_minX;
    float right=s_maxX;
    float bottom=s_minY;
    float top=s_maxY;

    if(domainAspect>windowAspect)
    {
        float newH= domainW/windowAspect;
        float cy=0.5f*(bottom+top);
        bottom= cy-0.5f*newH;
        top   = cy+0.5f*newH;
    }
    else
    {
        float newW= domainH*windowAspect;
        float cx= 0.5f*(left+right);
        left = cx-0.5f*newW;
        right= cx+0.5f*newW;
    }

    // Ortho
    float proj[16];
    makeOrthoMatrix(left,right,bottom,top,proj);
    glUniformMatrix4fv(fig->loc_u_projection,1,GL_FALSE,proj);

    // axis lines
    glBindVertexArray(s_axisVAO);
    glDrawArrays(GL_LINES,0,s_axisCount);

    // ticks
    DrawAxisTicks(fig,fig->xTicks,fig->xTicksCount,'x');
    DrawAxisTicks(fig,fig->yTicks,fig->yTicksCount,'y');

    // curve
    glBindVertexArray(s_curveVAO);
    glDrawArrays(GL_LINE_STRIP,0,s_curveCount);

    glBindVertexArray(0);
    glUseProgram(0);
}

void ShowPlot(Figure *fig)
{
    if(!fig) return;

    while(FigureIsOpen(fig))
    {
        FigureClear(fig, COLOR_BLACK);
        PlotDraw(fig);
        FigureSwapBuffers(fig);
    }

    glDeleteVertexArrays(1,&s_curveVAO);
    glDeleteBuffers(1,&s_curveVBO);
    glDeleteVertexArrays(1,&s_axisVAO);
    glDeleteBuffers(1,&s_axisVBO);

    FreeFigure(fig);

    s_curveVAO=0; s_curveVBO=0;
    s_axisVAO=0; s_axisVBO=0;
    s_curveCount=0; s_axisCount=0;
    s_isDataLoaded=0;
}
