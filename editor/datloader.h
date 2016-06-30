#ifndef DATLOADER_H
#define DATLOADER_H

#include <vector>
#include <string>
#include <unordered_map>

struct ArchiveEntry
{
    std::string name;
    uint32_t offset;
    uint32_t size;
};

class DATloader
{
public:
    DATloader();

    void loadArchive(const std::string &filename);
    std::vector<ArchiveEntry> archiveContents();
    std::vector<char> getEntryData(const std::string & name);

private:
    std::vector<char> m_fileData;
    std::vector<ArchiveEntry> m_archiveContents;
    std::unordered_map<std::string, size_t> m_archiveIndex;
};

#endif // DATLOADER_H
