// Ŭnicode please
#include "stdafx.h"
#include "filesystem.h"

// for filesystem
#include <Windows.h>
#include <io.h>
#include <tchar.h>
#include <direct.h>
#define tell _tell
#define lseek _lseek

#include <filesystem>

namespace fs = std::tr2::sys;
using std::string;
using std::vector;

#ifndef FS_LOGE
#define FS_LOGE(...) { TAG_LOGE("FS", __VA_ARGS__) }
#endif

namespace sora {;
namespace io {
#if SR_WIN
	const char kPathSeparator = '\\';
#else
	const char kPathSeparator = '/';
#endif
	
	std::string g_app_root_path;
	std::string g_doc_root_path;

	bool FS_Init()
	{
		/*
		// 윈도우에서 실행프로그램이 있는 경로 얻기
		// 이것을 이용해서 경로 변경후 파일을 열자
		// 라고 하기에 디버깅 경로만 설정이 있으니까 이거 대신 아래를 사용한다
		TCHAR path[MAX_PATH];
		::GetModuleFileName(0, path, _MAX_PATH);
		TCHAR* p = _tcsrchr(path, '\\');
		path[p - path] = 0;
		doc_root_path = path;
		app_root_path = path;
		chdir(app_root_path.c_str());
		return 1;
		*/
#if SR_WIN
		TCHAR path[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, path);
		g_doc_root_path = path;
		g_app_root_path = path;
		_chdir(g_app_root_path.c_str());
#else
#error "implement FS_Init"
#endif
		return true;
	}

	bool FS_Deinit()
	{
		return true;
	}

	std::string Filesystem::GetExtension(const std::string &str) 
	{
		fs::path p(str);
		return p.extension();
	}

	std::string Filesystem::GetAppPath(const std::string &str) 
	{
		std::string filename = str;
		for(size_t i = 0 ; i < filename.length() ; ++i) {
			if(filename[i] == '/') {
				filename[i] = kPathSeparator;
			}
		}

		char last_ch = g_app_root_path[g_app_root_path.size()-1];
		if (last_ch == '/' || last_ch == '\\') {
			return g_app_root_path + filename;
		} else {
			return g_app_root_path + kPathSeparator + filename;
		}
	}

	struct AbstractPathFilter {
		AbstractPathFilter() {}
		virtual ~AbstractPathFilter() {}
		virtual bool IsValid(const fs::path &elem) const = 0;
	};
	struct FileFilter : public AbstractPathFilter {
		FileFilter() {}
		virtual ~FileFilter() {}
		bool IsValid(const fs::path &elem) const {
			return (fs::is_regular_file(elem));
		}
	};
	struct DirFilter : public AbstractPathFilter {
		DirFilter() {}
		virtual ~DirFilter() {}
		bool IsValid(const fs::path &elem) const {
			return (fs::is_directory(elem));
		}
	};
	struct NullFilter : public AbstractPathFilter {
		NullFilter() {}
		virtual ~NullFilter() {}
		bool IsValid(const fs::path &elem) const {
			return true;
		}
	};

	std::vector<std::string> Filesystem::GetFileList(const std::string &root)
	{
		return GetFileElemList<FileFilter>(root);
	}
	std::vector<std::string> Filesystem::GetDirList(const std::string &root)
	{
		return GetFileElemList<DirFilter>(root);
	}

	std::vector<std::string> Filesystem::GetAllList(const std::string &root)
	{
		return GetFileElemList<NullFilter>(root);
	}

}	// namespace io
}	// namespace sora