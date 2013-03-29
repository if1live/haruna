// Ŭnicode please
#include "stdafx.h"
#include "file.h"
#include "filesystem.h"

#if SR_WIN
#include <io.h>
#include <fcntl.h>
#endif

#include <filesystem>

namespace fs = std::tr2::sys;

namespace sora {;
namespace io {
	int SeekOriginTypeToCStyle(SeekOriginType in)
	{
		switch(in) {
		case kSeekCurr:
			return SEEK_CUR;
		case kSeekEnd:
			return SEEK_END;
		case kSeekStart:
			return SEEK_SET;
		default:
			SR_ASSERT(!"not valid seeek origin type");
			return SEEK_SET;
		}
	}

	ReadonlyCFile::ReadonlyCFile(const std::string &file)
		: file_(nullptr), buffer_(nullptr), filename_(file)
	{
	}
	ReadonlyCFile::~ReadonlyCFile()
	{
		if(IsOpened()) {
			Close();
		}
	}

	bool ReadonlyCFile::Open()
	{
		CFileHelper<ReadonlyCFile> helper(this);
		return helper.Open();
	}
	bool ReadonlyCFile::Close()
	{
		if (buffer_ != nullptr) {
			free(buffer_);
			buffer_ = nullptr;
		}

		CFileHelper<ReadonlyCFile> helper(this);
		return helper.Close();
	}
	bool ReadonlyCFile::IsOpened() const
	{
		CFileHelper<ReadonlyCFile> helper(const_cast<ReadonlyCFile*>(this));
		return helper.IsOpened();
	}

	int ReadonlyCFile::Read(void *buf, int size)
	{
		if(IsOpened() == false) {
			return -1;
		}
		return fread(buf, 1, size, file_);
	}

	bool ReadonlyCFile::Seek(int offset, SeekOriginType origin)
	{
		if(IsOpened() == false) {
			return false;
		}
		int c_style_origin = SeekOriginTypeToCStyle(origin);
		int retval = fseek(file_, offset, c_style_origin);
		return (retval == 0);
	}

	const void *ReadonlyCFile::GetBuffer()
	{
		if (buffer_ == nullptr) {
			int length = GetLength();
			// 1바이트 더 할당해서 \0로 넣어놓자. 안정성 확보를 위한 목적
			// 나중에 마지막 널을 넣는지 안넣는지는 옵션으로 빼는게 나을듯
			buffer_ = malloc(length + 1);

			long curr_pos = ftell(file_);
			fseek(file_, 0, SEEK_SET);
			fread(buffer_, length, 1, file_);
			fseek(file_, curr_pos, SEEK_SET);

			((char*)buffer_)[length] = '\0';
		}
		return buffer_;
	}

	int ReadonlyCFile::GetLength() const
	{
		if(IsOpened() == false) {
			fs::path p(filename_);
			int size = fs::file_size(p);
			return size;

		} else {
			int curr_pos = ftell(file_);
			fseek(file_, 0, SEEK_END);
			int length = ftell(file_);
			fseek(file_, curr_pos, SEEK_SET);
			return length;
		}
	}

	int ReadonlyCFile::GetRemainLength() const
	{
		if(IsOpened() == false) {
			return 0;
		}
		int length = GetLength();
		int curr_pos = ftell(file_);
		return length - curr_pos;
	}

	/////////////////////////////////////////

	WriteonlyCFile::WriteonlyCFile(const std::string &file)
		: file_(nullptr), filename_(file)
	{
	}
	WriteonlyCFile::~WriteonlyCFile()
	{
		if(IsOpened()) {
			Close();
		}
	}

	bool WriteonlyCFile::Open()
	{
		CFileHelper<WriteonlyCFile> helper(this);
		return helper.Open();
	}
	bool WriteonlyCFile::Close()
	{
		CFileHelper<WriteonlyCFile> helper(this);
		return helper.Close();
	}
	bool WriteonlyCFile::IsOpened() const
	{
		CFileHelper<WriteonlyCFile> helper(const_cast<WriteonlyCFile*>(this));
		return helper.IsOpened();
	}
	int WriteonlyCFile::Write(const void *buf, int size)
	{
		if(IsOpened() == false) {
			return -1;
		}
		int retval = fwrite(buf, 1, size, file_);
		return retval;
	}

	///////////////////////////////////////

	MemoryFile::MemoryFile(const std::string &file)
		: start(nullptr),
		end(nullptr),
		curr(nullptr),
		data(nullptr),
		filename_(file)
	{
	}

	MemoryFile::~MemoryFile() 
	{
		Close();
	}

	bool MemoryFile::Close() 
	{
		if(data != NULL) {
			free(data);
			start = nullptr;
			curr = nullptr;
			end = nullptr;
			data = nullptr;
			return true;
		}
		return false;
	}

	int MemoryFile::Read(void *buf, int size) 
	{
		int curr_pos = curr - start;
		int length = GetLength();
		int remain = length - curr_pos;
		if(remain <= 0) {
			return 0;
		}
		
		int write_size = (remain < size) ? remain : size;
		std::copy(curr, curr + write_size, (unsigned char*)buf);
		curr += write_size;
		return write_size;
	}

	bool MemoryFile::Open() 
	{
		int flag = O_RDONLY;
#if SR_WIN
		flag |= O_BINARY;
#endif
		int fd = _open(filename_.data(), flag);
		SR_ASSERT(fd != -1 && "file is not exist");
		if (fd == -1) {
			return false;
		}

		// overflow 가능성을 조금이라도 낮추기 위해서 1byte 추가 할당
		// library하고 꼬여서 자구 죽네... 다른 방법으로 얻어야될듯
		// struct stat s;
		// int result = fstat(fd, &s);
		// return s.st_size;
		int curr_pos = _tell(fd);
		_lseek(fd, 0, SEEK_END);
		int length = tell(fd);
		lseek(fd, curr_pos, SEEK_SET);

		start = static_cast<unsigned char*>(malloc(length + 1)); 
		data = start;
		_read(fd, start, length);

		curr = start;
		end = curr + length;
		// 조금더 안전하게 하기 위한 용도
		*end = '\0';

		_close(fd);
		return true;
	}

	bool MemoryFile::Seek(int offset, SeekOriginType origin)
	{
		int remain_size = GetRemainLength();
		int size = GetLength();

		if(origin == kSeekCurr) {
			int curr_pos = (curr - start);
			if(offset < -curr_pos) {
				return false;
			}
			if(offset > remain_size) {
				return false;
			}
			curr = offset + curr;
			return true;

		} else if(origin == kSeekStart) {
			if(offset < 0) {
				return false;
			}
			if(offset > size) {
				return false;
			}
			curr = start + offset;
			return true;

		} else if(origin == kSeekEnd) {
			if(offset > 0) {
				return false;
			}
			if(offset > size) {
				return false;
			}
			curr = end + offset;
			return true;
		} else {
			SR_ASSERT(!"not valid seek origin type");
			return false;
		}
	}

	int MemoryFile::GetLength() const
	{
		if(start == nullptr) {
			fs::path p(filename_);
			return fs::file_size(p);
		} else {
			return end - start; 
		}
	}

}	// namespace io
}	// namespace sora