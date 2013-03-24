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

using std::string;

namespace sora {;
namespace io {
	const int kChunk = 16384;

	ZipHelper::ZipHelper()
		: error_(Z_OK)
	{
	}
	ZipHelper::~ZipHelper()
	{
	}

	const std::string &ZipHelper::ErrorCodeToStr(int ret)
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
	bool ZipHelper::Compress(const std::vector<unsigned char> &src, std::vector<unsigned char> *dst)
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
	bool ZipHelper::Decompress(const std::vector<unsigned char> &src, std::vector<unsigned char> *dst)
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
}	// namespace io
}	// namespace sora