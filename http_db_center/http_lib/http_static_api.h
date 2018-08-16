#ifndef _HTTP_STATIC_API_H_
#define _HTTP_STATIC_API_H_


#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp> 
#include <boost/algorithm/string.hpp>

#include "bdapi.h"
#include "mongo/client/dbclient.h"

namespace http{

	namespace http_static_api
	{
		static bool get_device_list(std::string sparams,std::list<std::string> * param_list,int sbegin = -1,int length = -1)
		{
			int pos = 0;
			std::string sparam = "";
			boost::char_separator<char> separator(",");
			boost::tokenizer<boost::char_separator<char> > tokens(sparams, separator);

			boost::tokenizer<boost::char_separator<char> >::iterator token_iter;
			for (token_iter = tokens.begin(); token_iter != tokens.end(); token_iter++)
			{
				if (pos < sbegin)
				{
					continue;
				}
				sparam = *token_iter;
				if (!sparam.empty())
				{
					param_list->push_back(sparam);
				}
				pos++;
				if ((pos-length)<=sbegin)
				{
					break;
				}
			}
			return true;
		}

		static mongo::BSONObj safe_fromjson(std::string json_record)
		{
			mongo::BSONObj obj;
			try
			{
				obj =  mongo::fromjson(json_record);
			}
			catch (std::exception* e)
			{
				printf("error load json_record,%s",e->what());
			}
			return obj;
		}

		static bool load_map_addinfo(double blng,double blat,std::string & json_str)
		{
			std::string addinfo = "";

			if(bdapi::reverse_encoding_services(blng, blat, addinfo))
			{
				mongo::BSONObjBuilder bb;
				mongo::BSONObj obj = http::http_static_api::safe_fromjson(addinfo);
				mongo::BSONObj robj = obj.getField("result").Obj();

				if(!obj.isEmpty() && !robj.isEmpty())
				{
					bb << "formatted_address" << robj.getStringField("formatted_address");
					bb << "addressComponent" << robj.getField("addressComponent");
					json_str = bb.obj().jsonString();
					return true;
				}
			}

			return false;
		}
	};//namespace http_static_api
	
	//设备定位数据
	struct device_map
	{
		std::string device_id;
		std::string map_type;
		double lat;
		double lng;
		int pt;//上报时间
		std::string mapaddinfo;//地图附加信息，百度地图有这个属性
		int bmapaddinfo;

		device_map()
		{
			reset();
		}

		void reset()
		{
			device_id = "";
			map_type = "GPS";
			lat = -1;
			lng = -1;
			pt  = -1;
			mapaddinfo="";
		}

		bool start(std::string device_id_, std::string map_type_ ,int bmapaddinfo_=0)//默认不加载地图不加信息
		{
			device_id = device_id_;
			map_type  = map_type_;
			bmapaddinfo = bmapaddinfo_;
			return true;
		}

		bool run(std::string record_data)
		{
			mongo::BSONObj obj = http::http_static_api::safe_fromjson(record_data);
			if (obj.isEmpty())
			{
				return false;
			}
			//device_id = obj.getStringField("vid");
			if(0==map_type.compare("BAIDU"))
			{
				lat = obj.getIntField("blat");
				lng = obj.getIntField("blng");
			}else
			{
				lat = obj.getIntField("lat");
				lng = obj.getIntField("lng");
			}

			pt  = obj.getIntField("ts");
			return true;
		}

		void end(mongo::BSONArrayBuilder * values)
		{
			if(0==map_type.compare("BAIDU"))
			{
				if (bmapaddinfo)
				{
					http_static_api::load_map_addinfo(lng,lat,mapaddinfo);
					values->append(BSON("deviceid" << device_id << "lat" <<  lng
						<< "lng" << lat << "pt" << timer_manager::datetime_to_str(pt) << "mapaddinfo" << mongo::fromjson(mapaddinfo)));
				}
				else
				{
					values->append(BSON("deviceid" << device_id << "lat" <<  lng
						<< "lng" << lat << "pt" << timer_manager::datetime_to_str(pt)));
				}
			}
			else
			{
				values->append(BSON("deviceid" << device_id << "lat" <<  lng
					<< "lng" << lat << "pt" << timer_manager::datetime_to_str(pt)));
			}
		}
	};


	//存储报警信息
	struct device_alarm
	{
		std::string device_id;//所属终端
		std::string device_type;//终端类型
		std::string alarm_id;//报警id
		std::string cn;//中文解释
		std::string en;//英文解释
		int  pt;//上报时间
		int level;//等级

		device_alarm()
		{
			reset();
		}

		void reset()
		{
			device_id = "";
			device_type = "";
			alarm_id = "";
			cn = "";
			en = "";
			pt = 0;
			level = 0;
		}

		bool load_info(std::string json_alarm)
		{
			mongo::BSONObj obj = http::http_static_api::safe_fromjson(json_alarm);
			if (!obj.isEmpty())
			{
				device_id = obj.getStringField("vid");
				alarm_id = obj.getStringField("alarmid");
				level    = obj.getIntField("level");
				pt       = obj.getIntField("endtime");
				return true;
			}
			return false;
		}

		bool load_mapping(std::string json_map)
		{
			mongo::BSONObj obj = http::http_static_api::safe_fromjson(json_map);
			if (!obj.isEmpty())
			{
				cn = obj.getStringField("cn");
				en = obj.getStringField("en");
				return true;
			}
			return false;
		}
	};

};



#endif