#ifndef SDLFILEIO_H
#define SDLFILEIO_H

#include "../lib/resources/abstractfileio.h"
#include "SDL2/SDL_rwops.h"

class SDLFileIO: public AbstractFileIO
{
public:
    SDLFileIO(const std::string &fileName);

    // AbstractFileIO interface
public:
    virtual bool open() override;
    virtual bool isOpen() override;
    virtual std::string fileName() override;
    virtual bool reset() override;
    virtual int64_t size() override;
    virtual size_t read(char *data, size_t maxBytes) override;

private:
    SDL_RWops *m_file;
    std::string m_fileName;
    bool m_isOpen;
};

#endif // SDLFILEIO_H
