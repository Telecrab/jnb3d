#include "resourcecontainer.h"

ResourceContainer::ResourceContainer()
{

}

void ResourceContainer::loadContainer(const std::string &fileName)
{
    m_loader.loadArchive(fileName);
}

std::vector<ArchiveEntry> ResourceContainer::containerContents()
{
    return m_loader.archiveContents();
}

std::vector<ColorIndex> ResourceContainer::readPCXimage(const std::string &name)
{
    std::vector<ColorIndex> image(400 * 256); // 400x256 8-bit indexed image as described in modify.txt.

    EntrySize entrySize;
    char *entryData = m_loader.getEntryData( name, entrySize );
    uint8_t currentByte;
    uint8_t fillerByte;
    int pcxOffset = 128;
    int levelImageOffset = 0;

    while (levelImageOffset < 400 * 256) {
        currentByte = entryData[pcxOffset++];

        if ((currentByte & 0xc0) == 0xc0) {
            fillerByte = entryData[pcxOffset++];
            currentByte &= 0x3f;

            for (int fillerCount = 0; fillerCount < currentByte; fillerCount++) {
                image[levelImageOffset++] = fillerByte;
            }

        } else {
            image[levelImageOffset++] = currentByte;
        }
    }

    return image;
}

std::vector<Color> ResourceContainer::readPCXpalette(const std::string &name)
{
    EntrySize entrySize;
    char *entryData = m_loader.getEntryData( name, entrySize );
    int pcxOffset = entrySize - 768; // Palette is the last 768 bytes of the image.
    int paletteIndex = 0;
    std::vector<Color> palette(256);
    Color paletteItem;
    while (paletteIndex < 256)
    {
        paletteItem  = (                              0xFF << 24 );
        paletteItem += ( uint8_t( entryData[pcxOffset++] ) << 16 );
        paletteItem += ( uint8_t( entryData[pcxOffset++] ) << 8 );
        paletteItem += ( uint8_t( entryData[pcxOffset++] ) << 0 );
        palette[paletteIndex++] = paletteItem;
    }

    return palette;
}

std::vector<GobImage> ResourceContainer::readGOB(const std::string &name)
{
    EntrySize entrySize;
    char *entryData = m_loader.getEntryData( name, entrySize );
    char *entryDataBegin = entryData;
    GobHeader header;

    header.numberOfImages = readUint16(entryData);

    for(int i = 0; i < header.numberOfImages; ++i) {
        uint32_t value = readUint32(entryData);
        header.imageOffsets.push_back(value);
    }
//    header.imageOffsets.push_back(entrySize); // The GOB end offset

    std::vector<GobImage> gobs;
    for(int i = 0; i < header.numberOfImages; ++i) {
        GobImage gobImage;
//        std::vector<uint8_t>::size_type entrySize = header.imageOffsets[i+1] - header.imageOffsets[i];
        gobImage.buf_ = reinterpret_cast<const uint8_t*>( &entryDataBegin[ header.imageOffsets[i] ] );
        gobs.push_back(gobImage);
    }

    return gobs;
}

std::vector<ColorIndex> ResourceContainer::readGobImage(const GobImage &gobImage)
{

}

std::vector<char> ResourceContainer::readSMP(const std::string &name)
{

}

std::vector<char> ResourceContainer::readMOD(const std::string &name)
{

}

uint16_t ResourceContainer::readUint16(char* &data)
{
    uint16_t result;
    result  = ( uint8_t(*data++) << 0 );
    result += ( uint8_t(*data++) << 8 );
    return result;
}

uint32_t ResourceContainer::readUint32(char* &data)
{
    uint32_t result;
    result  = ( uint8_t(*data++) << 0  );
    result += ( uint8_t(*data++) << 8  );
    result += ( uint8_t(*data++) << 16 );
    result += ( uint8_t(*data++) << 24 );
    return result;
}
