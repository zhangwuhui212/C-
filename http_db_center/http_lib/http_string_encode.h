#ifndef HTTP_STRING_ENCODE_H
#define HTTP_STRING_ENCODE_H

#include <boost/locale.hpp>

namespace http{

	class http_string_encode{
	public:

		static int is_alpha_number_char( unsigned char c )   
		{   
			if ( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') )   
				return 1;   
			return 0;   
		} 

		static unsigned char hex_to_char(const unsigned char ch)
		{
			//return is_digit(x) ? x-'0' : x-'A'+10;
			unsigned char result = 0;
			if(ch >= '0' && ch <= '9')
				result = (ch - '0');
			else if(ch >= 'a' && ch <= 'z')
				result = (ch - 'a') + 10;
			else if(ch >= 'A' && ch <= 'Z')
				result = (ch - 'A') + 10;
			else
				result = -1;
			return result;
		}

		static unsigned char char_to_hex(const unsigned char value)
		{
			//return (unsigned char)(value > 9 ? value -10 + 'A': value + '0');
			unsigned char result = '\0';
			if(value >= 0 && value <= 9)
				result = (value + 48); //48为ascii编码的‘0’字符编码值
			else if(value >= 10 && value <= 15)
				result = (value - 10 + 65); //减去10则找出其在16进制的偏移量，65为ascii的'A'的字符编码值
			else
				;
			return result;
		}

		static std::string gbk_2_utf8(std::string const &text)  
		{  
			//"UTF-8", "GBK"  
			std::string const &to_encoding("UTF-8");  
			std::string const &from_encoding("GBK");  
			return boost::locale::conv::between(text.c_str(), text.c_str() + text.size(), to_encoding, from_encoding);  
		}  
		//直接处理utf8转gbk  
		static std::string utf8_2_gbk(std::string const &text)  
		{  
			std::string const &to_encoding("GBK");  
			std::string const &from_encoding("UTF-8");  
			return boost::locale::conv::between(text.c_str(), text.c_str() + text.size(), to_encoding, from_encoding);  
		} 

		static bool uri_encode(const std::string& in, std::string& out)
		{
			out.clear();
			out.reserve(in.size());
			for (std::size_t i = 0; i < in.size(); ++i)
			{
				unsigned char ch = in[i];
				if (ch == ' ')
				{
					out += '+';
				}
				else if (is_alpha_number_char(ch))
				{
					out += ch;
				}
				else
				{
					out +=  '%'; 
					out += char_to_hex( (unsigned char)(ch >> 4) ); 
					// ch & 0xff;
					out += char_to_hex( (unsigned char)(ch % 16) );
				}
			}
			return true;
		}

		static bool uri_decode(const std::string& in, std::string& out)
		{
			char p[2];
			out.clear();
			out.reserve(in.size());
			for (std::size_t i = 0; i < in.size(); ++i)
			{
				char ch = in[i];
				if (ch == '%')
				{
					if (i + 3 <= in.size())
					{
						int value = 0;
						p[0] = hex_to_char(in[++i]);
						p[1] = hex_to_char(in[++i]);
						// (high << 4) + low;
						out+= (unsigned char)(p[0] * 16 + p[1]); 
// 						std::istringstream is(in.substr(i + 1, 2));
// 						if (is >> std::hex >> value)
// 						{
// 							out += static_cast<char>(value);
// 							i += 2;
// 						}
// 						else
// 						{
// 							return false;
// 						}
					}
					else
					{
						return false;
					}
				}
				else if (ch == '+')
				{
					out += ' ';
				}
				else
				{
					out += ch;
				}
			}
			return true;
		}

	};

};//namespace http

#endif