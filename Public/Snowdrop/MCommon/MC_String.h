#pragma once

#ifndef MC_STRING_H
#define MC_STRING_H

#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <iostream>

#include "../../Private/Memory/functions.h"

struct MC_String
{
	union
	{
		char buffer[16];
		char* ptr;
	};

	MC_String()
	{
		buffer[0] = 0;
		buffer[15] = 0;
	}

	MC_String(const char* str)
	{
		buffer[0] = 0;
		buffer[15] = 0;
		Assign(str);
	}

	const char* c_str() const
	{
		return (buffer[15] != 0) ? ptr : buffer;
	}

	MC_String& Assign(const char* str)
	{
		int len = 0;
		if (str)
		{
			const char* p = str;
			while (*p++) len++;
		}

		if (len < 15)
		{
			// free
			if (buffer[15] != 0 && ptr)
			{
				// size header is at ptr - 4 (i think)
				Free(ptr - 4);
			}

			memcpy(buffer, str, len);
			buffer[len] = 0;
			buffer[15] = 0;
		}
		else
		{
			int totalSize = 4 + len + 1;
			void* mem = Alloc(totalSize);

			if (mem)
			{
				if (buffer[15] != 0 && ptr)
				{
					Free(ptr - 4);
				}

				// write size header
				*static_cast<int*>(mem) = len + 1;

				// write string data
				char* data = static_cast<char*>(mem) + 4;
				memcpy(data, str, len);
				data[len] = 0;

				ptr = data;
				buffer[15] = 1; // mark as Large
			}
		}
		return *this;
	}

	MC_String& operator=(const char* str) { return Assign(str); }

	bool operator==(const char* other) const { return strcmp(c_str(), other) == 0; }
	bool operator!=(const char* other) const { return strcmp(c_str(), other) != 0; }

	operator const char*() const { return c_str(); }

	// MC_Strfmt - convenience wrapper for sprintf
	// use like this:	MC_String str = MC_Strfmt<32>("%f, %f", num1, num2); or just WhateverFunction(MC_Strfmt<>("%n says hi", name));
	template <int BUFSIZE>
	struct MC_Strfmt
	{
		MC_Strfmt(const char* aString, ...)
		{
			va_list paramList;
			va_start(paramList, aString);
#if _MSC_VER >= 1400
			vsnprintf_s(myBuffer, BUFSIZE, _TRUNCATE, aString, paramList);
#else
			_vsnprintf(myBuffer, BUFSIZE, aString, paramList);
#endif
			va_end(paramList);
		}

		operator const char*() const { return myBuffer; }

		char myBuffer[BUFSIZE];
	};
};

#endif
