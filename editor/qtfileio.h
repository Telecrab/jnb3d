#ifndef QTFILEIO_H
#define QTFILEIO_H

#include "../lib/resources/abstractfileio.h"
#include <QFile>

class QtFileIO : public AbstractFileIO
{
public:
    explicit QtFileIO(const std::string &fileName);

public:
    virtual bool open() override;
    virtual bool isOpen() override;
    virtual std::string fileName() override;
    virtual bool reset() override;
    virtual size_t read(char *data, size_t maxBytes) override;
    virtual int64_t size() override;

    QFile m_file;
};

#endif // QTFILEIO_H
