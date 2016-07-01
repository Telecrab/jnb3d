#ifndef RESOURCECONTAINER_H
#define RESOURCECONTAINER_H

#include <string>
#include <vector>
#include <cstdint>
#include "datloader.h"

using ColorIndex = uint8_t;
using Color = uint32_t;

// Also see http://stackoverflow.com/a/25938871.
struct GobImage {
    const uint8_t *buf_;

    const uint16_t& width() const { return reinterpret_cast<const uint16_t*>(buf_)[0]; }
    const uint16_t& height() const { return reinterpret_cast<const uint16_t*>(buf_)[1]; }
    const uint16_t& hotSpotX() const { return reinterpret_cast<const uint16_t*>(buf_)[2]; }
    const uint16_t& hotSpotY() const { return reinterpret_cast<const uint16_t*>(buf_)[3]; }

    const uint8_t *bitmap() const { return &buf_[8]; }
};

struct GobHeader
{
    uint16_t numberOfImages;
    std::vector<uint32_t> imageOffsets;
};

class ResourceContainer
{
public:
    ResourceContainer();

    void loadContainer(const std::string &fileName);
    std::vector<ArchiveEntry> containerContents();

    std::vector<ColorIndex> readPCXimage(const std::string &name);
    std::vector<Color> readPCXpalette(const std::string &name);
    std::vector<GobImage> readGOB(const std::string &name);
    std::vector<ColorIndex> readGobImage(const GobImage &gobImage);
    std::vector<char> readSMP(const std::string &name);
    std::vector<char> readMOD(const std::string &name);

private:
    uint16_t readUint16(char *&data);
    uint32_t readUint32(char *&data);

    DATloader m_loader;
};

#endif // RESOURCECONTAINER_H
