#include "abstractfileio.h"


std::vector<char> AbstractFileIO::read(size_t maxBytes)
{
    std::vector<char> result( maxBytes );
    int64_t bytesRead = 1;
    int64_t totalBytesRead = 0;
    char* buf = result.data();
    while (totalBytesRead < maxBytes && bytesRead != 0) {
        bytesRead = read( buf, (maxBytes - totalBytesRead) );
        totalBytesRead += bytesRead;
        buf += bytesRead;
    }
    return result;
}

std::vector<char> AbstractFileIO::readAll()
{
    if( size() > 0) return read( size() );
    else return std::vector<char>();
}
