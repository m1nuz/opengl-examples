/*
 * OpenGL example
 * autor: Michael Poddubnyi (c) 2015
 */
#include <SDL2/SDL_events.h>
#include <glcore_450.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstddef>
#include <cstdint>

#define EXAMPLE_CALL extern "C"

extern volatile int quit;

extern "C" void* load_targa(const char *filepath, GLuint *iformat, GLenum *format, GLsizei *width, GLsizei *height);

#include "cube.h"

struct Material {
    glm::vec4   color;
    float       layer;
    float       padding[3];
};

const char* vertex_shader =
        "#version 450 core\n"
        "#define MAX_INSTANCES 6\n"

        "layout(location = 0) in vec3 position;"
        "layout(location = 1) in vec2 texcoord;"
        "layout(location = 2) in vec3 normal;"

        "layout (std140) uniform MatrixBlock {"
        "mat4 projection_view;"
        "mat4 model[MAX_INSTANCES];"
        "};"

        "out gl_PerVertex {"
        "vec4 gl_Position;"
        "};"

        "out vertex {"
        "vec3 normal;"
        "vec2 texcoord;"
        "uint index;"
        "} vs_output;"

        "void main () {"
        "  vs_output.texcoord = texcoord;"
        "  vs_output.normal = vec3(model[gl_InstanceID] * vec4(normal, 0));"
        "  vs_output.index = gl_InstanceID;"
        "  gl_Position = projection_view * model[gl_InstanceID] * vec4(position, 1.0);"
        "}";

const char* fragment_shader =
        "#version 450 core\n"
        "#define MAX_MATERIALS 6\n"

        "uniform vec3 color;"
        "layout(binding = 0) uniform sampler2DArray tex;"

        "struct Material {"
        "vec4 color;"
        "float layer;"
        "};"

        "layout (std140) uniform MaterialBlock {"
        "Material materials[MAX_MATERIALS];"
        "};"

        "in vertex {"
        "vec3 normal;"
        "vec2 texcoord;"
        "uint index;"
        "} fs_input;"

        "layout (location = 0, index = 0) out vec4 frag_color;"
        "void main () {"
        "  vec3 n = normalize(fs_input.normal);"
        "  frag_color = materials[fs_input.index].color * texture(tex, vec3(fs_input.texcoord, materials[fs_input.index].layer));"
        "}";

GLuint ubo;         // uniform buffer for matrices
GLuint ubo2;        // uniform buffer for materials
GLuint vbo;         // vertex buffer
GLuint ebo;         // element buffer
GLuint vao;         // vertex array
GLuint pipeline;    // program pipeline
GLuint vs;          // vertex program
GLuint fs;          // fragment program

GLuint sampler;
GLint loc_color;    // "color" uniform location
GLuint tex_array;

static void init_textures() {
    const int base_w = 512;
    const int base_h = 512;

    const char *names[] = {
        "../textures/brick_guiGen_512_d.tga",
        "../textures/FloorBrick_JFCartoonyFloorBrickDirty_512_d.tga",
        "../textures/Ground_MossyDirt_512_d.tga",
        "../textures/Metal_SciFiDiamondPlate_512_d.tga",
        "../textures/Misc_OakbarrelOld_512_d.tga",
        "../textures/rock_guiWallSmooth09_512_d.tga"
    };

    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &tex_array);
    glTextureStorage3D(tex_array, 4, GL_RGB8, base_w, base_h, 6);

    for (int i = 0; i < 6; i++) {
        GLuint iformat;
        GLenum format;
        GLsizei tw;
        GLsizei th;

        void *pixels = load_targa(names[i], &iformat, &format, &tw, &th);

        glTextureSubImage3D(tex_array, 0, 0, 0, i, tw, th, 1, format, GL_UNSIGNED_BYTE, pixels);
        glGenerateTextureMipmap(tex_array);

        free(pixels);
    }
}

EXAMPLE_CALL void on_init(int w, int h, int vsync) {
    // Create texture sampler
    glCreateSamplers(1, &sampler);

    // Setup sampler
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Load textures
    init_textures();

    // Creeate UBO for matrices
    glCreateBuffers(1, &ubo);
    // Allocate memory for data
    glNamedBufferData(ubo, sizeof(glm::mat4) * 7, NULL, GL_STREAM_DRAW);

    Material materials[6] = {
        {glm::vec4(1, 0, 0, 1), 0},
        {glm::vec4(0, 1, 0, 1), 1},
        {glm::vec4(0, 0, 1, 1), 2},
        {glm::vec4(1, 1, 0, 1), 3},
        {glm::vec4(1, 0, 1, 1), 4},
        {glm::vec4(0, 1, 1, 1), 5},
    };

    // Create UBO for materials
    glCreateBuffers(1, &ubo2);
    // Allocate memory for data and send it
    glNamedBufferData(ubo2, sizeof(Material) * 6, materials, GL_STATIC_DRAW);

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
    loc_color = glGetUniformLocation(fs, "color");

    // Bind ubo
    int block_index = glGetUniformBlockIndex(vs, "MatrixBlock");
    glUniformBlockBinding(vs, block_index, 0);

    block_index = glGetUniformBlockIndex(fs, "MaterialBlock");
    glUniformBlockBinding(fs, block_index, 1);

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
    glDeleteBuffers(1, &ubo);
    glDeleteBuffers(1, &ubo2);
    glDeleteVertexArrays(1, &vao);
    glDeleteTextures(1, &tex_array);
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

    mat4 projection = perspective(45.f, (float)w/(float)h, 1.f, 100.f);
    mat4 view = translate(mat4(1.f), vec3(0, 0, -10.f));
    mat4 pvm = projection * view;

    float clear_color[4] = {0.4, 0.4, 0.4, 1};
    float clear_depth = 1.0f;

    mat4 models[6];
    vec3 angles[6] = {
        { 0,  1,  1},
        { 1,  0, -1},
        {-1, -1,  0},
        { 1,  0,  1},
        { 0,  1,  1},
        {-1,  0, -1},
    };

    for (int i = 0; i < 6; i++) {
        models[i] = translate(mat4(1.f), vec3((i % 2) * 4 - 1.5f, (i % 3) * 3 - 3.f, 0));
        models[i] = rotate(models[i], angle * angles[i][0], vec3(1, 0, 0));
        models[i] = rotate(models[i], angle * angles[i][1], vec3(0, 1, 0));
        models[i] = rotate(models[i], angle * angles[i][2], vec3(0, 0, 1));
        models[i] = scale(models[i], vec3(1.f));
    }

    // Update ubo buffer with matrices
    void *ptr = glMapNamedBufferRange(ubo, 0, sizeof(mat4) * 7, GL_MAP_WRITE_BIT);
    memcpy(ptr, &pvm[0][0], sizeof (mat4));
    memcpy(ptr + sizeof (mat4), models, sizeof (mat4) * 6);
    glUnmapNamedBuffer(ubo);

    // Clip window
    glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
    // Setup viewport 0
    glViewportIndexedf(0, 0, 0, (float)w, (float)h);
    // Clear color buffer of current framebuffer(0)
    glClearNamedFramebufferfv(0, GL_COLOR, 0, clear_color);
    // Clear depth buffer of current framebuffer(0)
    glClearNamedFramebufferfv(0, GL_DEPTH, 0, &clear_depth);

    glEnable(GL_DEPTH_TEST);

    glProgramUniform3f(fs, loc_color, 0.8, 0.9, 0.8);

    // Bind ubo
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, sizeof (mat4) * 7);
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, ubo2, 0, sizeof (Material) * 6);

    // Bind texture and sampler to unit 0
    glBindTextureUnit(0, tex_array);
    glBindSampler(0, sampler);

    glDrawElementsInstanced(GL_TRIANGLES, CUBE_INDICES_NUM, GL_UNSIGNED_SHORT, NULL, 6);

    glDisable(GL_DEPTH_TEST);
}

EXAMPLE_CALL void on_event(SDL_Event *event) {

}
