#ifndef SPRITE_H
#define SPRITE_H

#include "animation.h"

// GLAD
#include <glad/glad.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../lib/resources/resourcecontainer.h"

class Sprite
{
public:
    Sprite();
    void init();
    void draw(glm::mat4 modelViewProjection);
    void animate(const Seconds &delta);

private:
    struct Frame {
        GLint offsetX = 0;
        GLint offsetY = 0;
        GLint width   = 0;
        GLint height  = 0;
        std::vector<ColorIndex> texture;
    };

    void initShader();
    void initTexture();
    void initVBO();

    std::vector<GobImage> m_rabbits;
    std::vector<Color> m_palette;
    std::vector<Frame> m_frames;
    ResourceContainer m_resourceContainer;
    Animation m_animation;
    glm::mat4 m_model;
    glm::mat4 m_view;
    GLfloat m_offset;
    GLsizei m_width;
    GLsizei m_height;
    GLuint m_x;
    GLuint m_y;
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_texture;
    GLuint m_texturePalette;
    GLuint m_shader;
//    uint32_t m_frame = 0;

};

#endif // SPRITE_H
