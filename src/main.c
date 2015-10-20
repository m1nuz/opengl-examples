#include <SDL2/SDL.h>
#include <glcore_450.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define TIMESTEP 0.001f

void on_init(int w, int h, int vsync);
void on_quit(void);
void on_cleanup(void);
void on_update(float dt);
void on_present(int w, int h, float alpha);
void on_event(SDL_Event *event);

volatile int quit = 1;

static const char *debug_source_to_string(GLenum source) {
    switch (source) {
    case GL_DEBUG_SOURCE_API:
        return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        return "Window System";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        return "Shader Compiler";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        return "Third Party";
    case GL_DEBUG_SOURCE_APPLICATION:
        return "Application";
    case GL_DEBUG_SOURCE_OTHER:
        return "Other";
    default:
        return "Unknown";
    }
}

static const char *debug_type_to_string(GLenum type) {
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        return "Error";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        return "Deprecated Behavior";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        return "Undefined Behavior";
    case GL_DEBUG_TYPE_PORTABILITY:
        return "Portability";
    case GL_DEBUG_TYPE_PERFORMANCE:
        return "Performance";
    case GL_DEBUG_TYPE_OTHER:
        return "Other";
    case GL_DEBUG_TYPE_MARKER:
        return "Marker";
    case GL_DEBUG_TYPE_PUSH_GROUP:
        return "Push Group";
    case GL_DEBUG_TYPE_POP_GROUP:
        return "Pop Group";
    default:
        return "Unknown";
    }

}

static const char *debug_severity_to_string(GLenum severity) {
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        return "High";
    case GL_DEBUG_SEVERITY_MEDIUM:
        return "Medium";
    case GL_DEBUG_SEVERITY_LOW:
        return "Low";
    default:
        return "Unknown";
    }
}

static void APIENTRY debug_output_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *param) {
    const char *sourceStr = debug_source_to_string(source);
    const char *typeStr = debug_type_to_string(type);
    const char *severityStr = debug_severity_to_string(severity);

    fprintf(stderr, "%s:%s[%s](%d) %s\n", sourceStr, typeStr, severityStr, id, message);
}

extern int
main(int argc, char *argv[]) {
    int width = SCREEN_WIDTH, height = SCREEN_HEIGHT;
    int vsync = 0;

    SDL_Window *window;
    SDL_GLContext context;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    if ((window = SDL_CreateWindow(APP_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN)) == NULL) {
        printf("SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    SDL_GetWindowSize(window, &width, &height);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG | SDL_GL_CONTEXT_DEBUG_FLAG);

    if ((context = SDL_GL_CreateContext(window)) == NULL) {
        printf("SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    glLoadFunctions();
    glLoadExtensions();

    glDebugMessageCallback(debug_output_callback, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);

    glEnable(GL_DEBUG_OUTPUT);

    on_init(width, height, vsync);

    if (vsync)
        SDL_GL_SetSwapInterval(1);

    SDL_Event event;

    Uint64 current = 0;
    Uint64 last = 0;

    unsigned int timesteps = 0;
    float accumulator = 0.0f;

    while (quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                on_quit();
            else
                on_event(&event);
        }

        last = current;
        current = SDL_GetPerformanceCounter();
        Uint64 freq = SDL_GetPerformanceFrequency();

        float delta = (double)(current - last) / (double)freq;

        if (delta <= 0.0f)
            continue;

        if (delta > 0.2)
            delta = 0.2;

        accumulator += delta;

        while (accumulator >= TIMESTEP) {
            accumulator -= TIMESTEP;

            on_update(TIMESTEP);

            timesteps++;
        }

        on_present(width, height, accumulator / TIMESTEP);

        SDL_GL_SwapWindow(window);
    }

    on_cleanup();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
