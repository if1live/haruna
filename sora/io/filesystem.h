// Ŭnicode please
#pragma once

#include <cstdio>
#include <string>
#include <vector>

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
}
}
