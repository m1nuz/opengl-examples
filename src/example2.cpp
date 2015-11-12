/*
 * OpenGL example
 * autor: Michael Poddubnyi (c) 2015
 */
#include <SDL2/SDL_events.h>
#include <glcore_450.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstddef>

#define EXAMPLE_CALL extern "C"

extern volatile int quit;

extern "C" void* load_targa(const char *filepath, GLuint *iformat, GLenum *format, GLsizei *width, GLsizei *height);

#include "cube.h"

const char* vertex_shader =
        "#version 430 core\n"
        "uniform mat4 matrix_world;"

        "layout(location = 0) in vec3 position;"
        "layout(location = 1) in vec2 texcoord;"
        "layout(location = 2) in vec3 normal;"

        "out gl_PerVertex {"
        "vec4 gl_Position;"
        "};"

        "out vertex {"
        "vec3 normal;"
        "vec2 texcoord;"
        "} vs_output;"

        "void main () {"
        "  vs_output.texcoord = texcoord;"
        "  vs_output.normal = vec3(matrix_world * vec4(normal, 0));"
        "  gl_Position = matrix_world * vec4(position, 1.0);"
        "}";

const char* fragment_shader =
        "#version 430 core\n"
        "uniform vec3 color;"
        "layout(binding = 0)  uniform sampler2D tex;"

        "in vertex {"
        "vec3 normal;"
        "vec2 texcoord;"
        "} fs_input;"

        "layout (location = 0, index = 0) out vec4 frag_color;"
        "void main () {"
        "  vec3 n = normalize(fs_input.normal);"
        "  frag_color = vec4(color, 1.0) * texture(tex, fs_input.texcoord);"
        "}";


GLuint vbo;         // vertex buffer
GLuint ebo;         // element buffer
GLuint vao;         // vertex array
GLuint pipeline;    // program pipeline
GLuint vs;          // vertex program
GLuint fs;          // fragment program

GLuint tex_color;
GLuint sampler;
GLint loc_mvp;      // "world" matrix uniform location
GLint loc_color;    // "color" uniform location

EXAMPLE_CALL void on_init(int w, int h, int vsync) {
    // Create texture sampler
    glCreateSamplers(1, &sampler);

    // Setup sampler
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Load texture
    GLuint iformat;
    GLenum format;
    GLsizei tw;
    GLsizei th;

    void *pixels = load_targa("../textures/texture_01.tga", &iformat, &format, &tw, &th);

    glCreateTextures(GL_TEXTURE_2D, 1, &tex_color);
    glTextureStorage2D(tex_color, 4, iformat, tw, th);
    glTextureSubImage2D(tex_color, 0, 0, 0, tw, th, format, GL_UNSIGNED_BYTE, pixels);
    glGenerateTextureMipmap(tex_color);

    free(pixels);

    // Create VBO
    glCreateBuffers(1, &vbo);
    // Allocate memory for data and send it
    glNamedBufferData(vbo, CUBE_VERTICES_NUM * sizeof(v3t2n3_t), cube_vertices, GL_STATIC_DRAW);

    // Create VBO for elements
    glCreateBuffers(1, &ebo);
    // Allocate memory for data and send it
    glNamedBufferData(ebo, CUBE_INDICES_NUM * sizeof(uint16_t), cube_indices, GL_STATIC_DRAW);

    // Create VAO
    glCreateVertexArrays(1, &vao);

    // Enable vertex attribute (location = 0)
    glEnableVertexArrayAttrib(vao, 0);
    // Setup vertex array attributes
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(v3t2n3_t, position));

    // Enable vertex attribute (location = 1)
    glEnableVertexArrayAttrib(vao, 1);
    // Setup vertex array attributes
    glVertexArrayAttribBinding(vao, 1, 0);
    glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(v3t2n3_t, texcoord));

    // Enable vertex attribute (location = 2)
    glEnableVertexArrayAttrib(vao, 2);
    // Setup vertex array attributes
    glVertexArrayAttribBinding(vao, 2, 0);
    glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof(v3t2n3_t, normal));

    // Setup vertex buffer and element buffer for VAO
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(v3t2n3_t));
    glVertexArrayElementBuffer(vao, ebo);

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
    loc_mvp = glGetUniformLocation(vs, "matrix_world");
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
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    glDeleteTextures(1, &tex_color);
    glDeleteSamplers(1, &sampler);
    glDeleteProgram(vs);
    glDeleteProgram(fs);
    glDeleteProgramPipelines(1, &pipeline);
}

static float current_angle = 0;
static float previous_angle = 0;

EXAMPLE_CALL void on_update(float dt) {
    previous_angle = current_angle;
    current_angle += 0.5f * dt;
}

EXAMPLE_CALL void on_present(int w, int h, float alpha) {
    using namespace glm;

    float angle = mix(previous_angle, current_angle, alpha);

    mat4 model = translate(mat4(1.f), vec3(0, 0, 0));
    model = rotate(model, angle, vec3(1, 0, 0));
    model = rotate(model, 0.f, vec3(0, 1, 0));
    model = rotate(model, angle, vec3(0, 0, 1));
    model = scale(model, vec3(1.f));

    mat4 projection = perspective(45.f, (float)w/(float)h, 1.f, 100.f);
    mat4 view = translate(mat4(1.f), vec3(0, 0, -5.f));
    mat4 mvp = projection * view * model;

    float clear_color[4] = {0.4, 0.4, 0.4, 1};
    float clear_depth = 1.0f;

    // Clip window
    glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
    // Setup viewport 0
    glViewportIndexedf(0, 0, 0, (float)w, (float)h);
    // Clear color buffer of current framebuffer(0)
    glClearNamedFramebufferfv(0, GL_COLOR, 0, clear_color);
    // Clear depth buffer of current framebuffer(0)
    glClearNamedFramebufferfv(0, GL_DEPTH, 0, &clear_depth);

    glEnable(GL_DEPTH_TEST);

    glProgramUniformMatrix4fv(vs, loc_mvp, 1, GL_FALSE, &mvp[0][0]);
    glProgramUniform3f(fs, loc_color, 0.8, 0.9, 0.8);

    // bind texture and sampler to unit 0
    glBindTextureUnit(0, tex_color);
    glBindSampler(0, sampler);

    glDrawElements(GL_TRIANGLES, CUBE_INDICES_NUM, GL_UNSIGNED_SHORT, 0);

    glDisable(GL_DEPTH_TEST);
}

EXAMPLE_CALL void on_event(SDL_Event *event) {

}
