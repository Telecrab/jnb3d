#ifndef ABSTRACTFILEIO_H
#define ABSTRACTFILEIO_H

#include <string>
#include <vector>

class AbstractFileIO
{
public:
    AbstractFileIO(const std::string &fileName) {}
    virtual ~AbstractFileIO() {}

    virtual bool open() = 0;
    virtual bool isOpen() = 0;
    virtual bool reset() = 0;
    virtual std::vector<char> readAll(); // Don't use with huge files, please.
    virtual size_t read(char *data, size_t) = 0;
    virtual int64_t size() = 0;

};

#endif // ABSTRACTFILEIO_H
