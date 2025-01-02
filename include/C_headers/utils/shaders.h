#ifndef CPL_SHADERS_H
#define CPL_SHADERS_H

static const char* SHADER_VERTEX = 
"#version 330 core\n"
"layout(location = 0) in vec2 inPosition;\n"
"layout(location = 1) in vec3 inColor;\n"
"out vec3 vertColor;\n"
"uniform mat4 proj_mat;\n"
"void main()\n"
"{\n"
"    gl_Position = proj_mat * vec4(inPosition, 0.0, 1.0);\n"
"    vertColor = inColor;\n"
"}\n";

static const char* SHADER_FRAGMENT = 
"#version 330 core\n"
"in vec3 vertColor;\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"    fragColor = vec4(vertColor, 1.0);\n"
"}\n";
#endif // CPL_SHADERS_H