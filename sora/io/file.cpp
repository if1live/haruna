// Ŭnicode please
#include "stdafx.h"
#include "file.h"
#include "filesystem.h"

namespace sora {;
namespace io {
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
		if (file_ == nullptr) {
			return -1;
		}
		return fread(buf, size, 1, file_);
	}

	int ReadonlyCFile::Seek(int offset, int origin)
	{
		if (file_ == nullptr) {
			return -1;
		}
		return fseek(file_, offset, origin);
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
		if (file_ == nullptr) {
			return 0;
		}
		return Filesystem::GetFileSize(file_);
	}

	int ReadonlyCFile::GetRemainLength() const
	{
		if (file_ == nullptr) {
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
		if (file_ == nullptr) {
			return -1;
		}
		return fwrite(buf, size, 1, file_);
	}
}	// namespace io
}	// namespace sora