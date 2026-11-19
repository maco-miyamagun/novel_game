// 【save_data】 シナリオ用の真偽値・数値フラグを保持する。
#pragma once

#include <string>
#include <vector>
#include <map>

// クラス宣言: 公開操作と内部リソースをまとめる。
class SaveData
{
public:
    SaveData();
// 指定キーの真偽値フラグを設定または上書きする。
    void SetFlag(const std::string &key, bool value);
// 登録済みの真偽値フラグをすべて削除する。
    void ClearAllFlags();
// 指定キーの真偽値フラグを返す。未登録時はfalse。
    bool GetFlag(const std::string &key) const;

// 指定キーの数値フラグを設定または上書きする。
    void SetNumericFlag(const std::string &key, int value);
// 登録済みの数値フラグをすべて削除する。
    void ClearAllNumericFlags();
// 指定キーの数値フラグを返す。未登録時は0。
    int GetNumericFlag(const std::string &key) const;

private:
// 真偽値フラグをキー名で保持する。
    std::map<std::string, bool> flags;
// 数値フラグをキー名で保持する。
    std::map<std::string, int> numericFlags;
};