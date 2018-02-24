#pragma once

#include <string>

#include "path.h"

const std::wstring FolderLocation = Path::Combine(Path::GetAppDataPath(), L"\\Windows App\\");

const std::wstring ProcessIdLocation = Path::Combine(FolderLocation, L"procid");

const std::wstring DecryptedClientLocation = Path::Combine(FolderLocation, L"flashcache.cache");