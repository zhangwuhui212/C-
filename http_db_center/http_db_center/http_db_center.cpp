#include <iostream>

#include "http_dump_server.h"


int main()
{
	curl_global_init(CURL_GLOBAL_ALL);

	http_server_run("my_db_center.conf");

	return 0;
}