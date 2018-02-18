#pragma once

#include <string>

#include "path.h"

constexpr auto UpdateInfoUrl = L"https://github.com/mads256c/mads256c.github.io/raw/master/clientUpdateInfo.json";

constexpr auto MovieKey = L"Software\\Macromedia\\FlashPlayer";

std::wstring FolderLocation = Path::Combine(Path::GetAppDataPath(), L"\\Windows App");

std::wstring UpdateInfoLocation = Path::Combine(FolderLocation, L"clientUpdateInfo.json");