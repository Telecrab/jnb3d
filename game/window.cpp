#include "window.h"

#include <iostream>
#include <cstdint>
#include <thread>

#include "calllogger.h"

Window* Window::m_instance = nullptr;

// GLAD_DEBUG is only defined if the c-debug generator was used
#ifdef GLAD_DEBUG
// logs every gl call to the console
void pre_gl_call(const char *name, void *funcptr, int len_args, ...) {
    cout << "Calling: " << name << " (" << len_args << " arguments)" << endl;
}
#endif

void error_callback(int /*error*/, const char* description)
{
    std::cout << description << std::endl;
}

const char Window::TITLE[] = "Заяц";

[[noreturn]] static void sdl_die(const char * message) {
    std::cerr << message << ": " << SDL_GetError() << std::endl;
    exit(2);
}

void Window::framebuffersize_callback(int width, int height)
{
    std::cout << "RESIZE!" << std::endl;
    getInstance()->setViewMatrix( glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f) );

    float ratio = 1.0f;
    if(width < height) {
        ratio = width / 640.0f;
    } else {
        ratio = height / 640.0f;
    }

    getInstance()->setViewMatrix( glm::scale(getInstance()->getViewMatrix(), glm::vec3(4.0f * ratio, 4.0f * ratio, 1.0f)) );
    glViewport(0, 0, width, height);
}

Window::Window(GLuint width, GLuint height, bool fullscreen)
    : m_width(width)
    , m_height(height)
    , m_fullscreen(fullscreen)
    , m_isRunning(true)
    , m_model(1.0)
    , m_view(1.0)
    , m_framesRendered(0)
    , m_vsync(false)
{
    CallLogger::enable();
    LOG_CALL
    // To prevent looping when calling callbacks in the constructor.
    m_instance = this;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        sdl_die("Couldn't initialize SDL");
    atexit(SDL_Quit);
    SDL_GL_LoadLibrary(NULL); // Default OpenGL is fine.

    // Request an OpenGL 3.3 context (should be core)
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    // Also request a depth buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create the window
    if (m_fullscreen) {
        m_window = SDL_CreateWindow(
                    TITLE,
                    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                    0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL
                    );
    } else {
        m_window = SDL_CreateWindow(
                    TITLE,
                    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                    m_width, m_height, SDL_WINDOW_OPENGL
                    );

//                SDL_SetWindowMinimumSize(m_window, m_width, m_height);
//                SDL_SetWindowMaximumSize(m_window, m_width, m_height);
    }
    if (m_window == nullptr) sdl_die("Couldn't set video mode");

    m_context = SDL_GL_CreateContext(m_window);
    if (m_context == nullptr)
        sdl_die("Failed to create OpenGL context");

    // Check OpenGL properties
    printf("OpenGL loaded\n");
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));
    printf("GLSL Version:  %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Use v-sync
    SDL_GL_SetSwapInterval(m_vsync);

    int w,h;
    SDL_GetWindowSize(m_window, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.0f, 0.5f, 1.0f, 0.0f);

#ifdef GLAD_DEBUG
    // before every opengl call call pre_gl_call
    glad_set_pre_callback(pre_gl_call);

    // post callback checks for glGetError by default

    // don't use the callback for glClear
    // (glClear could be replaced with your own function)
    glad_debug_glClear = glad_glClear;
    glad_debug_glClearColor = glad_glClearColor;
#endif

    init();
}

int Window::mainLoop()
{
    LOG_CALL
    std::cout << "high_resolution_clock durations can represent:\n";
    std::cout << "min: " << std::chrono::high_resolution_clock::duration::min().count() << "\n";
    std::cout << "max: " << std::chrono::high_resolution_clock::duration::max().count() << "\n";

    std::cout << "sizeof(int)=" << sizeof(int) << std::endl;
    std::cout << "sizeof(long)=" << sizeof(long) << std::endl;
    std::cout << "sizeof(void*)=" << sizeof(void*) << std::endl;

    oldTime = std::chrono::high_resolution_clock::now();
    animTime = oldTime;

    // Game loop
    while (m_isRunning) {
        auto beginTime = std::chrono::high_resolution_clock::now();

        while (SDL_PollEvent(&m_event)) {
            switch(m_event.type) {
            case SDL_QUIT:
                m_isRunning = false;
                break;

            case SDL_WINDOWEVENT:
                if(SDL_WINDOWEVENT_RESIZED == m_event.window.event) {
                    m_width = m_event.window.data1;
                    m_height = m_event.window.data2;
                    framebuffersize_callback(m_width, m_height);
                }
                break;

            case SDL_KEYDOWN:
                switch(m_event.key.keysym.scancode) {

                case SDL_SCANCODE_ESCAPE:
                    m_isRunning = false;
                    break;

                case SDL_SCANCODE_SPACE:
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                                             "ОЛОЛО!",
                                             "Hey!",
                                             NULL);
                    break;

                case SDL_SCANCODE_V:
                    m_vsync = !m_vsync;
                    SDL_GL_SetSwapInterval(m_vsync);
                    std::cout << "VSync is " << m_vsync << std::endl;
                    break;

                default:
                    break;

                }
            }
        }

        render();
        SDL_GL_SwapWindow(m_window);

        auto endTime = std::chrono::high_resolution_clock::now();

        std::chrono::microseconds mainLoopTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - beginTime);
        if(!m_vsync) {
            std::this_thread::sleep_for( std::chrono::microseconds(16666) - mainLoopTime );
        }
//        std::cout << "Main loop takes " << mainLoopTime.count() << " microseconds." << std::endl;
    }

    // Delete our OpengL context
    SDL_GL_DeleteContext(m_context);

    // Destroy our window
    SDL_DestroyWindow(m_window);

    // Shutdown SDL 2
    SDL_Quit();

    return 0;
}

void Window::init()
{
    LOG_CALL
    sprite.init();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    framebuffersize_callback(m_width, m_height);
}

void Window::updateFps(std::chrono::microseconds timeSpan)
{
    LOG_CALL
    ++m_framesRendered;
    m_timeCounter += timeSpan;
    if( m_timeCounter.count() >= 1.0f) {
        std::cout << m_framesRendered << " frames per "
                  << m_timeCounter.count() << " seconds @ "
                  << m_framesRendered / m_timeCounter.count() << " fps" << std::endl;
        m_framesRendered = 0;
        m_timeCounter = m_timeCounter.zero();
    }
}

void Window::render()
{
    LOG_CALL
    // Render
    // Clear the colorbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    std::chrono::microseconds timeSpan = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - oldTime); // Milliseconds passed between frames.
    double delta = timeSpan.count() / 1000.0;
    updateFps(timeSpan);
    oldTime = std::chrono::high_resolution_clock::now();
//    std::cout << delta << std::endl;
    sprite.animate(timeSpan);

    static double const step = 0.05;
    glm::vec3 direction(0.0f);

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_LEFT]) {
        direction.x += -step * delta;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        direction.x += step * delta;
    }

    if (state[SDL_SCANCODE_UP]) {
        direction.y += step * delta;
    }
    if (state[SDL_SCANCODE_DOWN]) {
        direction.y += -step * delta;
    }

    m_model = glm::translate(m_model, direction);
    glm::mat4 mv = m_view * m_model;

    sprite.draw(mv);

}

void Window::setViewMatrix(const glm::mat4 &matrix)
{
    m_view = matrix;
}
