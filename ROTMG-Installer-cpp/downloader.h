#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <string>

#include <windows.h>
#include <urlmon.h>
#include <WinInet.h>
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib,"wininet.lib")

#include "file.h"
#include "path.h"
#include "json.hpp"
#include "constants.h"
#include "resource.h"
#include "mbox.h"

class DownloadException : public std::exception
{
	const char* what() const throw() override
	{
		return "There was an error while downloading!";
	}
} downloadException;

//Where we store the information from the JSON UpdateInfo file.
struct FileInfo
{

	FileInfo(const std::string& name, const std::string& fileName, const std::string& downloadUrl, const std::string& md5)
	{
		Name = name;
		FileName = fileName;
		DownloadUrl = downloadUrl;
		MD5 = md5;
	}

	std::string Name;
	std::string FileName;
	std::string DownloadUrl;
	std::string MD5;


};

bool downloading;

class DownloadStatus : public IBindStatusCallback
{
public:


	STDMETHOD(OnStartBinding)(
		/* [in] */ DWORD dwReserved,
		/* [in] */ IBinding __RPC_FAR *pib)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(GetPriority)(
		/* [out] */ LONG __RPC_FAR *pnPriority)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(OnLowResource)(
		/* [in] */ DWORD reserved)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(OnProgress)(
		/* [in] */ ULONG ulProgress,
		/* [in] */ ULONG ulProgressMax,
		/* [in] */ ULONG ulStatusCode,
		/* [in] */ LPCWSTR wszStatusText);

	STDMETHOD(OnStopBinding)(
		/* [in] */ HRESULT hresult,
		/* [unique][in] */ LPCWSTR szError)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(GetBindInfo)(
		/* [out] */ DWORD __RPC_FAR *grfBINDF,
		/* [unique][out][in] */ BINDINFO __RPC_FAR *pbindinfo);

	STDMETHOD(OnDataAvailable)(
		/* [in] */ DWORD grfBSCF,
		/* [in] */ DWORD dwSize,
		/* [in] */ FORMATETC __RPC_FAR *pformatetc,
		/* [in] */ STGMEDIUM __RPC_FAR *pstgmed)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(OnObjectAvailable)(
		/* [in] */ REFIID riid,
		/* [iid_is][in] */ IUnknown __RPC_FAR *punk)
	{
		return E_NOTIMPL;
	}

	// IUnknown methods.  Note that IE never calls any of these methods, since
	// the caller owns the IBindStatusCallback interface, so the methods all
	// return zero/E_NOTIMPL.

	STDMETHOD_(ULONG, AddRef)()
	{
		return 0;
	}

	STDMETHOD_(ULONG, Release)()
	{
		return 0;
	}

	STDMETHOD(QueryInterface)(
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		return E_NOTIMPL;
	}
};

int progress = 0;

HRESULT DownloadStatus::GetBindInfo(
	/* [out] */ DWORD __RPC_FAR *grfBINDF,
	/* [unique][out][in] */ BINDINFO __RPC_FAR *pbindinfo)
{
	*grfBINDF = BINDF_NOWRITECACHE | BINDF_PRAGMA_NO_CACHE | BINDF_GETNEWESTVERSION | BINDF_DONTPUTINCACHE | BINDF_DONTUSECACHE;

	return S_OK;
}

HRESULT DownloadStatus::OnProgress(ULONG ulProgress, ULONG ulProgressMax,
	ULONG ulStatusCode, LPCWSTR wszStatusText)
{
	std::wstringstream ss;
	ss << ulStatusCode << std::endl;

	OutputDebugString(ss.str().c_str());

	
	switch (ulStatusCode)
	{

	case BINDSTATUS_COOKIE_SENT:
	case BINDSTATUS_CONNECTING:
		progress = 5;
		MBox::SetStatus2(L"Connecting...");
		break;

	case BINDSTATUS_BEGINDOWNLOADDATA:
		progress = 10;
		MBox::SetStatus2(L"Downloading...");
		break;

	case BINDSTATUS_DOWNLOADINGDATA:
	{
		if (int prog = ((static_cast<double>(ulProgress) / static_cast<double>(ulProgressMax)) * 100.0) > progress)
		{
			progress = prog;
		}
		std::wstringstream ss;
		ss << ulProgress / 1024 << "KB / " << ulProgressMax << "KB";
		MBox::SetStatus2(ss.str().c_str());
		break;
	}

	case BINDSTATUS_ENDDOWNLOADDATA:
		progress = 100;
		break;
	}

	MBox::SetProgress(progress);

	return S_OK;
}

//Downloads things.
class Downloader {
public:
	//Downloads a file from url and puts it into path.
	static void DownloadFile(const std::wstring& url, const std::wstring& path, const std::wstring& info)
	{
		//Check if the directory we download to exists. If it does not then create it.
		if (!Path::Exists(Path::GetDirectory(path)))
		{
			Path::Create(Path::GetDirectory(path));
		}

		//Check if the file we want to download already exists. If it does delete the file.
		if (File::Exists(path))
		{
			File::Delete(path);
		}
		std::wstring str(L"Updating ");
		str.append(info);
		str.append(L"...");

		MBox::Create(str, L"Connecting...");

		DownloadStatus ds;

		progress = 0;

		BOOL b = DeleteUrlCacheEntry(url.c_str());

		DWORD d = GetLastError();

		//Try to download the file. Get the error code in hResult.
		HRESULT hResult = URLDownloadToFile(NULL, url.c_str(), path.c_str(), 0, &ds);

		MBox::Destroy();

		//Checks if there is an error.
		switch (hResult)
		{
		case S_OK:
			if (!File::Exists(path))
			{
				throw downloadException;
			}
			return;

		case INET_E_CANNOT_CONNECT:
		case INET_E_CONNECTION_TIMEOUT:
		case INET_E_DOWNLOAD_FAILURE:
		case INET_E_RESOURCE_NOT_FOUND:
			MessageBox(NULL, L"Could not connect to the server.\r\nMake sure your internet connection is working.", L"Download Error", MB_OK);
			exit(1);
			break;

		default:
			throw downloadException;
		}

	}

	//Downloads the stuff from the json file.
	static void DownloadComponents(const nlohmann::json& json)
	{
		//For each entries.
		for (auto it = json.begin(); it != json.end(); ++it)
		{
			//Create a new FileInfo. Using JSON data.
			FileInfo fileInfo(it.key(), json[it.key()]["FileName"], json[it.key()]["DownloadUrl"], json[it.key()]["MD5"]);

			//If the file does not exist: Download it.
			if (!File::Exists(Path::Combine(FolderLocation, Converter::ToWString(fileInfo.FileName))))
			{
				DownloadFile(Converter::ToWString(fileInfo.DownloadUrl), Path::Combine(FolderLocation, Converter::ToWString(fileInfo.FileName)), Converter::ToWString(fileInfo.Name));
				continue;
			}

			//If the file exists check the MD5 hash. If it does not match; download the file.
			std::string md5hash(File::MD5Hash(Path::Combine(FolderLocation, Converter::ToWString(fileInfo.FileName))));
			if (_strcmpi(fileInfo.MD5.c_str(), md5hash.c_str()) != 0)
			{
				DownloadFile(Converter::ToWString(fileInfo.DownloadUrl), Path::Combine(FolderLocation, Converter::ToWString(fileInfo.FileName)), Converter::ToWString(fileInfo.Name));
			}

		}
	}


	Downloader() = delete;
};

#endif // DOWNLOADER_H