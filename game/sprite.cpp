#include "sprite.h"

#include <iostream>
#include <fstream>

#include "sdlfileio.h"
#include "calllogger.h"

using namespace std;

Sprite::Sprite()
    : m_animation(0.0f, 3.99f, Seconds(0.3f), true)
    , m_offset(0.0f)
{
    LOG_CALL
    m_resourceContainer.loadContainer( std::make_shared<SDLFileIO>("jumpbump.dat") );
    m_animation.start();
}

void Sprite::init()
{
    LOG_CALL
    initShader();
    initTexture();
    initVBO();
}

void Sprite::draw(glm::mat4 modelViewProjection)
{
    LOG_CALL
    glBindVertexArray(m_vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, m_texturePalette);

    Frame frame = m_frames[m_animation.value()];
    m_model = glm::scale( glm::mat4(), glm::vec3(frame.width, frame.height, 1) );

    glm::mat4 mvp = modelViewProjection * m_model;

    GLint location = glGetUniformLocation(m_shader, "matrix");
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mvp));

    location = glGetUniformLocation(m_shader, "tex");
    glUniform1i(location, 0);
    location = glGetUniformLocation(m_shader, "palette");
    glUniform1i(location, 1);

    location = glGetUniformLocation(m_shader, "offset");
    glUniform1f(location, frame.offsetX);

    location = glGetUniformLocation(m_shader, "width");
    glUniform1f(location, frame.width);
    location = glGetUniformLocation(m_shader, "height");
    glUniform1f(location, frame.height);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Sprite::animate(const Seconds &delta)
{
    LOG_CALL
    m_animation.update(delta);
}

void Sprite::initShader()
{
    LOG_CALL
    std::cout << "Compiling shader!" << endl;
    char const *vertexShaderSource = "#version 330 core\n"
                                     "in vec4 vertex;\n"
                                     "uniform mat4 matrix;\n"
                                     "uniform float width;\n"
                                     "uniform float height;\n"
                                     "out vec3 coord;\n"
                                     "void main(void)\n"
                                     "{\n"
                                     "   coord = vertex.xyz * vec3(width, -height, 1.0) + vec3(0.0, height, 0.0);\n"
                                     "   gl_Position = matrix * vertex;\n"
                                     "}";
    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }

    char const *fragmentShaderSource = "#version 330 core\n"
                                       "uniform usampler2D tex;\n"
                                       "uniform sampler1D palette;\n"
                                       "uniform float offset;\n"
                                       "in highp vec3 coord;\n"
                                       "out highp vec4 color;\n"
                                       "void main(void)\n"
                                       "{\n"
                                       "    uint colorIndex = texelFetch(tex, ivec2(coord.x + offset, coord.y), 0).r;\n"
                                       "    color = texelFetch(palette, int(colorIndex), 0);\n"

                                       "    //if ( colorIndex != uint(0) ) color = vec4( 1.0, 0.0, 0.0, 1.0 );\n"
                                       "    //color = texelFetch( tex, ivec2(coord.xy), 0 ) / vec4(256.0 ,1.0, 1.0, 1.0);\n"
                                       "    //color = vec4( coord.xy / vec2(13.0, 15.0), 0.0, 1.0 );\n"
                                       "    //color = vec4( float(colorIndex), 0.0, 0.0, 1.0 );\n"
                                       "}";
    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
        std::ofstream out("test.frag");
        std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
        std::cout.rdbuf(out.rdbuf());
        std::cout << fragmentShaderSource;
        std::cout.rdbuf(coutbuf);
    }

    m_shader = glCreateProgram();
    glAttachShader(m_shader, vertexShader);
    glAttachShader(m_shader, fragmentShader);
    glLinkProgram(m_shader);
    glGetProgramiv(m_shader, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(m_shader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::LINK_FAILED\n" << infoLog << endl;
    }

    glUseProgram(m_shader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Sprite::initTexture()
{
    LOG_CALL
    m_rabbits = m_resourceContainer.readGOB("rabbit.gob");
    std::vector<char> texture;
    m_width = 0;
    m_height = 0;

    for (std::vector<GobImage>::size_type i = 0; i < m_rabbits.size(); i++) {
        GobImage rabbit = m_rabbits[i];
        uint16_t imageWidth = rabbit.width();
        uint16_t imageHeight = rabbit.height();

        Frame frame;
        frame.offsetX = m_width;
        frame.width = GLint(imageWidth);
        frame.height = GLint(imageHeight);
        m_frames.push_back(frame);

        m_height = std::max( m_height, GLsizei(imageHeight) );
        m_width += GLsizei(imageWidth);
    }

    for (GLsizei row = 0; row < m_height; row++) {
        for (std::vector<GobImage>::size_type i = 0; i < m_rabbits.size(); i++) {
            GobImage rabbit = m_rabbits[i];
            uint16_t imageWidth = rabbit.width();
            uint16_t imageHeight = rabbit.height();

            if (row > imageHeight - 1) {
                texture.insert( texture.end(), imageWidth, 0 );
            } else {
                auto start = rabbit.bitmap() + row * imageWidth;
                texture.insert( texture.end(), start, start + imageWidth );
            }
        }
    }
    m_model = glm::scale( m_model, glm::vec3(m_width, m_height, 1) );

    std::cout << "Rabbits texture size:" << m_height << "x" << m_width << std::endl;

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, m_width, m_height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, texture.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    auto palette = m_resourceContainer.readPCXpalette("level.pcx");
    palette[0] = 0x00000000;
    glGenTextures(1, &m_texturePalette);
    glBindTexture(GL_TEXTURE_1D, m_texturePalette);
    glTexImage1D( GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, palette.data() );
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Sprite::initVBO()
{
    LOG_CALL
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(0);

    GLfloat vertices[] = {
         0.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  0.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

