#include "save_data.hpp"

SaveData::SaveData()
{
    flags.clear();
    numericFlags.clear();
}

void SaveData::SetFlag(const std::string &key, bool value)
{
    flags[key] = value;
}

void SaveData::ClearAllFlags()
{
    flags.clear();
}

bool SaveData::GetFlag(const std::string &key) const
{
    auto it = flags.find(key);
    if (it != flags.end())
    {
        return it->second;
    }
    return false;
}

void SaveData::SetNumericFlag(const std::string &key, int value)
{
    numericFlags[key] = value;
}

void SaveData::ClearAllNumericFlags()
{
    numericFlags.clear();
}

int SaveData::GetNumericFlag(const std::string &key) const
{
    auto it = numericFlags.find(key);
    if (it != numericFlags.end())
    {
        return it->second;
    }
    return 0;
}