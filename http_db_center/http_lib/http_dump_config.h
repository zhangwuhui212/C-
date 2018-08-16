#ifndef HTTP_DUMP_CONFIG_H_
#define HTTP_DUMP_CONFIG_H_

#include <string.h>
#include <map>

#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/xml_parser.hpp"
#include "boost/property_tree/json_parser.hpp"

namespace http{

	struct app_obj
	{
	public:
		app_obj()
		{
		}

		bool has_api(std::string apiid)
		{
			std::map<std::string,std::string>::iterator it = api_list_.find(apiid);
			if (it!=api_list_.end())
			{
				return true;
			}
			return false;
		}

		bool has_device(std::string device)
		{
			std::map<std::string,std::string>::iterator it = device_list_.find(device);
			if (it!=device_list_.end())
			{
				return true;
			}
			return false;
		}

		struct app_base
		{
			std::string appid_;
			std::string apppwd_;
			std::string timelimit_;
			std::string verifycode_;

			app_base()
			{
				appid_="";
				apppwd_="";
				timelimit_ = "";
				verifycode_ ="";
			}
		};

		app_base base;

		std::map<std::string,std::string> api_list_;
		std::map<std::string,std::string> device_list_;//终端号唯一
	};


	class http_dump_config{
	public:
		bool load_config(std::string config)
		{
			return load_system_config(config);
		}

		bool load_system_config(std::string config_file)
		{
			boost::property_tree::ptree config;
			try
			{
				boost::property_tree::read_json(config_file, config);
			}
			catch(std::exception &e)
			{
				printf("[get_interval_time] read config error! --> %s.\n", e.what());
				return false;
			}

			listen_port  = config.get("port", 8001);
			factory_size = config.get("factory_size", 10);
			thread_num = config.get("thread_num", 5);

			sweep_interval = config.get("sweep_interval", 6);

			mongo_num = config.get("mongo_num", 10);

			mongo_host = config.get("mongo_host","182.92.157.147:6010").c_str();
			mongo_auth = config.get("mongo_auth","zhangwuhui:zhangwuhui@wang").c_str();

			redis_host = config.get("redis_host","182.92.157.147").c_str();
			redis_port = config.get("redis_port", 6003);
			return true;
		}

		void load_mongo_db_rule()
		{
			boost::property_tree::ptree config;
			try
			{
				boost::property_tree::read_xml("./rule_root/mongo_db_rule.xml", config);
				std::cout << "获取数据" << config.data()<< std::endl;
			}
			catch(std::exception &e)
			{
				printf("[get_interval_time] read config error! --> %s.\n", e.what());
				return;
			}

			boost::property_tree::ptree root;
			root = config.get_child("mongo_method_list");
		}

		void pre_load()
		{
			pre_load_app();
			pre_load_api();
		}

		//app function
	public:

		bool save_user(std::string json_record,void * mongo_ptr=NULL);

		void app_add_api(std::string appid,std::string apiid)
		{
			app_map_[appid].api_list_.insert(std::make_pair(apiid,""));
		}

		void app_add_device(std::string appid,std::string device)
		{
			app_map_[appid].device_list_.insert(std::make_pair(device,""));
		}

		bool find_app(std::string verifycode,app_obj::app_base * app)
		{
			for (std::map<std::string,app_obj>::iterator it = app_map_.begin();
				it!=app_map_.end();it++)
			{
				if (!(*it).second.base.verifycode_.compare(verifycode))
				{
					app->appid_ =  (*it).second.base.appid_;   
					app->apppwd_  =  (*it).second.base.apppwd_;           
					app->timelimit_ =  (*it).second.base.timelimit_;      
					app->verifycode_ =  (*it).second.base.verifycode_;   
					return true;
				}
			}
			return false;
		}

		bool find_app(std::string appid,std::string apppwd="",app_obj::app_base * app=NULL)
		{
			std::map<std::string,app_obj>::iterator it = app_map_.find(appid);
			if (it!=app_map_.end())
			{
				if (apppwd.size()>0 && (*it).second.base.apppwd_.compare(apppwd))
				{
					return false;
				}
				if (NULL!=app)
				{
					app->appid_ =  (*it).second.base.appid_;   
					app->apppwd_  =  (*it).second.base.apppwd_;           
					app->timelimit_ =  (*it).second.base.timelimit_;      
					app->verifycode_ =  (*it).second.base.verifycode_;
				}
				return true;
			}
			return false;
		}

		bool app_has_api(std::string appid,std::string apiid)
		{
			std::map<std::string,app_obj>::iterator it = app_map_.find(appid);
			if (it!=app_map_.end())
			{
				if ((*it).second.has_api(apiid))
				{
					return true;
				}
 			}
			return false;
		}

		bool app_has_device(std::string appid,std::string deviceid)
		{
			std::map<std::string,app_obj>::iterator it = app_map_.find(appid);
			if (it!=app_map_.end())
			{
				if ((*it).second.has_device(deviceid))
				{
					return true;
				}
			}
			return false;
		}
		
		//api function
	public:
		std::string get_api_name(int apiid)
		{
			std::map<int,std::string>::iterator it = api_map_.find(apiid);
			if (it!=api_map_.end())
			{
				return api_map_[apiid];
			}
			return "";
		}

		void api_add(int apiid,std::string apiname)
		{
			api_map_.insert(std::make_pair(apiid,apiname));
		}

	private:

		bool pre_load_app();
		bool pre_load_api();

	public:
		int listen_port;
		int factory_size;
		int thread_num;
		int sweep_interval;

		int mongo_num;
		std::string mongo_host;
		std::string mongo_auth;

		std::string redis_host;
		int redis_port;

		std::map<std::string,app_obj> app_map_;

		std::map<int,std::string> api_map_;
	};

};

#endif