#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <iostream>

std::ostream& operator <<(std::ostream &stream, const QString &str)
{
    stream << str.toStdString();
    return stream;
}

#endif
