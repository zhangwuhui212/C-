#ifndef HTTP_FILE_MANAGER_H
#define HTTP_FILE_MANAGER_H

#include <string>

namespace http{

	class http_file_manager
	{
	public:
		http_file_manager(std::string root);

		bool load_file(std::string file);

		std::string get_content()
		{
			return m_file_content;
		}

		std::string get_type()
		{
			return m_file_type;
		}

	private:
		std::string m_doc_root;

		std::string m_file_content;
		std::string m_file_type;
	};

};//namespace http


#endif