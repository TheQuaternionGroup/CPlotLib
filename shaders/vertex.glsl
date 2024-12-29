#version 330 core

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

out vec3 vertColor;

uniform mat4 u_projection; // orthographic transform for 2D

void main()
{
    // transform from world coords -> normalized device coords
    gl_Position = u_projection * vec4(inPosition, 0.0, 1.0);
    vertColor   = inColor;
}