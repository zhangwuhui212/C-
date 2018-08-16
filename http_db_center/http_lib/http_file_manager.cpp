#include "http_file_manager.h"
#include <fstream>
#include <sstream>

namespace http{

	struct mapping
	{
		const char* extension;
		const char* mime_type;
	} mappings[] =
	{
		{ "gif", "image/gif" },
		{ "htm", "text/html" },
		{ "html", "text/html" },
		{ "jpg", "image/jpeg" },
		{ "png", "image/png" },
		{ 0, 0 } // Marks end of list.
	};

	std::string extension_to_type(const std::string& extension)
	{
		for (mapping* m = mappings; m->extension; ++m)
		{
			if (m->extension == extension)
			{
				return m->mime_type;
			}
		}

		return "text/plain";
	}

	std::string get_extension(const std::string& file)
	{
		std::size_t last_slash_pos = file.find_last_of("/");
		std::size_t last_dot_pos   = file.find_last_of(".");
		std::string extension = "";
		if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
		{
			extension = file.substr(last_dot_pos + 1);
		}
		return extension;
	}

	http_file_manager::http_file_manager(std::string root)
		:m_doc_root(root)
	{

	}

	bool http_file_manager::load_file(std::string file)
	{
		std::string full_path = m_doc_root + file;
		std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
		if (!is)
		{
			return false;
		}
		char buf[512];
		m_file_content.clear();
		while (is.read(buf, sizeof(buf)).gcount() > 0)
			m_file_content.append(buf, is.gcount());
		m_file_type = extension_to_type(get_extension(m_file_content));
		// 	std::string doc_root = "./web/";
		// 	std::string request_path = "index.html";
		// 	std::string extension = ".html";
		// 	std::string full_path = doc_root + request_path;
		// 	std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
		// 	if (!is)
		// 	{
		// 		m_reply = http::reply::stock_reply(http::reply::not_found);
		// 		return;
		// 	}
		// 	m_reply.status = http::reply::ok;
		// 	char buf[512];
		// 	while (is.read(buf, sizeof(buf)).gcount() > 0)
		// 		m_reply.content.append(buf, is.gcount());
		// 	m_reply.content_type = http::server::mime_types::extension_to_type(extension);

		return true;
	}

};//namespace http