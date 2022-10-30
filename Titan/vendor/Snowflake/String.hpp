#pragma once
#include <string>
#include <cassert>

class String
{
public:
	String();
	String(String& str);
	String(const char* cptr);
	String(const std::string& str);
	
	void operator=(const std::string& str);
	void operator=(const char* cptr);

	bool operator==(const std::string& str);
	bool operator==(const char* str);
	bool operator==(const String& str);

	bool operator!=(const std::string& str);
	bool operator!=(const char* str);
	bool operator!=(const String& str);

	char& operator[](size_t i);
	const char* c_str();
	char* Data();
	size_t MaxSize();
	size_t size();

	~String();

private:
	char m_Data[2048];
	size_t m_CurrentSize = 0;
	const size_t m_Size = 2048;
};

inline String::String()
{
	m_CurrentSize = 0;
	strset(m_Data, 0);
}

inline String::String(String& str)
{
	m_CurrentSize = str.m_CurrentSize;
	strcpy_s(m_Data, str.m_Data);
}

inline String::String(const char* cptr)
{
	size_t size = strlen(cptr);
	assert(size < 2048u && L"const char* is bigger than 2048");
	m_CurrentSize = size;
	strcpy_s(m_Data, cptr);
}

inline String::String(const std::string& str)
{
	m_CurrentSize = str.size();
	strcpy_s(m_Data, str.c_str());
}

inline void String::operator=(const std::string& str)
{
	m_CurrentSize = str.size();
	strset(m_Data, 0);
	strcpy_s(m_Data, str.c_str());
}

inline void String::operator=(const char* cptr)
{
	size_t size = strlen(cptr);
	m_CurrentSize = size;
	strcpy_s(m_Data, cptr);
}

inline bool String::operator==(const std::string& str)
{
	return 0 == strcmp(m_Data, str.c_str());
}

inline bool String::operator==(const char* str)
{
	return 0 == strcmp(m_Data, str);
}

inline bool String::operator==(const String& str)
{
	return 0 == strcmp(m_Data, str.m_Data);
}

inline bool String::operator!=(const std::string& str)
{
	return !operator==(str);
}

inline bool String::operator!=(const char* str)
{
	return !operator==(str);
}

inline bool String::operator!=(const String& str)
{
	return !operator==(str);
}

inline char& String::operator[](size_t i)
{
	return m_Data[i];
}

inline const char* String::c_str()
{
	return m_Data;
}

inline char* String::Data()
{
	return m_Data;
}

inline size_t String::MaxSize()
{
	return m_Size;
}

inline size_t String::size()
{
	return m_CurrentSize;
}

inline String::~String()
{
	/*delete[] m_Data;*/
}