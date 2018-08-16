#ifndef _AUTH_MANAGER_H_
#define _AUTH_MANAGER_H_

/*****************************************************************************
Copyright: 2015-2015, sinolbs
File name: auth_manager.h
Description: app verifycode manager
Author: zhangwuhui
Version: 1.0.0
Date: 2015-12
History: no
*****************************************************************************/

#include <objbase.h>

// #include <sstream>
// #include <boost/uuid/random_generator.hpp>
// #include <boost/uuid/uuid.hpp>

namespace auth_manager
{
	/*************************************************
	Function: create_uuid
	Description: create app verifycode
	Input: no
	Output: no
	Return: if success return a 32 bit string,else return ""
	Others: no
	*************************************************/
	std::string create_uuid()
	{
		std::string uuid = "";
		char buffer[64] = { 0 };

		GUID guid;
		if (CoCreateGuid(&guid))
		{
			return "";
		}
		_snprintf(buffer, sizeof(buffer), 
			"%08X%04X%04x%02X%02X%02X%02X%02X%02X%02X%02X", 
			guid.Data1, guid.Data2, guid.Data3, 
			guid.Data4[0], guid.Data4[1], guid.Data4[2], 
			guid.Data4[3], guid.Data4[4], guid.Data4[5], 
			guid.Data4[6], guid.Data4[7]);

		uuid.append(buffer);
		return uuid;
	}



// 	static std::string create_uuid()
// 	{
// 		std::stringstream str;
// 		boost::uuids::uuid u;
// 		boost::uuids::random_generator rgen;
// 		boost::uuids::uuid ranUUID = rgen();	// 生成一个随机uuid
// 
// 		int ss = ranUUID.size();
// 		for (int i=0; i<ss; ++i)
// 		{
// 			str << std::hex << (int)ranUUID.data[i];
// 		}
// 
// 		return str.str();
// 	}

	/*************************************************
	Function: create_uuid
	Description: try create a app verifycode max 3 times
	Input: 
	    appid  -- register app id
		apppwd -- register app password
	Output: no
	Return: if success return a 32 bit string,else return ""
	Others: appid and apppwd not use now
	*************************************************/
	static std::string create_verifycode(std::string appid,std::string apppwd)
	{
		int tc = 0;
		std::string verifycode = "";
		while(verifycode.empty())
		{
			verifycode = create_uuid();
			if (tc++>3)
			{
				break;
			}
		}
		return verifycode;
	}
};

#endif