#include "blf/writer.hpp"

namespace blf
{
	Writer::Writer(String filePath)
	{
		m_writeStream = new std::ofstream(filePath, std::ios::binary);
	}

	Writer::~Writer()
	{
		delete m_writeStream;
	}

	void Writer::writeCommonTable(CommonTable* table)
	{
		// The indexer size goes first.
		write(table->getIndexerSize());

		// The amount of objects stored is second.
		write(table->getArraySize());

		for (TemplateObject* object : (*table))
		{
			// Writing the object type
			writeObject(object);
		}
	}
}
