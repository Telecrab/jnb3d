#include "qtfileio.h"

QtFileIO::QtFileIO(const std::string &fileName)
    : AbstractFileIO(fileName)
    , m_file( QString::fromStdString(fileName) )
{}

bool QtFileIO::open()
{
    return m_file.open(QIODevice::ReadOnly);
}

bool QtFileIO::isOpen()
{
    return m_file.isOpen();
}

std::string QtFileIO::fileName()
{
    return m_file.fileName().toStdString();
}

bool QtFileIO::reset()
{
    return m_file.reset();
}

size_t QtFileIO::read(char *data, size_t maxBytes)
{
    return m_file.read(data, maxBytes);
}

int64_t QtFileIO::size()
{
    return m_file.size();
}
