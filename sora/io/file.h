// Ŭnicode please
#include "filesystem.h"
#include <string>

namespace sora {;
namespace io {
	template<typename FileType> class CFileHelper;

	class ReadableFile {
	public:
		virtual bool Open() = 0;
		virtual bool Close() = 0;
		virtual bool IsOpened() const = 0;
		virtual int Read(void *buf, int size) = 0;
		virtual int Seek(int offset, int origin) = 0;
		virtual const void *GetBuffer() = 0;
		virtual int GetLength() const = 0;
		virtual int GetRemainLength() const = 0;

		virtual const std::string &filepath() const = 0;
	};

	class WritableFile {
	public:
		virtual bool Open() = 0;
		virtual bool Close() = 0;
		virtual bool IsOpened() const = 0;
		virtual int Write(const void *buf, int size) = 0;
		virtual const std::string &filepath() const = 0;
	};

	class ReadonlyCFile : private ReadableFile {
	public:
		friend class CFileHelper<ReadonlyCFile>;
	public:
		ReadonlyCFile(const std::string &file);
		~ReadonlyCFile();

		virtual bool Open();
		virtual bool Close();
		virtual bool IsOpened() const;
		virtual const std::string &filepath() const { return filename_; }

		virtual int Read(void *buf, int size);
		virtual int Seek(int offset, int origin);
		virtual const void *GetBuffer();
		virtual int GetLength() const;
		virtual int GetRemainLength() const;

	private:
		FILE *file_;
		std::string filename_;
		void *buffer_;
	};

	class WriteonlyCFile : private WritableFile {
	public:
		friend class CFileHelper<WriteonlyCFile>;
	public:
		WriteonlyCFile(const std::string &file);
		~WriteonlyCFile();

		virtual bool Open();
		virtual bool Close();
		virtual bool IsOpened() const;
		virtual int Write(const void *buf, int size);
		virtual const std::string &filepath() const { return filename_; }

	private:
		FILE *file_;
		std::string filename_;
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
