#include "blf/writer.hpp"

#include <zlib.h>

namespace blf
{
	Writer::Writer(String filePath, BLF_FLAG flag)
	{
		m_allowCompression = hasFlag(flag, FLAG_COMPRESSED);
		m_verbose          = hasFlag(flag, FLAG_VERBOSE);
		
		m_outputPath = filePath;
	}

	Writer::~Writer()
	{
		//m_writeStream->close();
		//delete m_writeStream;
	}

	void Writer::writeCommonTable(const CommonTable& table, const ObjectTable& objects)
	{
		// The indexer size goes first.
		write(table.getIndexerSize());

		// The amount of objects stored is second.
		write(table.getArraySize());

		for (CommonTable::ConstIterator it = table.begin(); it != table.end(); it++)
		{
			// Writing the object type
			writeObject(*it, objects, table);
		}
	}

	const int DATA_CHUNK = 2*1024;
	const int PADDING = 2*DATA_CHUNK;

	void Writer::flushToFile()
	{
		//std::cout << "flushing..." << std::endl;
		std::ofstream outFile((const char*)m_outputPath, std::ios::binary);
		outFile << m_headerStream.rdbuf();
		if( m_allowCompression )
		{
			z_stream stream;
			stream.zalloc = Z_NULL;
			stream.zfree  = Z_NULL;
			stream.opaque = Z_NULL;

			unsigned int expectedLen = m_writeStream.rdbuf()->str().length()+1;

			char* in  = new char[expectedLen];
			char* out = new char[expectedLen+PADDING];
			m_writeStream.rdbuf()->str().copy(in, expectedLen, 0);
		
			stream.next_in = (Bytef*)in;
			stream.avail_in = expectedLen;
			stream.next_out = (Bytef*)out;
			stream.avail_out = expectedLen+PADDING;

			deflateInit(&stream, Z_BEST_COMPRESSION);
			deflate(&stream, Z_FINISH);
			deflateEnd(&stream);

			int compressedSize = (expectedLen+PADDING) - stream.avail_out;

			outFile.write(reinterpret_cast<char*>(&compressedSize), sizeof(uInt));
			outFile.write(reinterpret_cast<char*>(&expectedLen), sizeof(uInt));
			outFile.write((const char*)out, compressedSize);

			delete[] in;
			delete[] out;
		}
		else
		{
			outFile << m_writeStream.rdbuf();
		}
		outFile.close();
	}
}
