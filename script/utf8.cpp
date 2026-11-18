#include "utf8.hpp"

int UTF8Length(const std::string &str)
{
    int count = 0;

    for (unsigned char c : str)
    {
        if ((c & 0xC0) != 0x80)
            count++;
    }

    return count;
}

std::string GetUTF8Substring(
    const std::string &str,
    int start,
    int length)
{
    int currentChar = 0;
    size_t byteStart = 0;
    size_t byteEnd = str.size();

    for (size_t i = 0; i < str.size();)
    {
        if (currentChar == start)
            byteStart = i;

        if (currentChar == start + length)
        {
            byteEnd = i;
            break;
        }

        unsigned char c = str[i];

        if ((c & 0x80) == 0)
            i += 1;
        else if ((c & 0xE0) == 0xC0)
            i += 2;
        else if ((c & 0xF0) == 0xE0)
            i += 3;
        else
            i += 4;

        currentChar++;
    }

    if (start >= currentChar)
        return "";

    return str.substr(byteStart, byteEnd - byteStart);
}