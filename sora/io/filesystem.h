// Ŭnicode please
#pragma once

#include <cstdio>
#include <string>
#include <vector>

#include <filesystem>
#include "file.h"
#include "zip_helper.h"

#ifndef FS_LOGE
#define FS_LOGE(...) { TAG_LOGE("FS", __VA_ARGS__) }
#endif

namespace sora {;
namespace io {
	const int kMaxPathLength = 512;

	typedef enum {
		kElemFileNone,
		kElemFileRaw,
		kElemFileZip,
	} ElemFileType;

	bool FS_Init();
	bool FS_Deinit();

	class ReadableFile;
	class ReadableMemoryFile;
	class ZipHelper;

	class Filesystem {
	public:
		static std::string GetExtension(const std::string &str);
		static std::string GetAppPath(const std::string &str);

		static std::vector<std::string> GetFileList(const std::string &root);
		static std::vector<std::string> GetDirList(const std::string &root);
		static std::vector<std::string> GetAllList(const std::string &root);

		static std::unique_ptr<ReadableFile> GetReadableFile(const std::string &file);
		static std::unique_ptr<ReadableMemoryFile> GetReadableMemoryFile(const std::string &file);

		static bool RegisterZip(const std::string &zipfile, const std::string &pw);
		static bool RegisterZip(const std::string &zipfile) {
			return Filesystem::RegisterZip(zipfile, std::string(""));
		}

		static ElemFileType GetFileType(const std::string &file);
		static ZipHelper *GetZipHelperByElem(const std::string &file);
	};

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
}
}
