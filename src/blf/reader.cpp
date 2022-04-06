#include "blf/reader.hpp"

#include <chrono>
#include <zlib.h>

namespace blf
{
	Reader::Reader(blf::String filePath, BLF_FLAG flag)
	{
		m_strictTypeChecking = hasFlag(flag, FLAG_TYPESTRICT);
		m_verbose            = hasFlag(flag, FLAG_VERBOSE);

		auto t1 = std::chrono::high_resolution_clock::now();
		std::ifstream file = std::ifstream(filePath, std::ios::binary);
		m_readStream << file.rdbuf();
		file.close();
		auto t2 = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	}

	void Reader::decompress()
	{
		uInt compressedLength = read<uInt>();
		uInt uncompressedLength = read<uInt>();

		char* uncompressedData = new char[uncompressedLength];
		char* compressedData   = new char[compressedLength];
		
		std::string compressed = m_readStream.str().substr(m_readStream.tellg());
		compressed.copy(compressedData, compressedLength);

		z_stream stream;
		stream.zalloc = Z_NULL;
		stream.zfree  = Z_NULL;
		stream.opaque = Z_NULL;

		stream.avail_in 	= compressedLength;
		stream.next_in 		= (Bytef*)compressedData;
		stream.avail_out 	= uncompressedLength;
		stream.next_out 	= (Bytef*)uncompressedData;

		inflateInit(&stream);
		inflate(&stream, Z_NO_FLUSH);
		inflateEnd(&stream);

		delete[] compressedData;

		//std::cout << "COMPRESSED: " << compressedLength/1024 << "KiB" << std::endl;
		//std::cout << "UNCOMPRESSED: " << uncompressedLength/1024 << "KiB" << std::endl;

		m_readStream.seekg(0);
		m_readStream.str("");
		m_readStream.write(uncompressedData, uncompressedLength);
		m_readStream.seekg(0);

		delete[] uncompressedData;
	}

	Reader::~Reader()
	{
		//m_readStream->close();
		//delete m_readStream;
	}
}