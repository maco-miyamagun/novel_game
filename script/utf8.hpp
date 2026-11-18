#pragma once
#include <string>

int UTF8Length(const std::string &str);

std::string GetUTF8Substring(
    const std::string &str,
    int start,
    int length);