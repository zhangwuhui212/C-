#ifndef HTTP_REPLY_H
#define HTTP_REPLY_H

#include <vector>
#include "http_request.h"

namespace http
{
	///// A reply to be sent to a client.
	struct http_reply
	{
		/// The status of the reply.
		enum status_type
		{
			ok = 200,
			created = 201,
			accepted = 202,
			no_content = 204,
			multiple_choices = 300,
			moved_permanently = 301,
			moved_temporarily = 302,
			not_modified = 304,
			bad_request = 400,
			unauthorized = 401,
			forbidden = 403,
			not_found = 404,
			internal_server_error = 500,
			not_implemented = 501,
			bad_gateway = 502,
			service_unavailable = 503
		} http_status;

		std::string to_buffers();
		static http_reply stock_reply(status_type status);

		std::vector<header> headers;
		//http 结构
		std::string content;
		std::string content_type;
		
		//回复数据
		int status;
		std::string result_str;
		std::string result;
	};
};//namespace http

#endif