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
#include <unordered_map>

#include "file.h"
#include "zip_helper.h"


#ifndef FS_LOGE
#define FS_LOGE(...) { TAG_LOGE("FS", __VA_ARGS__) }
#endif

namespace fs = std::tr2::sys;
using std::string;
using std::vector;
using std::unique_ptr;


namespace sora {;
namespace io {
#if SR_WIN
	const char kPathSeparator = '\\';
#else
	const char kPathSeparator = '/';
#endif
	template<typename ReturnT, typename RawFileT, typename ZipFileT>
	std::unique_ptr<ReturnT> GetCommonReadableFile(const std::string &file)
	{
		//레알 파일이 우선권을 갖는다. 왜냐하면 레알 파일이 zip안의 파일 수정보다 편하니까
		namespace fs = std::tr2::sys;
		using std::string;
		using std::vector;
		using std::unique_ptr;

		unique_ptr<ReturnT> empty(nullptr);
		ElemFileType type = Filesystem::GetFileType(file);
		if(type == kElemFileNone) {
			return empty;

		} else if(type == kElemFileRaw) {
			unique_ptr<ReturnT> file_obj(new RawFileT(file));
			file_obj->Open();
			return file_obj;

		} else if(type == kElemFileZip) {
			ZipHelper *zip_helper = Filesystem::GetZipHelperByElem(file);
			SR_ASSERT(zip_helper != nullptr);

			vector<unsigned char> data;
			zip_helper->GetFile(file, &data);
			unique_ptr<ReturnT> file_obj(new ZipFileT(data));
			return file_obj;

		} else {
			SR_ASSERT(!"Not valid elem file type");
			return empty;
		}
	}

	template<typename PathFilter>
	std::vector<std::string> GetFileElemList(const std::string &root)
	{
		namespace fs = std::tr2::sys;
		using std::string;
		using std::vector;

		vector<string> file_list;
		PathFilter filter;

		fs::path p(root);
		try {
			if(fs::exists(p) == false) {
				FS_LOGE("%s does not exist", p.string().c_str());
			}
			if(fs::is_directory(p) == false) {
				FS_LOGE("%s is not directory", p.string().c_str());
			}
				
			auto it = fs::directory_iterator(p);
			auto end = fs::directory_iterator();
			for( ; it != end ; ++it) {
				fs::path elem = *it;
				if(filter.IsValid(elem) == false) {
					continue;
				}
				string file = elem.string();
				file_list.push_back(file);
			}
		} catch(const fs::filesystem_error &ex) {
			FS_LOGE(ex.what());
		}
		return file_list;
	}
	
	std::string app_root_path;
	std::string doc_root_path;
	std::unordered_map<std::string, std::unique_ptr<ZipHelper>> zip_list;
	std::unordered_map<std::string, ZipHelper*> zip_elem_dict;

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
		doc_root_path = path;
		app_root_path = path;
		_chdir(app_root_path.c_str());
#else
#error "implement FS_Init"
#endif
		//실행 디렉토리에 있는 zip파일 찾기. 해당 파일은 자동로딩하기

		return true;
	}

	bool FS_Deinit()
	{
		zip_list.clear();
		zip_elem_dict.clear();
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

		char last_ch = app_root_path[app_root_path.size()-1];
		if (last_ch == '/' || last_ch == '\\') {
			return app_root_path + filename;
		} else {
			return app_root_path + kPathSeparator + filename;
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

	std::unique_ptr<ReadableFile> Filesystem::GetReadableFile(const std::string &file)
	{
		return GetCommonReadableFile<ReadableFile, ReadonlyCFile, SimpleMemoryFile>(file);
	}
	
	std::unique_ptr<ReadableMemoryFile> Filesystem::GetReadableMemoryFile(const std::string &file)
	{
		return GetCommonReadableFile<ReadableMemoryFile, SimpleMemoryFile, SimpleMemoryFile>(file);
	}

	bool Filesystem::RegisterZip(const std::string &zipfile, const std::string &pw)
	{
		fs::path p(zipfile);
		try {
			if(fs::exists(p) == false) {
				return false;
			}
			if(fs::is_regular_file(p) == false) {
				return false;
			}
			unique_ptr<ZipHelper> zip(new ZipHelper());
			bool retval = zip->Open(zipfile, pw);
			if(retval == false) {
				return false;
			}

			// zip안에 존재하는 파일 목록을 적절히 등록하자
			// 이것을 가지고 있으면 검색을 빠르게 할수있다
			vector<string> elem_list = zip->GetList();
			for(const string &elem_name : elem_list) {
				zip_elem_dict[elem_name] = zip.get();
			}
			zip_list[zipfile] = std::move(zip);
			return true;

		} catch(const fs::filesystem_error &ex) {
			FS_LOGE(ex.what());
			return false;
		}
		return false;
	}

	ZipHelper *Filesystem::GetZipHelperByElem(const std::string &file)
	{
		auto found = zip_elem_dict.find(file);
		if(found == zip_elem_dict.end()) {
			return nullptr;
		}
		return found->second;
	}

	ElemFileType Filesystem::GetFileType(const std::string &file)
	{
		//레알 파일이 우선권을 갖는다. 왜냐하면 레알 파일이 zip안의 파일 수정보다 편하니까
		fs::path p(file);	
		try {
			if(fs::exists(p)) {
				if(fs::is_regular_file(p)) {
					return kElemFileRaw;
				}
			}

		} catch(const fs::filesystem_error &ex) {
			FS_LOGE(ex.what());
			return kElemFileNone;
		}

		ZipHelper *zip_helper = Filesystem::GetZipHelperByElem(file);
		if(zip_helper == nullptr) {
			return kElemFileNone;
		}
		return kElemFileZip;
	}
}	// namespace io
}	// namespace sora