#ifndef DATLOADER_H
#define DATLOADER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

using EntryOffset   = uint32_t;
using EntrySize     = uint32_t;

struct ArchiveEntry
{
    std::string name;
    EntryOffset offset;
    EntrySize   size;
};

class AbstractFileIO;

class DATloader
{
public:
    DATloader();

    void loadArchive(std::shared_ptr<AbstractFileIO> file);
    std::vector<ArchiveEntry> archiveContents();
    char *getEntryData(const std::string & name, EntrySize &size);

private:
    std::vector<char> m_fileData;
    std::vector<ArchiveEntry> m_archiveContents;
    std::unordered_map<std::string, std::vector<char>::size_type> m_archiveIndex;
};

#endif // DATLOADER_H
