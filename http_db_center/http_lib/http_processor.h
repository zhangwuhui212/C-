#ifndef HTTP_PROCESSOR_H
#define HTTP_PROCESSOR_H

#include <list>
#include <map>
#include "http_request.h"

namespace http{
	
	struct http_request;

	struct uri_obj
	{
		std::string method;
		std::string file;
		std::string querystring;

		void reset()
		{
			method ="";
			querystring = "";
		}
	};

	class http_processor
	{
	public:
		http_processor();
		
		void reset();
		bool feed(const char * data_ptr,int data_size,int & pos);

		std::string get_method()
		{
			return m_request_.method;
		}

		std::string get_uri_method()
		{
			return m_uri.method;
		}

		std::string get_param(const char *name);

	private:
		bool process_uri();
		bool process_param();
		bool feedchar(char input);

		int get_content_lenght(std::vector<header> * plist);

		/// Check if a byte is an HTTP character.
		static bool is_char(int c);

		/// Check if a byte is an HTTP control character.
		static bool is_ctl(int c);

		/// Check if a byte is defined as an HTTP tspecial character.
		static bool is_tspecial(int c);

		/// Check if a byte is a digit.
		static bool is_digit(int c);

		/// Check if two characters are equal, without regard to case.
		static bool tolower_compare(char a, char b);

		/// Check whether the two request header names match.
		bool headers_equal(const std::string& a, const std::string& b);

	private:
		static std::string content_length_name_;
		std::size_t content_length_;
		http_request m_request_;
		uri_obj      m_uri;

		std::map<std::string,std::string> m_param_map;

		enum state
		{
			method_start,
			method,
			uri,
			http_version_h,
			http_version_t_1,
			http_version_t_2,
			http_version_p,
			http_version_slash,
			http_version_major_start,
			http_version_major,
			http_version_minor_start,
			http_version_minor,
			expecting_newline_1,
			header_line_start,
			header_lws,
			header_name,
			space_before_header_value,
			header_value,
			expecting_newline_2,
			expecting_newline_3,
			http_content,
			process_ok,
		} state_;
	};

};//http namespace

#endif