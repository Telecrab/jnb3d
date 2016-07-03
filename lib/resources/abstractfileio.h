#ifndef ABSTRACTFILEIO_H
#define ABSTRACTFILEIO_H

#include <string>
#include <vector>

class AbstractFileIO
{
public:
    explicit AbstractFileIO(const std::string &fileName) {}
    AbstractFileIO(const AbstractFileIO &other) = delete;
    virtual ~AbstractFileIO() {}

    AbstractFileIO &operator=(const AbstractFileIO &other) = delete;

    virtual bool open() = 0;
    virtual bool isOpen() = 0;
    virtual std::string fileName() = 0;
    virtual bool reset() = 0;
    virtual int64_t size() = 0;
    virtual size_t read(char *data, size_t maxBytes) = 0;    
            std::vector<char> read(size_t maxBytes);
            std::vector<char> readAll(); // Don't use with huge files, please.

};

#endif // ABSTRACTFILEIO_H
