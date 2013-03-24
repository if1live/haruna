// Ŭnicode please 
#pragma once

#include <vector>
#include <algorithm>
#include <string>

namespace sora {;
namespace io {
	class ZipHelper {
	public:
		ZipHelper();
		~ZipHelper();

		const std::string &ErrorCodeToStr(int ret);
		bool Compress(const std::vector<unsigned char> &src, std::vector<unsigned char> *dst);
		bool Decompress(const std::vector<unsigned char> &src, std::vector<unsigned char> *dst);

		int error() const { return error_; }
	private:
		int error_;
	};

	template<typename ContainerType>
	class ContainerStream {
	public:
		typedef ContainerType container_type;
		typedef typename ContainerType::value_type value_type;
	public:
		ContainerStream(const ContainerType &data)
			: data_(data), curr_(0) {}
		~ContainerStream() {}
		
		template<typename T>
		int Read(int size, T &dst)
		{
			if(size < 0) {
				return 0;
			}

			int data_size = data_.size();
			if(data_size <= curr_) {
				return 0;
			}

			int write_size = size;
			if((curr_ + size) > data_size) {
				write_size = data_size - curr_;				
			}
			auto data_begin = std::begin(data_) + curr_;
			auto dst_begin = std::begin(dst);
			std::copy(data_begin, data_begin + write_size, dst_begin);

			curr_ += write_size;
			return write_size;
		}
		bool IsEnd() const { return curr_ == data_.size(); }
	private:
		const ContainerType &data_;
		int curr_;
	};
}	// namespace io
}	// namespace sora