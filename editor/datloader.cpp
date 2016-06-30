#include "datloader.h"

//#include <fstream>
//#include <iterator>
//#include <iostream>
//#include <cstdlib>
#include <QFile>

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
    QFile level("jumpbump.dat");
    level.open(QIODevice::ReadOnly);
    if(!level.isOpen()) {
        qFatal("Cannot open 'jumpbump.dat'.");
        return;
    }
    std::string data = level.readAll().toStdString(); // Cashing the file in memory for later use
    m_fileData.assign( data.begin(), data.end() );
    level.reset();

    // Reading .dat directory
    int32_t numEntries = 0;
    level.read( reinterpret_cast<char*>(&numEntries), sizeof(int32_t) );
    for (int32_t entryNumber = 0; entryNumber < numEntries; entryNumber++)
    {
        ArchiveEntry entry;
        QByteArray tmp = level.read( sizeof(int8_t) * 12 );

        for(int i = tmp.size() - 1; i >= 0; i--) {
            if( tmp.at(i) != 0 ) {
                tmp.resize(i + 1);
                break;
            }
        }

        entry.name = tmp.toStdString();
        level.read( reinterpret_cast<char*>(&entry.offset), sizeof(uint32_t) * 2 ); // Offset and size
        m_archiveContents.push_back(entry);
        m_archiveIndex[entry.name] = entryNumber;
    }
}

std::vector<ArchiveEntry> DATloader::archiveContents()
{
    return m_archiveContents;
}

std::vector<char> DATloader::getEntryData(const std::string &name)
{
    if( m_archiveIndex.find(name) == m_archiveIndex.end() ) return std::vector<char>();

    const ArchiveEntry &entry = m_archiveContents[ m_archiveIndex.at(name) ];
    return std::vector<char>( &m_fileData[entry.offset], &m_fileData[entry.offset + entry.size] );
}
