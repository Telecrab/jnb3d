#include "datloader.h"

//#include <fstream>
//#include <iterator>
//#include <iostream>
//#include <cstdlib>

#include "abstractfileio.h"

DATloader::DATloader()
{
    // //http://stackoverflow.com/questions/4761529/efficient-way-of-reading-a-file-into-an-stdvectorchar
    // std::ifstream file("jumpbump.dat", std::ios::binary);
    // if(!file) {
    //     std::cout << "Couldn't open 'jumpbump.dat'!" << std::endl;
    //     std::exit(EXIT_FAILURE);
    // }
    //
    // file.seekg(0, std::ios::end);
    // std::vector<char>::size_type fileSize = file.tellg();
    // file.seekg(0, std::ios::beg);
    //
    // m_fileData.reserve(fileSize);
    // m_fileData.assign(std::istreambuf_iterator<char>(file),
    //                     std::istreambuf_iterator<char>());
    //
    // std::cout << m_fileData.size() << std::endl;

}

void DATloader::loadArchive(std::shared_ptr<AbstractFileIO> file)
{
    file->open();
    if(!file->isOpen()) {
        std::string message = "Cannot open '" + fileName + "'.";
        qFatal( message.c_str() );
        return;
    }
    m_fileData = file->readAll(); // Cashing the file in memory for later use
    file->reset();

    // Reading .dat directory
    int32_t numEntries = 0;
    file->read( reinterpret_cast<char*>(&numEntries), sizeof(int32_t) );
    for (int32_t entryNumber = 0; entryNumber < numEntries; entryNumber++)
    {
        ArchiveEntry entry;
        QByteArray tmp = file->read( sizeof(int8_t) * 12 );

        for(int i = tmp.size() - 1; i >= 0; i--) {
            if( tmp.at(i) != 0 ) {
                tmp.resize(i + 1);
                break;
            }
        }

        entry.name = tmp.toStdString();
        file->read( reinterpret_cast<char*>(&entry.offset), sizeof(uint32_t) * 2 ); // Offset and size
        m_archiveContents.push_back(entry);
        m_archiveIndex[entry.name] = entryNumber;
    }
}

std::vector<ArchiveEntry> DATloader::archiveContents()
{
    return m_archiveContents;
}

char *DATloader::getEntryData(const std::string &name, EntrySize &size)
{
    if( m_archiveIndex.find(name) == m_archiveIndex.end() ) {
        size = 0;
        return nullptr;
    }

    const ArchiveEntry &entry = m_archiveContents[ m_archiveIndex[name] ];
    size = entry.size;
    return &m_fileData[entry.offset];
}
