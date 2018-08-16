#include "http_processor.h"
#include "http_string_encode.h"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp> 
#include <boost/algorithm/string.hpp>

namespace http{

	std::string http_processor::content_length_name_ = "Content-Length";

	http_processor::http_processor()
	{
		reset();
	}

	void http_processor::reset()
	{
		state_ = method_start;
		m_request_.reset();
		m_uri.reset();
		m_param_map.clear();
		content_length_ = 0;
	}

	std::string http_processor::get_param(const char *name)
	{
		std::string mv = "";
		std::map<std::string,std::string>::iterator it = m_param_map.find(name);
		if (it != m_param_map.end())
		{
			mv = it->second;
			return http_string_encode::gbk_2_utf8(mv);
		}
		return "";
	}

	bool http_processor::process_uri()
	{
		std::string project = "/db_center/";
		
		int pos = -1;
		std::string str ="";
		std::string str_gbk = "";

		if (http_string_encode::uri_decode(m_request_.uri,str))//http - utf8
		{
			str_gbk = http_string_encode::utf8_2_gbk(str);//utf8 - gbk
		}
		else
		{
			return false;
		}
		if (str_gbk.find(project))
		{
			return false;
		}
		else
		{
			str_gbk = str_gbk.substr(strlen(project.c_str()));
		}

		pos = str_gbk.find_first_of('?');
		if (pos == -1)//no uri param
		{
			str = str_gbk;
			m_uri.querystring = "";
		}
		else
		{
			str = str_gbk.substr(0,pos);
			m_uri.querystring = str_gbk.substr(pos+1);
		}

		pos = str.find_last_of('.');
		if (pos==-1)//no file
		{
			m_uri.method = str;
		}
		else
		{
			pos = str.find_last_of('/');
			m_uri.method = str.substr(0,pos);
			m_uri.file   = str.substr(pos+1);
		}

		return true;
	}

	bool http_processor::process_param()
	{
		int pos = 0;
		std::string str_param = "";
		std::string sparam = "",sparam_field= "",sparam_value = "";
		
		if (0==m_request_.method.compare("POST"))
		{
			str_param = m_request_.content;
		}else if(0==m_request_.method.compare("GET"))
		{
			str_param = m_uri.querystring;
		}
		else
		{
			return false;
		}

		boost::char_separator<char> separator("&");
		boost::tokenizer<boost::char_separator<char> > tokens(str_param, separator);

		boost::tokenizer<boost::char_separator<char> >::iterator token_iter;
		for (token_iter = tokens.begin(); token_iter != tokens.end(); token_iter++)
		{
			sparam = *token_iter;
			if (pos = sparam.find('='))
			{
				sparam_field = sparam.substr(0,pos);
				sparam_value = sparam.substr(pos+1);
				if (sparam_field.size()>0)
				{
					m_param_map.insert(std::make_pair(sparam_field,sparam_value));
				}
			}
		}
		return true;
	}

	bool http_processor::feed(const char * data_ptr,int data_size,int & pos)
	{
		pos = 0;
		const char *act_ptr=data_ptr;
		int i =0;
		for (i=0;i<data_size;i++,act_ptr++)
		{
			char ch=*act_ptr;
			if (!feedchar(ch))
			{
				return false;
			}

			if (state_==process_ok)
			{
				pos = i+1;
				break;
			}
		}

		if (state_==process_ok && process_uri() && process_param())
		{
			return true;
		}

		return false;
	}

	bool http_processor::feedchar(char c)
	{
		switch (state_)
		{
		case method_start:
			if (!is_char(c) || is_ctl(c) || is_tspecial(c))
			{
				return false;
			}
			else
			{
				state_ = method;
				m_request_.method.push_back(c);
				return true;
			}
		case method:
			if (c == ' ')
			{
				state_ = uri;
				return true;
			}
			else if (!is_char(c) || is_ctl(c) || is_tspecial(c))
			{
				return false;
			}
			else
			{
				m_request_.method.push_back(c);
				return true;
			}
		case uri:
			if (c == ' ')
			{
				state_ = http_version_h;
				return true;
			}
			else if (is_ctl(c))
			{
				return false;
			}
			else
			{
				m_request_.uri.push_back(c);
				return true;
			}
		case http_version_h:
			if (c == 'H')
			{
				state_ = http_version_t_1;
				return true;
			}
			else
			{
				return false;
			}
		case http_version_t_1:
			if (c == 'T')
			{
				state_ = http_version_t_2;
				return true;
			}
			else
			{
				return false;
			}
		case http_version_t_2:
			if (c == 'T')
			{
				state_ = http_version_p;
				return true;
			}
			else
			{
				return false;
			}
		case http_version_p:
			if (c == 'P')
			{
				state_ = http_version_slash;
				return true;
			}
			else
			{
				return false;
			}
		case http_version_slash:
			if (c == '/')
			{
				m_request_.http_version_major = 0;
				m_request_.http_version_minor = 0;
				state_ = http_version_major_start;
				return true;
			}
			else
			{
				return false;
			}
		case http_version_major_start:
			if (is_digit(c))
			{
				m_request_.http_version_major = m_request_.http_version_major * 10 + c - '0';
				state_ = http_version_major;
				return true;
			}
			else
			{
				return false;
			}
		case http_version_major:
			if (c == '.')
			{
				state_ = http_version_minor_start;
				return true;
			}
			else if (is_digit(c))
			{
				m_request_.http_version_major = m_request_.http_version_major * 10 + c - '0';
				return true;
			}
			else
			{
				return false;
			}
		case http_version_minor_start:
			if (is_digit(c))
			{
				m_request_.http_version_minor = m_request_.http_version_minor * 10 + c - '0';
				state_ = http_version_minor;
				return true;
			}
			else
			{
				return false;
			}
		case http_version_minor:
			if (c == '\r')
			{
				state_ = expecting_newline_1;
				return true;
			}
			else if (is_digit(c))
			{
				m_request_.http_version_minor = m_request_.http_version_minor * 10 + c - '0';
				return true;
			}
			else
			{
				return false;
			}
		case expecting_newline_1:
			if (c == '\n')
			{
				state_ = header_line_start;
				return true;
			}
			else
			{
				return false;
			}
		case header_line_start:
			if (c == '\r')
			{
				state_ = expecting_newline_3;
				return true;
			}
			else if (!m_request_.headers.empty() && (c == ' ' || c == '\t'))
			{
				state_ = header_lws;
				return true;
			}
			else if (!is_char(c) || is_ctl(c) || is_tspecial(c))
			{
				return false;
			}
			else
			{
				m_request_.headers.push_back(header());
				m_request_.headers.back().name.push_back(c);
				state_ = header_name;
				return true;
			}
		case header_lws:
			if (c == '\r')
			{
				state_ = expecting_newline_2;
				return true;
			}
			else if (c == ' ' || c == '\t')
			{
				return true;
			}
			else if (is_ctl(c))
			{
				return false;
			}
			else
			{
				state_ = header_value;
				m_request_.headers.back().value.push_back(c);
				return true;
			}
		case header_name:
			if (c == ':')
			{
				state_ = space_before_header_value;
				return true;
			}
			else if (!is_char(c) || is_ctl(c) || is_tspecial(c))
			{
				return false;
			}
			else
			{
				m_request_.headers.back().name.push_back(c);
				return true;
			}
		case space_before_header_value:
			if (c == ' ')
			{
				state_ = header_value;
				return true;
			}
			else
			{
				return false;
			}
		case header_value:
			if (c == '\r')
			{
				state_ = expecting_newline_2;
				return true;
			}
			else if (is_ctl(c))
			{
				return false;
			}
			else
			{
				m_request_.headers.back().value.push_back(c);
				return true;
			}
		case expecting_newline_2:
			if (c == '\n')
			{
				state_ = header_line_start;
				return true;
			}
			else
			{
				return false;
			}
		case expecting_newline_3:
			if (c == '\n')
			{
				content_length_ = get_content_lenght(&m_request_.headers);
				if (content_length_==0)
				{
					state_ = process_ok;
				}else
				{
					state_ = http_content;
				}
				return true;
			}
			else
			{
				return false;
			}
		case http_content:
			if (c == '\r' || c == '\n')
			{
				return false;
			}
			else if(m_request_.content.size() < content_length_)
			{
				m_request_.content.push_back(c);
				if (m_request_.content.size()==content_length_)
				{
					state_ = process_ok;
				}
				return true;
			}
		default:
			printf("un kown state\r\n");
		}
		return false;
	}

	int http_processor::get_content_lenght(std::vector<header> * plist)
	{
		std::vector<header>::iterator it = plist->begin();
		for (;it!=plist->end();it++)
		{
			if (headers_equal((*it).name, content_length_name_))
			{
				try
				{
					return atol((*it).value.c_str());
				}
				catch (...)
				{
					return 0;
				}
			}
			if ((*it).name=="Content-Length")
			{
				
			}
		}
		return 0;
	}

	bool http_processor::is_char(int c)
	{
		return c >= 0 && c <= 127;
	}

	bool http_processor::is_ctl(int c)
	{
		return (c >= 0 && c <= 31) || (c == 127);
	}

	bool http_processor::is_tspecial(int c)
	{
		switch (c)
		{
		case '(': case ')': case '<': case '>': case '@':
		case ',': case ';': case ':': case '\\': case '"':
		case '/': case '[': case ']': case '?': case '=':
		case '{': case '}': case ' ': case '\t':
			return true;
		default:
			return false;
		}
	}

	bool http_processor::is_digit(int c)
	{
		return c >= '0' && c <= '9';
	}

	bool http_processor::tolower_compare(char a, char b)
	{
		return std::tolower(a) == std::tolower(b);
	}

	bool http_processor::headers_equal(const std::string& a, const std::string& b)
	{
		if (a.length() != b.length())
			return false;

		return std::equal(a.begin(), a.end(), b.begin(),
			&http_processor::tolower_compare);
	}

};//http namespace