// Standard C++ Headers
#include <cstdint>

#include "Base64Encoding.h"
  using std::string;         // from included <string>

#define LOBYTE(w) ((BYTE)((w) & 0xff))
#define LOWORD(a) ((WORD)(a))

string _64("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

int base64Encode(string& coded, const void* plain, std::uint32_t dwLength);
int base64Decode(const string& coded, void* plain, std::uint32_t dwLength);

string SurgeUtil::base64Encoded(const string& plain)
{
	string coded;
	// ignore possible error return code!
	base64Encode(coded, plain.c_str(), static_cast<std::uint32_t>(plain.size()));
	return coded;
}

string SurgeUtil::base64Decoded(const string& coded)
{
	string plain;
	std::uint32_t dwLen = static_cast<std::uint32_t>((coded.size() * 3) / 4);
	plain.resize(dwLen);
	// ignore possible error return code!
	base64Decode(coded, (void*)&plain[0], dwLen);
	return plain;
}

int base64Encode(string& coded, const void* plain, std::uint32_t dwLength)
{
	// safety net
	//
	coded = "";
	if (NULL == plain)
		return -1;

// 2002-07-15 - Spec actually says auth string is required
//		but doesn't say it can't be zero length.
//		Therefore, allow zero length!
//
	if (0 == dwLength)
		return 0;

  // Rules:
  //  0.	treat data as a bit stream.
  //  1.	take three octets to produce four code characters.
  //  1.1	if not enough octets, pad with zero bits to fill.
  //  1.2	msb of 1st octet is msb of 1st code
  //  2.	codes 0..63 map to  [A-Za-z0-9+/]
  //  3.	last code group comes from:
  //  3.1	three octets == normal encoding
  //  3.2	two octets   == three normal codes and one "=" pad character
  //  3.3	one octet    == two normal codes and two "=" pad characters.
  //
  // Example:	"A"						=>  "QQ=="
  // Example:	"AA"					=>  "QUE="
  // Example:	"AAA"					=>  "QUFB"
  // Example:	"Aladdin:open sesame"	=>  "QWxhZGRpbjpvcGVuIHNlc2FtZQ=="
  //

  std::uint32_t dw;
  const unsigned char* szClear = (const unsigned char*)plain;

  for(UINT i=0; i<dwLength; i+=3)
  {
    dw = 0;

    switch (dwLength - i)
    {
    default:
    case 3:
      dw = (szClear[i] << 16) | (szClear[i+1] << 8) | (szClear[i+2]);
      coded += _64[ (dw >> 18) & 0x3F ];
      coded += _64[ (dw >> 12) & 0x3F ];
      coded += _64[ (dw >>  6) & 0x3F ];
      coded += _64[ (dw      ) & 0x3F ];
      break;
    case 2:
      dw = (szClear[i] << 16) | (szClear[i+1] << 8) | 0;
      coded += _64[ (dw >> 18) & 0x3F ];
      coded += _64[ (dw >> 12) & 0x3F ];
      coded += _64[ (dw >>  6) & 0x3F ];
      coded += "=";
      break;
    case 1:
      dw = (szClear[i] << 16) | 0 | 0;
      coded += _64[ (dw >> 18) & 0x3F ];
      coded += _64[ (dw >> 12) & 0x3F ];
      coded += "=";
      coded += "=";
      break;
    }
  }

  return (int)coded.size();
}

int base64Decode(const string& coded, void* plain, std::uint32_t dwLength)
{
	// 2002-07-15 - Actually, spec allows for zero length string
	//	so, soften safety net to allow for zero length input and return.
	//
	if (NULL == plain)
		return -1;
	if (0 < dwLength)
		*((char*)plain) = '\0';	// in case of ASCIZ string

	if (0 == coded.size())
		return 0;

	std::uint32_t dwExpected = ((std::uint32_t)coded.size() * 3) / 4;
	if (dwLength < dwExpected)
	{
		// return error code if buffer not large enough!
		//
		return (-1*(int)dwExpected);
	}


  std::uint32_t dw;
  unsigned char* szClear = (unsigned char*)plain;
  size_t bufSize = coded.size();

  for(size_t i=0; i<bufSize; i+=4)
  {
    dw = 0;

    if ( (i<(bufSize-2)) && coded[i+2] == '=')
    {
      dw = (std::uint32_t)(_64.find( coded[i+0] ) << 18)
         | (std::uint32_t)(_64.find( coded[i+1] ) << 12);
      *szClear++ = LOBYTE(dw >> 16);
    }
    else if ( (i<(bufSize-3)) && coded[i+3] == '=')
    {
      dw = (std::uint32_t)(_64.find( coded[i+0] ) << 18)
         | (std::uint32_t)(_64.find( coded[i+1] ) << 12)
         | (std::uint32_t)(_64.find( coded[i+2] ) <<  6);
      *szClear++ = LOBYTE(dw >> 16);
      *szClear++ = LOBYTE(dw >>  8);
    }
    else
    {
      dw = (std::uint32_t)(_64.find( coded[i] ) << 18);
      dw |= (i<(bufSize-1)) ? (std::uint32_t)(_64.find( coded[i+1] ) << 12) : 0;
      dw |= (i<(bufSize-2)) ? (std::uint32_t)(_64.find( coded[i+2] ) <<  6) : 0;
      dw |= (i<(bufSize-3)) ? (std::uint32_t)_64.find( coded[i+3] ) : 0;
      *szClear++ = LOBYTE(dw >> 16);
      *szClear++ = LOBYTE(dw >>  8);
      *szClear++ = LOBYTE(dw      );
    }
  }
  std::uint32_t dwActual = (std::uint32_t)(szClear - (unsigned char*)plain);

  if (dwActual < dwLength)
    *szClear = '\0';	// in case it is an ASCIZ string

  return dwActual;
}
