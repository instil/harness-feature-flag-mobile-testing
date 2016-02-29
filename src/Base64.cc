// Standard C++ Headers
#include <cstdint>

#include "Base64.h"
  using std::string;         // from included <string>

#define LOBYTE(w) ((BYTE)((w) & 0xff))
#define LOWORD(a) ((WORD)(a))

typedef unsigned char BYTE;
typedef std::uint32_t UINT;

string _64("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

static int base64Encode(string& coded, const void* plain, std::uint32_t dwLength);

string SurgeUtil::Base64Encode(const string& plain)
{
	string coded;
	// ignore possible error return code!
	base64Encode(coded, plain.c_str(), static_cast<std::uint32_t>(plain.size()));
	return coded;
}

static
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
