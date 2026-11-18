#pragma once

#include <string>
#include <vector>
#include <map>

class SaveData
{
public:
    SaveData();
    void SetFlag(const std::string &key, bool value);
    void ClearAllFlags();
    bool GetFlag(const std::string &key) const;

    void SetNumericFlag(const std::string &key, int value);
    void ClearAllNumericFlags();
    int GetNumericFlag(const std::string &key) const;

private:
    std::map<std::string, bool> flags;
    std::map<std::string, int> numericFlags;
};