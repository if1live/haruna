// Ŭnicode please
#pragma once

#include <cstdio>
#include <string>

#include <filesystem>

namespace sora {;
namespace io {

	const int kMaxPathLength = 512;

	bool FS_Init();
	bool FS_Deinit();

	class Filesystem {
	public:
		static std::string GetExtension(const std::string &str);
		static std::string GetAppPath(const std::string &str);

		static std::vector<std::string> GetFileList(const std::string &root);
		static std::vector<std::string> GetDirList(const std::string &root);
		static std::vector<std::string> GetAllList(const std::string &root);
	};

	template<typename PathFilter>
	std::vector<std::string> GetFileElemList(const std::string &root)
	{
#ifndef FS_LOGE
#define FS_LOGE(...) { TAG_LOGE("FS", __VA_ARGS__) }
#endif
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

#undef FS_LOGE
	}
}
}
