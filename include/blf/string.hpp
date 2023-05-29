#pragma once

#include <string>
#include <cstdint>

namespace blf
{
	typedef uint32_t StringLength;

	// This is supposed to be a class that intergrates any kind of string
	// into the BLF object format.
	class String
	{
		// The internal string buffer.
		char* m_stringBuffer;

		// Storing both string length and buffer length
		// for O(1) lookup times.
		size_t m_stringSize;
		size_t m_bufferSize;

		// Will delete the buffer once the String is deleted.
		// Not meant for use with predefined buffers, only ones that String made.
		bool m_shouldDelete = false;
		bool m_deleted = false;

		// TODO: OVERLOAD OPERATORS.

		// Setup method for c-strings, expects a string with a null terminator.
		// Presumes the c-string is not owned by the string.
		void setupCString(const char* string, size_t bufferSize);
		void copyCString(const String& otherString);

		public:

			// Operators
			String operator+  (const String& otherString) const;
			String operator=  (const String& otherString);
			bool operator== (const String& otherString) const;
			friend std::ostream& operator<<(std::ostream& ostream, String& string);

			operator const char*() { return getBuffer(); }
			explicit operator std::string() { return std::string(getBuffer(), getLength()); }

			String();

			// PLEASE NOTE, THE COPY CONSTRUCTOR ALSO CREATES
			// ITS OWN COPY OF THE STRING'S INTERNAL BUFFER.
			// YOU ONLY HAVE TO REMOVE THE ORIGINAL BUFFER YOURSELF.
			String(const String& string);

			String(const std::string& string);
			String(const char* stringBuffer);
			template<size_t size>
			String(const char(&stringBuffer)[size])
			{
				setupCString(stringBuffer, size);

				m_shouldDelete = false;
			}

			String(const char* stringBuffer, size_t bufferLength);

			~String();

			// Getter methods for members.
			size_t  getLength() const      	{ return m_stringSize; }
			size_t  getBufferLength() const	{ return m_bufferSize; }
			const char* getBuffer() const 	{ return m_stringBuffer; }
	};
}