#include "abstractfileio.h"


std::vector<char> AbstractFileIO::readAll()
{
    std::vector<char> result( size() );
    int64_t fileSize = size();
    int64_t bytesRead = 1;
    int64_t totalBytesRead = 0;
    char* buf = result.data();
    while (totalBytesRead < fileSize && bytesRead != 0) {
        bytesRead = read( buf, (fileSize - totalBytesRead) );
        totalBytesRead += bytesRead;
        buf += bytesRead;
    }
    return result;
}
