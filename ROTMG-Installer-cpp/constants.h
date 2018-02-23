#pragma once

#include <string>

#include "path.h"

//The UpdateInfo file is the JSON that gets parsed.
constexpr auto UpdateInfoUrl = L"https://github.com/mads256c/mads256c.github.io/raw/master/clientUpdateInfo.json";

//Where the recent movies are stored in regestry current user.
constexpr auto MovieKey = L"Software\\Macromedia\\FlashPlayer";

//Where we store all our files.
const std::wstring FolderLocation = Path::Combine(Path::GetAppDataPath(), L"\\Windows App\\");

const std::wstring TrustedFolderLocation = Path::Combine(Path::GetAppDataPath(), L"Macromedia\\Flash Player\\#Security\\FlashPlayerTrust\\");

const std::wstring TrustedFile = Path::Combine(TrustedFolderLocation, L"default.cfg");



//Where the UpdateInfo is stored temporarily.
const std::wstring UpdateInfoLocation = Path::Combine(FolderLocation, L"clientUpdateInfo.json");