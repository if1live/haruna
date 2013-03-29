// Ŭnicode please
#pragma once

#include <cstdio>
#include <string>

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
}
}
