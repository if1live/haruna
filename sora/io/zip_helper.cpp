// Ŭnicode please 
#include "stdafx.h"
#include "zip_helper.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <zlib.h>
#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#include "minizip/unzip.h"
#include "minizip/zip.h"
#include "minizip/iowin32.h"

using std::string;
using std::vector;

namespace sora {;
namespace io {
	const int kChunk = 16384;

	ZlibHelper::ZlibHelper()
		: error_(Z_OK)
	{
	}
	ZlibHelper::~ZlibHelper()
	{
	}

	const std::string &ZlibHelper::ErrorCodeToStr(int ret)
	{
		switch(ret) {
		case Z_ERRNO: 
			{
				static string msg("Error Writing Input or Output");
				return msg;
			}
			break;
		case Z_STREAM_ERROR:
			{
				static string msg("invalid compression level");
				return msg;
			}
			break;
		case Z_DATA_ERROR:
			{
				static string msg("invalid or incomplete deflate data");
				return msg;
			}
			break;
		case Z_MEM_ERROR:
			{
				static string msg("out of memory");
				return msg;
			}
			break;
		case Z_VERSION_ERROR:
			{
				static string msg("zlib version mismatch!");
				return msg;
			}
			break;
		}
		static string msg("unknown");
		return msg;
	}

	/* Compress from file source to file dest until EOF on source.
	def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
	allocated for processing, Z_STREAM_ERROR if an invalid compression
	level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
	version of the library linked do not match, or Z_ERRNO if there is
	an error reading or writing the files. */
	bool ZlibHelper::Compress(const std::vector<unsigned char> &src, std::vector<unsigned char> *dst)
	{
		int level = Z_DEFAULT_COMPRESSION;
		int ret, flush;
		unsigned have;
		z_stream strm;
		unsigned char in[kChunk];
		unsigned char out[kChunk];

		/* allocate deflate state */
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		ret = deflateInit(&strm, level);
		if (ret != Z_OK) {
			error_ = ret;
			return false;
		}

		ContainerStream<std::vector<unsigned char>> in_stream(src);

		/* compress until end of file */
		do {
			strm.avail_in = in_stream.Read(kChunk, in);
			if(in_stream.IsEnd()) {
				flush = Z_FINISH;
			} else {
				flush = Z_NO_FLUSH;
			}
			strm.next_in = in;

			/* run deflate() on input until output buffer not full, finish
			compression if all of source has been read in */
			do {
				strm.avail_out = kChunk;
				strm.next_out = out;

				ret = deflate(&strm, flush);    /* no bad return value */
				assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

				have = kChunk - strm.avail_out;
				dst->reserve(dst->size() + have);
				std::copy(out, out + have, std::back_inserter(*dst));
			} while (strm.avail_out == 0);
			assert(strm.avail_in == 0);     /* all input will be used */
			/* done when last data in file processed */
		} while (flush != Z_FINISH);
		assert(ret == Z_STREAM_END);        /* stream will be complete */
		/* clean up and return */
		(void)deflateEnd(&strm);
		error_ = Z_OK;
		return true;
	}

	/* Decompress from file source to file dest until stream ends or EOF.
	inf() returns Z_OK on success, Z_MEM_ERROR if memory could not be
	allocated for processing, Z_DATA_ERROR if the deflate data is
	invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
	the version of the library linked do not match, or Z_ERRNO if there
	is an error reading or writing the files. */
	bool ZlibHelper::Decompress(const std::vector<unsigned char> &src, std::vector<unsigned char> *dst)
	{
		int ret;
		unsigned have;
		z_stream strm;
		unsigned char in[kChunk];
		unsigned char out[kChunk];

		/* allocate inflate state */
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.avail_in = 0;
		strm.next_in = Z_NULL;
		ret = inflateInit(&strm);
		if (ret != Z_OK) {
			error_ = ret;
			return false;
		}

		ContainerStream<std::vector<unsigned char>> in_stream(src);

		/* decompress until deflate stream ends or end of file */
		do {
			strm.avail_in = in_stream.Read(kChunk, in);
			if (strm.avail_in == 0) {
				break;
			}
			strm.next_in = in;

			/* run inflate() on input until output buffer not full */
			do {
				strm.avail_out = kChunk;
				strm.next_out = out;

				ret = inflate(&strm, Z_NO_FLUSH);
				assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
				switch (ret) {
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;     /* and fall through */
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
					error_ = ret;
					return false;
				}

				have = kChunk - strm.avail_out;
				dst->reserve(dst->size() + have);
				std::copy(out, out + have, std::back_inserter(*dst));
			} while (strm.avail_out == 0);
			/* done when inflate() says it's done */
		} while (ret != Z_STREAM_END);
		/* clean up and return */
		(void)inflateEnd(&strm);

		if(ret == Z_STREAM_END) {
			error_ = Z_OK;
			return true;
		} else {
			error_ = Z_DATA_ERROR;
			return false;
		}
	}

	ZipHelper::ZipHelper()
		: uf_(nullptr), error_(UNZ_OK)
	{
	}
	ZipHelper::~ZipHelper()
	{
		Close();
	}

	bool ZipHelper::Open(const std::string &zipfilename, const std::string &password)
	{
		password_ = password;
		
		if(this->uf_ != nullptr) {
			return false;
		}
#ifdef USEWIN32IOAPI
		zlib_filefunc64_def ffunc;
		fill_win32_filefunc64A(&ffunc);
		uf_ = unzOpen2_64(zipfilename, &ffunc);
#else
		uf_ = unzOpen64(zipfilename.data());
#endif
		if(uf_ == NULL) {
			//printf("Cannot open %s\n", zipfilename);
			return false;
		}
		return true;
	}

	bool ZipHelper::Close()
	{
		password_ = "";
		if(uf_ == nullptr) {
			return false;
		}
		unzClose(uf_);
		uf_ = nullptr;
		return true;
	}

	void display_zpos64(ZPOS64_T n, int size_char)
	{

		/* To avoid compatibility problem we do here the conversion */
		char number[21] = {0};
		int offset = 19;
		int pos_string = 19;
		int size_display_string = 19;

		while (1)
		{
			number[offset] = (char)((n%10) + '0');
			if (number[offset] != '0')
				pos_string = offset;
			n /= 10;
			if (offset == 0)
				break;
			offset--;
		}

		size_display_string -= pos_string;
		while (size_char-- > size_display_string)
			printf(" ");
		printf("%s",&number[pos_string]);
	}

	std::vector<std::string> ZipHelper::GetList()
	{
		std::vector<std::string> file_list;

		uLong i = 0;
		int err = unzGoToFirstFile(uf_);
		if (err != UNZ_OK) {
			printf("error %d with zipfile in unzGoToFirstFile\n", err);
			error_ = err;
			return std::vector<std::string>();
		}

		do {
			char filename_inzip[256] = {0};
			unz_file_info64 file_info = {0};
			uLong ratio = 0;
			const char *string_method = NULL;
			char charCrypt = ' ';

			err = unzGetCurrentFileInfo64(uf_, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
			if (err != UNZ_OK) {
				printf("error %d with zipfile in unzGetCurrentFileInfo\n", err);
				break;
			}

			//directory는 뺴기
			string filename(filename_inzip);
			if(filename[filename.size()-1] != '/') {
				file_list.push_back(filename);
			}
			err = unzGoToNextFile(uf_);
		} while (err == UNZ_OK);
		if (err != UNZ_END_OF_LIST_OF_FILE) {
			//printf("error %d with zipfile in unzGoToNextFile\n", err);
			err = UNZ_OK;
		}

		error_ = err;
		return file_list;
	}

	bool ZipHelper::PrintList()
	{
		uLong i = 0;
		int err = unzGoToFirstFile(uf_);
		if (err != UNZ_OK) {
			printf("error %d with zipfile in unzGoToFirstFile\n", err);
			error_ = err;
			return false;
		}

		printf("  Length  Method     Size Ratio   Date    Time   CRC-32     Name\n");
		printf("  ------  ------     ---- -----   ----    ----   ------     ----\n");

		do {
			char filename_inzip[256] = {0};
			unz_file_info64 file_info = {0};
			uLong ratio = 0;
			const char *string_method = NULL;
			char charCrypt = ' ';

			err = unzGetCurrentFileInfo64(uf_, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
			if (err != UNZ_OK) {
				printf("error %d with zipfile in unzGetCurrentFileInfo\n", err);
				break;
			}

			if (file_info.uncompressed_size > 0) {
				ratio = (uLong)((file_info.compressed_size*100) / file_info.uncompressed_size);
			}

			/* Display a '*' if the file is encrypted */
			if ((file_info.flag & 1) != 0) {
				charCrypt = '*';
			}

			if (file_info.compression_method == 0) {
				string_method = "Stored";
			} else if (file_info.compression_method == Z_DEFLATED) {
				uInt iLevel = (uInt)((file_info.flag & 0x6) / 2);
				if (iLevel == 0) {
				  string_method = "Defl:N";
				} else if (iLevel == 1) {
				  string_method = "Defl:X";
				} else if ((iLevel == 2) || (iLevel == 3)) {
				  string_method = "Defl:F"; /* 2:fast , 3 : extra fast*/
				}
			} else if (file_info.compression_method == Z_BZIP2ED) {
				string_method = "BZip2 ";
			} else {
				string_method = "Unkn. ";
			}

			display_zpos64(file_info.uncompressed_size, 7);
			printf("  %6s%c", string_method, charCrypt);
			display_zpos64(file_info.compressed_size, 7);
			printf(" %3lu%%  %2.2lu-%2.2lu-%2.2lu  %2.2lu:%2.2lu  %8.8lx   %s\n", ratio,
					(uLong)file_info.tmu_date.tm_mon + 1, (uLong)file_info.tmu_date.tm_mday,
					(uLong)file_info.tmu_date.tm_year % 100,
					(uLong)file_info.tmu_date.tm_hour, (uLong)file_info.tmu_date.tm_min,
					(uLong)file_info.crc, filename_inzip);

			err = unzGoToNextFile(uf_);
		} while (err == UNZ_OK);
		if (err != UNZ_END_OF_LIST_OF_FILE) {
			printf("error %d with zipfile in unzGoToNextFile\n", err);
		}

		error_ = err;
		return (error_ == UNZ_OK);
	}

	bool ZipHelper::Reset()
	{
		int err = unzGoToFirstFile(uf_);
		if (err != UNZ_OK) {
			error_ = err;
			return false;
		}
		return true;
	}
	bool ZipHelper::GetFile(const std::string &filename, std::vector<unsigned char> *data)
	{
		if (unzLocateFile(uf_, filename.data(), NULL) != UNZ_OK) {
			//printf("file %s not found in the zipfile\n", filename);
			error_ = 2;
			return false;
		}

		unz_file_info64 file_info = {0};
		int err = UNZ_OK;
		int errclose = UNZ_OK;
		char filename_inzip[256] = {0};

		err = unzGetCurrentFileInfo64(uf_, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
		if (err != UNZ_OK)
		{
			printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);
			error_ = err;
			return false;
		}

		const uInt size_buf = 1024 * 8;
		unsigned char buf[size_buf];

		if (buf == NULL) {
			//printf("Error allocating memory\n");
			error_ = UNZ_INTERNALERROR;
			return false;
		}

		const char *pw_ptr = nullptr;
		if(password_.empty() == false) {
			pw_ptr = password_.data();
		}
		err = unzOpenCurrentFilePassword(uf_, pw_ptr);
		if (err != UNZ_OK) {
			//printf("error %d with zipfile in unzOpenCurrentFilePassword\n", err);
			error_ = err;
			return false;
		}

		do {
			err = unzReadCurrentFile(uf_, buf, size_buf);
			if (err < 0) {
				printf("error %d with zipfile in unzReadCurrentFile\n", err);
				break;
			}
			if (err == 0) {
				break;
			}

			data->reserve(data->size() + err);
			std::copy(buf, buf + err, std::back_inserter(*data));
		} while (err > 0);

		errclose = unzCloseCurrentFile(uf_);
		if (errclose != UNZ_OK) {
			//printf("error %d with zipfile in unzCloseCurrentFile\n", errclose);
			error_ = errclose;
			return false;
		}

		error_ = err;
		return (err == UNZ_OK);
	}

}	// namespace io
}	// namespace sora