#ifndef WINDOW_H
#define WINDOW_H

#include "common.h"

// GLAD
#include <glad/glad.h>
#undef GLAD_DEBUG

// SDL
#include <SDL2/SDL.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sprite.h"

class WindowSingleton {

};

class Window
{
public:
    static Window* getInstance(GLuint width = 600, GLuint height = 600, bool fullscreen = false)
    {
        if(!m_instance)
            new Window(width, height, fullscreen); // m_instance is set in the constructor.
        return m_instance;
    }

    Window(const Window&) = delete;
    Window& operator=(Window&) = delete;

    int mainLoop();

private:
    Window(GLuint width = 600, GLuint height = 600, bool fullscreen = false);
    static Window* m_instance;

    static const char TITLE[];

    void init();
    void render();
    void setViewMatrix(const glm::mat4& matrix);
    const glm::mat4& getViewMatrix() {return m_view;}
    static void framebuffersize_callback(int width, int height);
    void updateFps(std::chrono::microseconds timeSpan);

    SDL_Window *m_window;
    SDL_GLContext m_context;
    SDL_Event m_event;

    GLsizei m_width;
    GLsizei m_height;
    bool m_fullscreen;

    bool m_isRunning;

    std::chrono::high_resolution_clock::time_point oldTime;
    std::chrono::high_resolution_clock::time_point animTime;

    glm::mat4 m_model;
    glm::mat4 m_view;

    Sprite sprite;

    Seconds m_timeCounter;
    uint32_t m_framesRendered;

    bool m_vsync;
};

#endif // WINDOW_H
