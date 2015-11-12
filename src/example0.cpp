/*
 * OpenGL example
 * autor: Michael Poddubnyi (c) 2015
 */
#include <stdio.h>
#include <SDL2/SDL_events.h>
#include <glcore_450.h>
#include "common.h"

#define EXAMPLE_CALL extern "C"

extern volatile int quit;

const float points[] = {  // vertices of triangle
    0.0f,  0.5f,  0.0f,
    0.5f, -0.5f,  0.0f,
    -0.5f, -0.5f,  0.0f
};

GLuint vbo;         // vertex buffer
GLuint vao;         // vertex array

EXAMPLE_CALL void on_init(int w, int h, int vsync) {
    UNUSED(w), UNUSED(h), UNUSED(vsync);

    // Create VBO
    glCreateBuffers(1, &vbo);
    // Allocate memory for data and send it
    glNamedBufferData(vbo, sizeof (points), points, GL_STATIC_DRAW);

    // Create VAO
    glCreateVertexArrays(1, &vao);

    // Enable vertex attribute (location = 0)
    glEnableVertexArrayAttrib(vao, 0);

    // Setup vertex array attributes
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    // Setup vertex buffer for this VAO
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(float) * 3);

    glBindVertexArray(vao);
}

EXAMPLE_CALL void on_quit(void) {
    quit = 0;
}

EXAMPLE_CALL void on_cleanup(void) {
    // Delete all resources
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

EXAMPLE_CALL void on_update(float dt) {
    UNUSED(dt);
}

EXAMPLE_CALL void on_present(int w, int h, float alpha) {
    UNUSED(alpha);

    float clear_color[4] = {0.4, 0.4, 0.4, 1};

    // Clip window
    glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
    // Setup viewport 0
    glViewportIndexedf(0, 0, 0, (float)w, (float)h);
    // Clear color buffer of current framebuffer(0)
    glClearNamedFramebufferfv(0, GL_COLOR, 0, clear_color);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

EXAMPLE_CALL void on_event(SDL_Event *event) {
    UNUSED(event);
}

