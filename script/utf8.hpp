// 【utf8】 UTF-8文字列の文字数と部分文字列を扱う。
#pragma once
#include <string>

// UTF-8文字列を文字単位で数え、文字数を返す。
int UTF8Length(const std::string &str);

// UTF-8境界を維持したまま指定範囲を切り出す。
std::string GetUTF8Substring(
    const std::string &str,
    int start,
    int length);