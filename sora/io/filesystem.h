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
		static int GetFileSize(int fd);
		static int GetFileSize(FILE *file);
		static std::string GetExtension(const std::string &str);
		static std::string GetAppPath(const std::string &str);
	};

}
}
