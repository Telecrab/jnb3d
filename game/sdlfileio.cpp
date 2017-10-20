#include "sdlfileio.h"

SDLFileIO::SDLFileIO(const std::__cxx11::string &fileName)
    : AbstractFileIO(fileName)
    , m_fileName(fileName)
    , m_isOpen(false)
{}


bool SDLFileIO::open()
{
    m_file = SDL_RWFromFile( m_fileName.c_str(), "rb");
    m_isOpen = m_file;
    return m_isOpen;
}

bool SDLFileIO::isOpen()
{
    return m_isOpen;
}

std::string SDLFileIO::fileName()
{
    return m_fileName;
}

bool SDLFileIO::reset()
{
    Sint64 result = SDL_RWseek(m_file, 0, RW_SEEK_SET);
    return (result >= 0);
}

int64_t SDLFileIO::size()
{
    return SDL_RWsize(m_file);
}

size_t SDLFileIO::read(char *data, size_t maxBytes)
{
    return SDL_RWread(m_file, data, 1, maxBytes);
}
