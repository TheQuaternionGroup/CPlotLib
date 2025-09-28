#include "CPLShader.h"
#include <stdio.h>
#include <stdlib.h>

// Optimized shader source code
const char* CPL_VERTEX_SHADER_SOURCE = 
"#version 330 core\n"
"layout(location = 0) in vec2 position;\n"
"layout(location = 1) in vec3 color;\n"
"out vec3 fragColor;\n"
"uniform mat4 proj_mat;\n"
"void main() {\n"
"    gl_Position = proj_mat * vec4(position, 0.0, 1.0);\n"
"    fragColor = color;\n"
"}\n";

const char* CPL_FRAGMENT_SHADER_SOURCE = 
"#version 330 core\n"
"in vec3 fragColor;\n"
"out vec4 color;\n"
"void main() {\n"
"    color = vec4(fragColor, 1.0);\n"
"}\n";

// Optimized shader sources for different rendering modes
const char* CPL_GRID_VERTEX_SHADER_SOURCE = 
"#version 330 core\n"
"layout(location = 0) in vec2 position;\n"
"layout(location = 1) in vec3 color;\n"
"out vec3 fragColor;\n"
"out vec2 screenPos;\n"
"uniform mat4 proj_mat;\n"
"uniform vec2 resolution;\n"
"void main() {\n"
"    gl_Position = proj_mat * vec4(position, 0.0, 1.0);\n"
"    fragColor = color;\n"
"    screenPos = gl_Position.xy * resolution * 0.5;\n"
"}\n";

const char* CPL_GRID_FRAGMENT_SHADER_SOURCE = 
"#version 330 core\n"
"in vec3 fragColor;\n"
"in vec2 screenPos;\n"
"out vec4 color;\n"
"uniform float time;\n"
"uniform float lineWidth;\n"
"uniform vec2 resolution;\n"
"void main() {\n"
"    // Calculate distance to grid lines\n"
"    vec2 gridPos = screenPos * 0.1;\n"
"    vec2 grid = abs(fract(gridPos) - 0.5);\n"
"    \n"
"    // Convert line width from pixels to screen space\n"
"    float pixelWidth = lineWidth / resolution.y;\n"
"    \n"
"    // Create anti-aliased lines with proper thickness\n"
"    float distToLine = min(grid.x, grid.y);\n"
"    float line = 1.0 - smoothstep(pixelWidth * 0.5, pixelWidth * 0.5 + 1.0, distToLine);\n"
"    \n"
"    // Subtle animation for grid lines\n"
"    float pulse = 0.8 + 0.2 * sin(time * 2.0);\n"
"    \n"
"    color = vec4(fragColor * pulse, line);\n"
"}\n";

const char* CPL_POINTS_VERTEX_SHADER_SOURCE = 
"#version 330 core\n"
"layout(location = 0) in vec2 position;\n"
"layout(location = 1) in vec3 color;\n"
"out vec3 fragColor;\n"
"out float pointSize;\n"
"uniform mat4 proj_mat;\n"
"uniform float time;\n"
"void main() {\n"
"    gl_Position = proj_mat * vec4(position, 0.0, 1.0);\n"
"    fragColor = color;\n"
"    \n"
"    // Dynamic point size with slight animation\n"
"    pointSize = 8.0 + 2.0 * sin(time + position.x * 10.0);\n"
"    gl_PointSize = pointSize;\n"
"}\n";

const char* CPL_POINTS_FRAGMENT_SHADER_SOURCE = 
"#version 330 core\n"
"in vec3 fragColor;\n"
"in float pointSize;\n"
"out vec4 color;\n"
"uniform float time;\n"
"void main() {\n"
"    // Create circular points with smooth edges\n"
"    vec2 center = gl_PointCoord - 0.5;\n"
"    float dist = length(center);\n"
"    \n"
"    // Smooth circular mask\n"
"    float alpha = 1.0 - smoothstep(0.4, 0.5, dist);\n"
"    \n"
"    // Add subtle glow effect\n"
"    float glow = 1.0 - smoothstep(0.0, 0.3, dist);\n"
"    \n"
"    color = vec4(fragColor + glow * 0.3, alpha);\n"
"}\n";

const char* CPL_FILLED_VERTEX_SHADER_SOURCE = 
"#version 330 core\n"
"layout(location = 0) in vec2 position;\n"
"layout(location = 1) in vec3 color;\n"
"out vec3 fragColor;\n"
"out vec2 worldPos;\n"
"uniform mat4 proj_mat;\n"
"void main() {\n"
"    gl_Position = proj_mat * vec4(position, 0.0, 1.0);\n"
"    fragColor = color;\n"
"    worldPos = position;\n"
"}\n";

const char* CPL_FILLED_FRAGMENT_SHADER_SOURCE = 
"#version 330 core\n"
"in vec3 fragColor;\n"
"in vec2 worldPos;\n"
"out vec4 color;\n"
"uniform float time;\n"
"void main() {\n"
"    // Add subtle gradient effect\n"
"    float gradient = 0.8 + 0.2 * sin(worldPos.x * 2.0 + worldPos.y * 2.0);\n"
"    \n"
"    // Add animated border effect\n"
"    float border = 1.0 - smoothstep(0.8, 1.0, abs(worldPos.x)) * \n"
"                   smoothstep(0.8, 1.0, abs(worldPos.y));\n"
"    \n"
"    color = vec4(fragColor * gradient * border, 0.8);\n"
"}\n";

static GLuint cpl_compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    // Check compilation status
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        fprintf(stderr, "Shader compilation failed: %s\n", info_log);
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

GLuint cpl_create_shader_program(void) {
    // Compile vertex shader
    GLuint vertex_shader = cpl_compile_shader(GL_VERTEX_SHADER, CPL_VERTEX_SHADER_SOURCE);
    if (vertex_shader == 0) {
        return 0;
    }
    
    // Compile fragment shader
    GLuint fragment_shader = cpl_compile_shader(GL_FRAGMENT_SHADER, CPL_FRAGMENT_SHADER_SOURCE);
    if (fragment_shader == 0) {
        glDeleteShader(vertex_shader);
        return 0;
    }
    
    // Create program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    
    // Check linking status
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(program, 512, NULL, info_log);
        fprintf(stderr, "Shader program linking failed: %s\n", info_log);
        glDeleteProgram(program);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return 0;
    }
    
    // Clean up shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    return program;
}

void cpl_destroy_shader_program(GLuint program) {
    if (program) {
        glDeleteProgram(program);
    }
}

// Advanced shader manager implementation
CPLShaderManager* cpl_create_shader_manager(void) {
    CPLShaderManager* manager = (CPLShaderManager*)calloc(1, sizeof(CPLShaderManager));
    if (!manager) return NULL;
    
    // Shader source arrays for easy iteration
    const char* vertex_sources[CPL_SHADER_COUNT] = {
        CPL_VERTEX_SHADER_SOURCE,
        CPL_GRID_VERTEX_SHADER_SOURCE,
        CPL_POINTS_VERTEX_SHADER_SOURCE,
        CPL_FILLED_VERTEX_SHADER_SOURCE
    };
    
    const char* fragment_sources[CPL_SHADER_COUNT] = {
        CPL_FRAGMENT_SHADER_SOURCE,
        CPL_GRID_FRAGMENT_SHADER_SOURCE,
        CPL_POINTS_FRAGMENT_SHADER_SOURCE,
        CPL_FILLED_FRAGMENT_SHADER_SOURCE
    };
    
    // Compile all shader programs
    for (int i = 0; i < CPL_SHADER_COUNT; i++) {
        GLuint vertex_shader = cpl_compile_shader(GL_VERTEX_SHADER, vertex_sources[i]);
        if (vertex_shader == 0) {
            cpl_destroy_shader_manager(manager);
            return NULL;
        }
        
        GLuint fragment_shader = cpl_compile_shader(GL_FRAGMENT_SHADER, fragment_sources[i]);
        if (fragment_shader == 0) {
            glDeleteShader(vertex_shader);
            cpl_destroy_shader_manager(manager);
            return NULL;
        }
        
        // Create and link program
        manager->programs[i] = glCreateProgram();
        glAttachShader(manager->programs[i], vertex_shader);
        glAttachShader(manager->programs[i], fragment_shader);
        glLinkProgram(manager->programs[i]);
        
        // Check linking status
        GLint success;
        glGetProgramiv(manager->programs[i], GL_LINK_STATUS, &success);
        if (!success) {
            char info_log[512];
            glGetProgramInfoLog(manager->programs[i], 512, NULL, info_log);
            fprintf(stderr, "Shader program %d linking failed: %s\n", i, info_log);
            glDeleteProgram(manager->programs[i]);
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);
            cpl_destroy_shader_manager(manager);
            return NULL;
        }
        
        // Clean up individual shaders
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        
        // Cache uniform locations for performance
        manager->proj_mat_locations[i] = glGetUniformLocation(manager->programs[i], "proj_mat");
        manager->color_locations[i] = glGetUniformLocation(manager->programs[i], "color");
        manager->time_locations[i] = glGetUniformLocation(manager->programs[i], "time");
        manager->resolution_locations[i] = glGetUniformLocation(manager->programs[i], "resolution");
        manager->line_width_locations[i] = glGetUniformLocation(manager->programs[i], "lineWidth");
    }
    
    manager->initialized = true;
    return manager;
}

void cpl_destroy_shader_manager(CPLShaderManager* manager) {
    if (!manager) return;
    
    for (int i = 0; i < CPL_SHADER_COUNT; i++) {
        if (manager->programs[i]) {
            glDeleteProgram(manager->programs[i]);
        }
    }
    
    free(manager);
}

GLuint cpl_get_shader_program(CPLShaderManager* manager, CPLShaderType type) {
    if (!manager || !manager->initialized || type >= CPL_SHADER_COUNT) {
        return 0;
    }
    return manager->programs[type];
}

GLint cpl_get_uniform_location(CPLShaderManager* manager, CPLShaderType type, const char* name) {
    if (!manager || !manager->initialized || type >= CPL_SHADER_COUNT) {
        return -1;
    }
    return glGetUniformLocation(manager->programs[type], name);
}
