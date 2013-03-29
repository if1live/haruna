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
		: file_(nullptr), filename_(file)
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
		if(buffer_.size() != 0) {
			buffer_.resize(0);
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
		if (buffer_.size() == 0) {
			int length = GetLength();
			// 1바이트 더 할당해서 \0로 넣어놓자. 안정성 확보를 위한 목적
			// 나중에 마지막 널을 넣는지 안넣는지는 옵션으로 빼는게 나을듯
			buffer_.resize(length + 1);

			long curr_pos = ftell(file_);
			fseek(file_, 0, SEEK_SET);
			fread(buffer_.data(), length, 1, file_);
			fseek(file_, curr_pos, SEEK_SET);

			buffer_[length] = '\0';
		}
		return buffer_.data();
	}

	int ReadonlyCFile::GetLength() const
	{
		if(IsOpened() == false) {
			fs::path p(filename_);
			int size = static_cast<int>(fs::file_size(p));
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

	SimpleMemoryFile::SimpleMemoryFile(const std::string &file)
		: curr_(nullptr), filename_(file)
	{
	}

	SimpleMemoryFile::~SimpleMemoryFile() 
	{
		Close();
	}

	bool SimpleMemoryFile::Close() 
	{
		if(data_.size() != 0) {
			curr_ = nullptr;
			data_.resize(0);
			return true;
		}
		return false;
	}

	int SimpleMemoryFile::Read(void *buf, int size) 
	{
		int curr_pos = curr_ - start();
		int length = GetLength();
		int remain = length - curr_pos;
		if(remain <= 0) {
			return 0;
		}
		
		int write_size = (remain < size) ? remain : size;
		std::copy(curr_, curr_ + write_size, (unsigned char*)buf);
		curr_ += write_size;
		return write_size;
	}

	bool SimpleMemoryFile::Open() 
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
		int length = _tell(fd);
		_lseek(fd, curr_pos, SEEK_SET);

		data_.resize(length + 1);
		_read(fd, data_.data(), length);

		curr_ = start();
		// 조금더 안전하게 하기 위한 용도
		data_[data_.size()-1] = '\0';

		_close(fd);
		return true;
	}

	bool SimpleMemoryFile::Seek(int offset, SeekOriginType origin)
	{
		int remain_size = GetRemainLength();
		int size = GetLength();

		if(origin == kSeekCurr) {
			int curr_pos = (curr_ - start());
			if(offset < -curr_pos) {
				return false;
			}
			if(offset > remain_size) {
				return false;
			}
			curr_ = offset + curr_;
			return true;

		} else if(origin == kSeekStart) {
			if(offset < 0) {
				return false;
			}
			if(offset > size) {
				return false;
			}
			curr_ = start() + offset;
			return true;

		} else if(origin == kSeekEnd) {
			if(offset > 0) {
				return false;
			}
			if(offset > size) {
				return false;
			}
			curr_ = end() + offset;
			return true;
		} else {
			SR_ASSERT(!"not valid seek origin type");
			return false;
		}
	}

	int SimpleMemoryFile::GetLength() const
	{
		if(data_.size() == 0) {
			fs::path p(filename_);
			return static_cast<int>(fs::file_size(p));
		} else {
			return data_.size() - 1; 
		}
	}

}	// namespace io
}	// namespace sora