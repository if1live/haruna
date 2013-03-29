// Ŭnicode please
#pragma once
#include "filesystem.h"
#include <string>

namespace sora {;
namespace io {
	template<typename FileType> class CFileHelper;

	typedef enum {
		kSeekCurr,
		kSeekEnd,
		kSeekStart,
	} SeekOriginType;

	class ReadableFile {
	public:
		ReadableFile() {}
		virtual ~ReadableFile() {}

		virtual bool Open() = 0;
		virtual bool Close() = 0;
		virtual bool IsOpened() const = 0;
		virtual int Read(void *buf, int size) = 0;
		virtual bool Seek(int offset, SeekOriginType origin) = 0;
		virtual const void *GetBuffer() = 0;
		virtual int GetLength() const = 0;
		virtual int GetRemainLength() const = 0;

		virtual const std::string &filename() const = 0;
	};

	class WritableFile {
	public:
		WritableFile() {}
		virtual ~WritableFile() {}

		virtual bool Open() = 0;
		virtual bool Close() = 0;
		virtual bool IsOpened() const = 0;
		virtual int Write(const void *buf, int size) = 0;
		virtual const std::string &filename() const = 0;

		template<typename Container>
		int WriteContainer(const Container &data)
		{
			typedef typename Container::value_type T;
			const T *ptr = data.data();
			size_t size = data.size();
			return Write(ptr, size * sizeof(T));
		}
	};

	//ReadableFile에 추가 속성으로 붙을수 있는 인터페이스이다
	//복잡하게 read/seek를 사용하지 않고 메모리를 다루듯이 접근하기 위해서 만들었다
	//ReadableFile을 무조건 상속한 상태에서 이것도 상속하도록하자
	class ReadableMemoryFile {
	public:
		ReadableMemoryFile() {}
		virtual ~ReadableMemoryFile() {}

		virtual bool Open() = 0;
		virtual bool Close() = 0;
		virtual bool IsOpened() const = 0;

		virtual unsigned char *start() = 0;
		virtual unsigned char *end() = 0;
		virtual unsigned char *curr() = 0;
		virtual void *data() = 0;
		virtual void set_curr(unsigned char *curr) = 0;
	};

	class ReadonlyCFile : public ReadableFile {
	public:
		friend class CFileHelper<ReadonlyCFile>;
	public:
		ReadonlyCFile(const std::string &file);
		virtual ~ReadonlyCFile();

		virtual bool Open();
		virtual bool Close();
		virtual bool IsOpened() const;
		virtual const std::string &filename() const { return filename_; }

		virtual int Read(void *buf, int size);
		virtual bool Seek(int offset, SeekOriginType origin);
		virtual const void *GetBuffer();
		virtual int GetLength() const;
		virtual int GetRemainLength() const;

	private:
		FILE *file_;
		std::string filename_;
		std::vector<unsigned char> buffer_;
	};

	class WriteonlyCFile : public WritableFile {
	public:
		friend class CFileHelper<WriteonlyCFile>;
	public:
		WriteonlyCFile(const std::string &file);
		virtual ~WriteonlyCFile();

		virtual bool Open();
		virtual bool Close();
		virtual bool IsOpened() const;
		virtual int Write(const void *buf, int size);
		virtual const std::string &filename() const { return filename_; }

	private:
		FILE *file_;
		std::string filename_;
	};

	class SimpleMemoryFile : public ReadableFile, public ReadableMemoryFile {
	public:
		SimpleMemoryFile(const std::string &file);
		//데이터로 사용할것을 같이 넣어주면 Open까지 되엇다고 친다
		SimpleMemoryFile(const std::vector<unsigned char> &data);

		virtual ~SimpleMemoryFile();

		virtual bool Open();
		virtual bool Close();
		virtual bool IsOpened() const { return !data_.empty(); }
		virtual int Read(void *buf, int size);
		virtual bool Seek(int offset, SeekOriginType origin);
		virtual const void *GetBuffer() { return data_.data(); }
		virtual int GetLength() const;
		virtual int GetRemainLength() const { return data_.data() + data_.size() - curr_; }

		virtual const std::string &filename() const { return filename_; }

		virtual unsigned char *start() { return data_.data(); }
		virtual unsigned char *end() { return data_.data() + data_.size(); }
		virtual unsigned char *curr() { return curr_; }
		virtual void *data() { return data_.data(); }
		virtual void set_curr(unsigned char *curr) { curr_ = curr; }

	public:
		unsigned char *curr_;
		std::string filename_;
		std::vector<unsigned char> data_;
	};

	template<typename T> struct CFilePolicyHolder {};
	template<> 
	struct CFilePolicyHolder<ReadonlyCFile> {
		static const std::string &Mode() { static std::string mode("rb"); return mode; }
	};
	template<> 
	struct CFilePolicyHolder<WriteonlyCFile> {
		static const std::string &Mode() { static std::string mode("wb"); return mode; }
	};

	template<typename FileType>
	class CFileHelper {
	public:
		CFileHelper(FileType *f) :f_(f) {}
		~CFileHelper() {}

		bool Open()
		{
			using std::string;
			if(IsOpened()) {
				return false;
			}
			const string &mode = CFilePolicyHolder<FileType>::Mode();
			f_->file_ = fopen(f_->filename_.c_str(), mode.data());
			if (f_->file_ == nullptr) {
				return false;
			} else {
				return true;
			}
		} 

		bool Close()
		{
			if(IsOpened() == false) {
				return false;
			}
			fclose(f_->file_);
			f_->file_ = nullptr;
			return true;
		} 
	
		bool IsOpened() const
		{
			if(f_->file_ == nullptr) {
				return false;
			} else {
				return true;
			}
		} 


	private:
		FileType *f_;
	};
}	// namespace io
}	// namespcee sora
