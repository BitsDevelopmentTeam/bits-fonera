
//Taken from http://www.adp-gmbh.ch/cpp/common/base64.html

#include <string>

#ifndef BASE64_H
#define BASE64_H

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);

//Added by TFT
inline std::string base64_encode(const std::string& s)
{
	base64_encode(reinterpret_cast<const unsigned char*>(s.c_str()),s.length());
}

#endif //BASE64_H
