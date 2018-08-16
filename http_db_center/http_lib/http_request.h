#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <iostream>
#include <vector>
#include <map>

namespace http{

	struct header
	{
		std::string name;
		std::string value;
	};

	/// A request received from a client.
	struct http_request
	{
		/// The request method, e.g. "GET", "POST".
		std::string method;

		/// The requested URI, such as a path to a file.
		std::string uri;

		/// Major version number, usually 1.
		int http_version_major;

		/// Minor version number, usually 0 or 1.
		int http_version_minor;

		/// The headers included with the request.
		std::vector<header> headers;

		/// The optional content sent with the request.
		std::string content;

		void reset()
		{
			method.clear();
			uri.clear();
			http_version_major = 0;
			http_version_minor = 0;
			headers.clear();
			content = "";
		}
	};

};//http namespace

#endif