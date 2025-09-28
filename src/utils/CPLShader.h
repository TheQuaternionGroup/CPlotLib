#ifndef CPL_SHADER_H
#define CPL_SHADER_H

#include <GL/glew.h>
#include <stdbool.h>

// Shader program types for different rendering modes
typedef enum {
    CPL_SHADER_BASIC = 0,      // Basic line rendering
    CPL_SHADER_GRID,           // Grid rendering with anti-aliasing
    CPL_SHADER_POINTS,         // Point rendering
    CPL_SHADER_FILLED,         // Filled polygon rendering
    CPL_SHADER_COUNT
} CPLShaderType;

// Shader program structure
typedef struct {
    GLuint programs[CPL_SHADER_COUNT];
    GLint proj_mat_locations[CPL_SHADER_COUNT];
    GLint color_locations[CPL_SHADER_COUNT];
    GLint time_locations[CPL_SHADER_COUNT];
    GLint resolution_locations[CPL_SHADER_COUNT];
    GLint line_width_locations[CPL_SHADER_COUNT];
    bool initialized;
} CPLShaderManager;

// Shader management
CPLShaderManager* cpl_create_shader_manager(void);
void cpl_destroy_shader_manager(CPLShaderManager* manager);
GLuint cpl_get_shader_program(CPLShaderManager* manager, CPLShaderType type);
GLint cpl_get_uniform_location(CPLShaderManager* manager, CPLShaderType type, const char* name);

// Legacy functions for backward compatibility
GLuint cpl_create_shader_program(void);
void cpl_destroy_shader_program(GLuint program);

// Shader source code
extern const char* CPL_VERTEX_SHADER_SOURCE;
extern const char* CPL_FRAGMENT_SHADER_SOURCE;

#endif // CPL_SHADER_H
