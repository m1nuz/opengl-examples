/*
 * OpenGL example
 * autor: Michael Poddubnyi (c) 2015
 */
#include <stdio.h>
#include <SDL2/SDL_events.h>
#include <glcore_450.h>

#define EXAMPLE_CALL extern "C"

extern volatile int quit;

const char* vertex_shader =
        "#version 450 core\n"
        "uniform vec3 offset;"

        "layout(location = 0) in vec3 position;"

        "out gl_PerVertex {"
        "vec4 gl_Position;"
        "};"

        "void main () {"
        "  gl_Position = vec4(position + offset, 1.0);"
        "}";

const char* fragment_shader =
        "#version 450 core\n"
        "uniform vec3 color;"
        "layout (location = 0, index = 0) out vec4 frag_color;"
        "void main () {"
        "  frag_color = vec4(color, 1.0);"
        "}";

const float points[] = {  // vertices of triangle
    0.0f,  0.25f,  0.0f,
    0.25f, -0.25f,  0.0f,
    -0.25f, -0.25f,  0.0f
};

GLuint vbo;         // vertex buffer
GLuint vao;         // vertex array
GLuint pipeline;    // program pipeline
GLuint vs;          // vertex program
GLuint fs;          // fragment program
GLint loc_offset;   // "offset" uniform location
GLint loc_color;    // "color" uniform location

EXAMPLE_CALL void on_init(int w, int h, int vsync) {
    const char *version = (const char*)glGetString(GL_VERSION);
    const char *renderer = (const char*)glGetString(GL_RENDERER);
    const char *vendor = (const char*)glGetString(GL_VENDOR);
    const char *glsl_version = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GL version: %s\nGL renderer: %s\nGL vendor: %s\nGL shading language version: %s\n", version, renderer, vendor, glsl_version);    

    // Create VBO
    glCreateBuffers(1, &vbo);
    // Allocate memory for data and send it
    glNamedBufferData(vbo, sizeof (points), points, GL_STATIC_DRAW);

    // Create VAO
    glCreateVertexArrays(1, &vao);

    // Enable vertex attribute (location = 0)
    glEnableVertexArrayAttrib(vao, 0);

    // Setup vertex array attributes
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(float) * 3);

    // Create program pipeline
    glCreateProgramPipelines(1, &pipeline);

    // Create vertex and fragment seperable programs
    vs = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &vertex_shader);
    fs = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &fragment_shader);

    // Print programs log
    char program_log[4096];
    GLsizei written = 0;
    glGetProgramInfoLog(vs, sizeof(program_log), &written, program_log);
    if (written > 0)
        printf("%s\n", program_log);

    glGetProgramInfoLog(fs, sizeof(program_log), &written, program_log);
    if (written > 0)
        printf("%s\n", program_log);

    // Use programs in pipeline
    glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vs);
    glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fs);

    // Get programs uniform locations
    loc_offset = glGetUniformLocation(vs, "offset");
    loc_color = glGetUniformLocation(fs, "color");

    // Bind pipeline and vao
    glBindProgramPipeline(pipeline);
    glBindVertexArray(vao);
}

EXAMPLE_CALL void on_quit(void) {
    quit = 0;
}

EXAMPLE_CALL void on_cleanup(void) {
    // Delete all resources
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(vs);
    glDeleteProgram(fs);
    glDeleteProgramPipelines(1, &pipeline);
}

EXAMPLE_CALL void on_update(float dt) {

}

typedef float float3[3];

EXAMPLE_CALL void on_present(int w, int h, float alpha) {
    float clear_color[4] = {0.4, 0.4, 0.4, 1};

    // Clip window
    glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
    // Setup viewport 0
    glViewportIndexedf(0, 0, 0, (float)w, (float)h);
    // Clear color buffer of current framebuffer(0)
    glClearNamedFramebufferfv(0, GL_COLOR, 0, clear_color);

    float3 offsets[3] =
    {
        {-0.25, -0.25, 0},
        { 0.0,  0.25, 0},
        { 0.25, -0.25, 0},
    };

    float3 colors[3] =
    {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };

    for (int i = 0; i < 3; i++) {
        // Set offset and color of the triangle
        glProgramUniform3fv(vs, loc_offset, 1, &offsets[i][0]);
        glProgramUniform3fv(fs, loc_color, 1, &colors[i][0]);

        // Draw the triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}

EXAMPLE_CALL void on_event(SDL_Event *event) {

}
