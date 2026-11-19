// 【save_data】 シナリオ用の真偽値・数値フラグを保持する。
#include "save_data.hpp"

SaveData::SaveData()
{
    flags.clear();
    numericFlags.clear();
}

// 指定キーの真偽値フラグを設定または上書きする。
void SaveData::SetFlag(const std::string &key, bool value)
{
    flags[key] = value;
}

// 登録済みの真偽値フラグをすべて削除する。
void SaveData::ClearAllFlags()
{
    flags.clear();
}

// 指定キーの真偽値フラグを返す。未登録時はfalse。
bool SaveData::GetFlag(const std::string &key) const
{
    auto it = flags.find(key);
    if (it != flags.end())
    {
        return it->second;
    }
    return false;
}

// 指定キーの数値フラグを設定または上書きする。
void SaveData::SetNumericFlag(const std::string &key, int value)
{
    numericFlags[key] = value;
}

// 登録済みの数値フラグをすべて削除する。
void SaveData::ClearAllNumericFlags()
{
    numericFlags.clear();
}

// 指定キーの数値フラグを返す。未登録時は0。
int SaveData::GetNumericFlag(const std::string &key) const
{
    auto it = numericFlags.find(key);
    if (it != numericFlags.end())
    {
        return it->second;
    }
    return 0;
}