#ifndef CPLSHADERS_H
#define CPLSHADERS_H

static char* SHADER_VERTEX = 
"#version 330 core\n"
"layout(location = 0) in vec2 inPosition;\n"
"layout(location = 1) in vec3 inColor;\n"
"out vec3 vertColor;\n"
"uniform mat4 u_projection;\n"
"void main()\n"
"{\n"
"    gl_Position = u_projection * vec4(inPosition, 0.0, 1.0);\n"
"    vertColor = inColor;\n"
"}\n";

static char* SHADER_FRAGMENT = 
"#version 330 core\n"
"in vec3 vertColor;\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"    fragColor = vec4(vertColor, 1.0);\n"
"}\n";

#endif // CPLSHADERS_H