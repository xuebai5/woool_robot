#include "framework.h"
#include <iostream>
void LogSystem(LPCTSTR log, COLORREF tc)
{
	std::cout << log << std::endl;
}
void LogChat(LPCTSTR log, COLORREF tc, COLORREF bc)
{
	std::cout << log << std::endl;
}


// hex2int
int hex2int(char c)
{
	if (c >= 'A' && c <= 'F')
		return(int)(c - 'A' + 10);
	else if (c >= 'a' && c <= 'f')
		return(int)(c - 'a' + 10);
	else if (c >= '0' && c <= '9')
		return(int)(c - '0');
	return 0;
}

int WINAPI Encode6BitBuf(const unsigned char* pszSrc, char* pszDest, int nSrcLen, int nDestLen)
//int CodeWoool(const BYTE * pIn, int size, CHAR * pOut)
{
	nDestLen--;
	int nDestPos = 0;
	unsigned char chMade, chRest = 0;
	int nRestCount = 0;

	for (int i = 0; i < nSrcLen; i++)
	{
		chMade = pszSrc[i] ^ 0xeb;
		if (nDestPos >= nDestLen)break;
		if (nRestCount == 2)
		{
			pszDest[nDestPos++] = (chMade & 0x3f) + 0x3b;
			chRest |= ((chMade >> 2) & 0x30);
			if (nDestPos >= nDestLen)break;
			pszDest[nDestPos++] = chRest + 0x3b;
			nRestCount = 0;
			chRest = 0;
		}
		else
		{
			pszDest[nDestPos++] = ((chMade & 0xF0) >> 2 | (chMade & 3)) + 0x3b;
			chRest <<= 2;
			chRest |= (chMade >> 2) & 3;
			nRestCount++;
		}
	}
	if ((nRestCount != 0) && (nDestPos < nDestLen))
	{
		pszDest[nDestPos++] = chRest + 0x3b;
	}
	pszDest[nDestPos] = (char)0;
	return nDestPos;
}
//---------------------------------------------------------------------------
int WINAPI Decode6BitBuf(const char* pszSrc, char* pszDest, int nDestLen)
{
	char ch1, ch2, ch3, ch4;

	--nDestLen; //给return前的那个赋值留1字节空间

	int nSrcLen = strlen(pszSrc);

	for (int i = 0; i < nSrcLen; i++)
	{
		if (pszSrc[i] < 0x3b)
			return 0;
	}

	//	if(nSrcLen<=0)
	//		return 0;

	int nSrcGroup = nSrcLen >> 2;	// 4个字节1组 组数

	nSrcLen = nSrcLen % 4;			//剩余长度

	int nDestPos = 0;

	//按4字节一组解码
	for (int i = 0; i < nSrcGroup; i++)
	{
		ch1 = pszSrc[i * 4] - 0x3b;
		ch2 = pszSrc[i * 4 + 1] - 0x3b;
		ch3 = pszSrc[i * 4 + 2] - 0x3b;
		ch4 = pszSrc[i * 4 + 3] - 0x3b;

		if (nDestPos >= nDestLen)goto _decode_exit_;
		pszDest[nDestPos++] = (((ch1 << 2) & 0xF0) | (ch1 & 0x03) | (ch4 & 0x0C)) ^ 0xeb;
		if (nDestPos >= nDestLen)goto _decode_exit_;
		pszDest[nDestPos++] = (((ch2 << 2) & 0xF0) | (ch2 & 0x03) | ((ch4 << 2) & 0x0C)) ^ 0xeb;
		if (nDestPos >= nDestLen)goto _decode_exit_;
		pszDest[nDestPos++] = (ch3 | ((ch4 << 2) & 0xC0)) ^ 0xeb;
	}

	//解码剩余字节
	if (nSrcLen == 2)
	{
		ch1 = pszSrc[nSrcGroup * 4] - 0x3b;
		ch4 = pszSrc[nSrcGroup * 4 + 1] - 0x3b;

		if (nDestPos >= nDestLen)goto _decode_exit_;
		pszDest[nDestPos++] = (((ch1 << 2) & 0xF0) | (ch1 & 0x03) | ((ch4 << 2) & 0x0C)) ^ 0xeb;
	}
	else if (nSrcLen == 3)
	{
		ch1 = pszSrc[nSrcGroup * 4] - 0x3b;
		ch2 = pszSrc[nSrcGroup * 4 + 1] - 0x3b;
		ch4 = pszSrc[nSrcGroup * 4 + 2] - 0x3b;

		if (nDestPos >= nDestLen)goto _decode_exit_;
		pszDest[nDestPos++] = (((ch1 << 2) & 0xF0) | (ch1 & 0x03) | (ch4 & 0x0C)) ^ 0xeb;
		if (nDestPos >= nDestLen)goto _decode_exit_;
		pszDest[nDestPos++] = (((ch2 << 2) & 0xF0) | (ch2 & 0x03) | ((ch4 << 2) & 0x0C)) ^ 0xeb;
	}

_decode_exit_:
	pszDest[nDestPos] = '\0';
	return nDestPos;
}

int memprintf(char* dest, const unsigned char* src, unsigned long len)
{
	const static char hextable[] = "0123456789ABCDEF";
	unsigned long SrcPos = 0, DestPos = 0;
	unsigned char ch;
	for (unsigned long SrcPos = 0; SrcPos < len; SrcPos++)
	{
		ch = src[SrcPos];
		dest[DestPos++] = hextable[(ch & 0xf0) >> 4];
		dest[DestPos++] = hextable[ch & 0x0f];
		if ((SrcPos & 15) == 15)
		{
			dest[DestPos++] = (char)0xd;
			dest[DestPos++] = (char)0xa;
		}
		else if ((SrcPos & 7) == 7)
		{
			dest[DestPos++] = ' ';
			dest[DestPos++] = '-';
			dest[DestPos++] = ' ';
		}
		else
		{
			dest[DestPos++] = ' ';
		}
	}
	dest[DestPos] = (char)0;
	return DestPos;
}

const int ColorTable[] =
{
  0,  0,  0,
128,  0,  0,
  0,128,  0,
128,128,  0,
  0,  0,128,
128,  0,128,
  0,128,128,
192,192,192,
 85,128,151,
157,185,200,
123,115,115,
 45, 41, 41,
 90, 82, 82,
 99, 90, 90,
 66, 57, 57,
 29, 24, 24,
 24, 16, 16,
 41, 24, 24,
 16,  8,  8,
242,121,113,
225,103, 95,
255, 90, 90,
255, 49, 49,
214, 90, 82,
148, 16,  0,
148, 41, 24,
 57,  8,  0,
115, 16,  0,
181, 24,  0,
189, 99, 82,
 66, 24, 16,
255,170,153,
 90, 16,  0,
115, 57, 41,
165, 74, 49,
148,123,115,
189, 82, 49,
 82, 33, 16,
123, 49, 24,
 45, 24, 16,
140, 74, 49,
148, 41,  0,
189, 49,  0,
198,115, 82,
107, 49, 24,
198,107, 66,
206, 74,  0,
165, 99, 57,
 90, 49, 24,
 42, 16,  0,
 21,  8,  0,
 58, 24,  0,
  8,  0,  0,
 41,  0,  0,
 74,  0,  0,
157,  0,  0,
220,  0,  0,
222,  0,  0,
251,  0,  0,
156,115, 82,
148,107, 74,
115, 74, 41,
 82, 49, 24,
140, 74, 24,
136, 68, 17,
 74, 33,  0,
 33, 24, 16,
214,148, 90,
198,107, 33,
239,107,  0,
255,119,  0,
165,148,132,
 66, 49, 33,
 24, 16,  8,
 41, 24,  8,
 33, 16,  0,
 57, 41, 24,
140, 99, 57,
 66, 41, 16,
107, 66, 24,
123, 74, 24,
148, 74,  0,
140,132,123,
107, 99, 90,
 74, 66, 57,
 41, 33, 24,
 70, 57, 41,
181,165,148,
123,107, 90,
206,177,148,
165,140,115,
140,115, 90,
181,148,115,
214,165,115,
239,165, 74,
239,198,140,
123, 99, 66,
107, 86, 57,
189,148, 90,
 99, 57,  0,
214,198,173,
 82, 66, 41,
148, 99, 24,
239,214,173,
165,140, 99,
 99, 90, 74,
189,165,123,
 90, 66, 24,
189,140, 49,
 53, 49, 41,
148,132, 99,
123,107, 74,
165,140, 90,
 90, 74, 41,
156,123, 57,
 66, 49, 16,
239,173, 33,
 24, 16,  0,
 41, 33,  0,
156,107,  0,
148,132, 90,
 82, 66, 24,
107, 90, 41,
123, 99, 33,
156,123, 33,
222,165,  0,
 90, 82, 57,
 49, 41, 16,
206,189,123,
 99, 90, 57,
148,132, 74,
198,165, 41,
 16,156, 24,
 66,140, 74,
 49,140, 66,
 16,148, 41,
  8, 24, 16,
  8, 24, 24,
  8, 41, 16,
 24, 66, 41,
165,181,173,
107,115,115,
 24, 41, 41,
 24, 66, 74,
 49, 66, 74,
 99,198,222,
 68,221,255,
140,214,239,
115,107, 57,
247,222, 57,
247,239,140,
247,231,  0,
107,107, 90,
 90,140,165,
 57,181,239,
 74,156,206,
 49,132,181,
 49, 82,107,
222,222,214,
189,189,181,
140,140,132,
247,247,222,
  0,  8, 24,
  8, 24, 57,
  8, 16, 41,
  8, 24,  0,
  8, 41,  0,
  0, 82,165,
  0,123,222,
 16, 41, 74,
 16, 57,107,
 16, 82,140,
 33, 90,165,
 16, 49, 90,
 16, 66,132,
 49, 82,132,
 24, 33, 49,
 74, 90,123,
 82,107,165,
 41, 57, 99,
 16, 74,222,
 41, 41, 33,
 74, 74, 57,
 41, 41, 24,
 74, 74, 41,
123,123, 66,
156,156, 74,
 90, 90, 41,
 66, 66, 20,
 57, 57,  0,
 89, 89,  0,
202, 53, 44,
107,115, 33,
 41, 49,  0,
 49, 57, 16,
 49, 57, 24,
 66, 74,  0,
 82, 99, 24,
 90,115, 41,
 49, 74, 24,
 24, 33,  0,
 24, 49,  0,
 24, 57, 16,
 99,132, 74,
107,189, 74,
 99,181, 74,
 99,189, 74,
 90,156, 74,
 74,140, 57,
 99,198, 74,
 99,214, 74,
 82,132, 74,
 49,115, 41,
 99,198, 90,
 82,189, 74,
 16,255,  0,
 24, 41, 24,
 74,136, 74,
 74,231, 74,
  0, 90,  0,
  0,136,  0,
  0,148,  0,
  0,222,  0,
  0,238,  0,
  0,251,  0,
 74, 90,148,
 99,115,181,
123,140,214,
107,123,214,
119,136,255,
198,198,206,
148,148,156,
156,148,198,
 49, 49, 57,
 41, 24,132,
 24,  0,132,
 74, 66, 82,
 82, 66,123,
 99, 90,115,
206,181,247,
140,123,156,
119, 34,204,
221,170,255,
240,180, 42,
223,  0,159,
227, 23,179,
255,251,240,
160,160,164,
128,128,128,
255,  0,  0,
  0,255,  0,
255,255,  0,
  0,  0,255,
255,  0,255,
  0,255,255,
255,255,255,
};