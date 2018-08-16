#ifndef _BDAPI_H_
#define _BDAPI_H_


#include <string.h>

#include "boost/format.hpp"
#include "boost/locale.hpp"
#include "boost/property_tree/json_parser.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/program_options/detail/convert.hpp>
#include <boost/program_options/detail/utf8_codecvt_facet.hpp>

#include "curl/curl.h"

namespace bdapi
{
	static std::string baidu_key="21a199d566d967e75bf2acb884fe07df";
        /*
        5288675	PosConv3	Y5zR0DlhBDYxRqUZ7AcdmUcm
        5288662	PosConv2	83LHf67HOmjExYv13b5OGB9t
        5288658	PosConv	eF5nvHEcgSwdcrFp63NOLZZT
        */

	static size_t write_cb(char *ptr, size_t size, size_t nmemb, void *userdata){
        std::string *data=(std::string *)userdata;
        data->append(ptr,size*nmemb);
        return size*nmemb;
    }

    static bool convert(double lng,double lat,double &blng,double &blat){
        
        std::string url=(boost::format("http://api.map.baidu.com/geoconv/v1/?coords=%f,%f&from=1&to=5&ak=%s&output=json")
            %lng%lat%baidu_key).str();
        printf("query baidu location --> %s\n",url.c_str());
        std::string resp_data;
        CURLcode code=CURL_LAST;
        {
            CURL *curl=curl_easy_init();
            if (curl){
                curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
                curl_easy_setopt(curl,CURLOPT_WRITEDATA,&resp_data);
                curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,&bdapi::write_cb);
                code=curl_easy_perform(curl);
                curl_easy_cleanup(curl);
            }
        }
        if (code==CURLE_OK){
            //resp_data=boost::locale::conv::from_utf<char>(resp_data,"gbk");
            printf("baidu json data is %s \n",resp_data.c_str());
            boost::property_tree::ptree config;
            static boost::mutex mutex;
            try{
                std::stringstream ss;
                ss<<resp_data;
                {
                    boost::mutex::scoped_lock lock(mutex);
                    boost::property_tree::read_json(ss,config);
                }
                if (config.get("status",1)==0){
                    boost::property_tree::ptree data_config=config.get_child("result");
                    boost::property_tree::ptree & first_data=data_config.begin()->second;
                    blng=first_data.get("x",0.0);
                    blat=first_data.get("y",0.0);
                    return true;
                }
            }catch(std::exception &e){
                printf("baidu json parsing error --> %s\n",e.what());
            }
        }else{
            printf("baidu code is %d\n",code);
        }
        return false;
    }

	static bool reverse_encoding_services(double lng,double lat,std::string & addinfo){

		std::string url=(boost::format("http://api.map.baidu.com/geocoder/v2/?ak=%s&location=%f,%f&output=json&pois=0")
			%baidu_key%lat%lng).str();
		printf("query baidu location --> %s\n",url.c_str());
		std::string resp_data;
		CURLcode code=CURL_LAST;
		{
			CURL *curl=curl_easy_init();
			if (curl){
				curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
				curl_easy_setopt(curl,CURLOPT_WRITEDATA,&resp_data);
				curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,&bdapi::write_cb);
				code=curl_easy_perform(curl);
				curl_easy_cleanup(curl);
			}
		}
		if (code==CURLE_OK){
			//resp_data=boost::locale::conv::from_utf(resp_data,"gbk");
			printf("baidu json data is %s \n",resp_data.c_str());
			boost::property_tree::ptree config;
			static boost::mutex mutex;
			try{
				std::stringstream ss;
				ss<< resp_data;
				{
					boost::mutex::scoped_lock lock(mutex);
					boost::property_tree::read_json<boost::property_tree::ptree>(ss,config);
				}
				if (config.get<int>("status",1)==0)
				{
					//boost::property_tree::ptree data_config =config.get_child("result");
					//item_config = data_config.get_child("formatted_address");
					//item_config = data_config.get_child("addressComponent");
					addinfo = resp_data;
					return true;
				}
			}catch(std::exception &e){
				printf("baidu json parsing error --> %s\n",e.what());
			}
		}else{
			printf("baidu code is %d\n",code);
		}
		return false;
	}

}

#endif