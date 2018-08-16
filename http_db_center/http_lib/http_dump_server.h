
#define _WIN32_WINNT 0x0501

#pragma warning(disable:4251)
#pragma warning(disable:4275)
#pragma warning(disable:4996)

#define BOOST_SPIRIT_THREADSAFE

#include "http_worker.h"
#include "http_dump_config.h"
#include "http_string_encode.h"
#include "http_page_manager.h"
#include "http_file_manager.h"
#include "curl/curl.h"

#include "auth_manager.h"
#include "timer_manager.h"
#include "log_manager.h"

#include "http_static_api.h"
#include "dump_db_center.h"

http::http_dump_config m_dump_config;
http::dump_db_center   m_dump_center;
http::http_file_manager *  m_file_manager;

int process_device_alarm(http::mongo_context * mtx_ptr,std::string alarm_record,http::device_alarm * alarm_)
{
	std::string str = "";

	if (!alarm_->load_info(alarm_record))//加载报警信息
		return 1;

	alarm_->device_type = m_dump_center.load_device_type(mtx_ptr, alarm_->device_id);

	if(alarm_->device_type.empty())
		return 2;

	str = m_dump_center.load_alarm_mapping(mtx_ptr, alarm_->alarm_id, alarm_->device_type);

	if (!alarm_->load_mapping(str))
		return 3;

	return 0;
}
//预加载app ，app-api-map , app-device-map三张表的数据
bool http::http_dump_config::pre_load_app()
{
	std::list<std::string> data_list;
	http::mongo_context * mtx = m_dump_center.get_mongo_context();

	bool ret = m_dump_center.select_mongo_muti_record(mtx,
		[&data_list](mongo::BSONObj va)
	{
		data_list.push_back(va.jsonString());
	},
		mongo_app_table,
		mongo::Query());
	mongo::BSONObj obj;
	for (std::list<std::string>::iterator it = data_list.begin();
		it!=data_list.end();it++)
	{
		save_user(*it,mtx);
	}

	m_dump_center.return_mongo_context(mtx);
 
	return false;
}

//预加载app ，app-api-map , app-device-map三张表的数据
bool http::http_dump_config::pre_load_api()
{
	http::mongo_context * mtx = m_dump_center.get_mongo_context();

	class call_back_imp
	{
	public:
		call_back_imp(std::map<int,std::string> * call_ptr)
		{
			call_ptr_ = call_ptr;
		}
		void operator()(mongo::BSONObj va)
		{
			if (va.isValid())
			{
				int apiid = atol(va.getStringField("apiid"));
				std::string apiname = va.getStringField("apiname");
				call_ptr_->insert(std::make_pair(apiid,apiname));
			}
		}
	private:
		std::map<int,std::string> * call_ptr_;
	};

	call_back_imp call(&api_map_);
	bool ret = m_dump_center.select_mongo_muti_record(mtx,call,
		mongo_api_table,
		mongo::Query());

	m_dump_center.return_mongo_context(mtx);
	return true;
}

bool http::http_dump_config::save_user(std::string json_record,void * mongo_ptr)
{
	http::mongo_context * mtx = (http::mongo_context *)mongo_ptr;
	if (NULL == mtx)
	{
		return false;
	}

	mongo::BSONObj obj = http::http_static_api::safe_fromjson(json_record);
	if (obj.isValid() && obj.hasField("appid") && obj.hasField("apppassword"))
	{
		std::string appid = obj.getStringField("appid");
		if (find_app(appid))
		{
			app_map_[appid].api_list_.clear();
			app_map_[appid].device_list_.clear();
		}
		else
		{
			app_map_.insert(std::make_pair(appid,http::app_obj()));
		}
		app_map_[appid].base.appid_      = appid;
		app_map_[appid].base.apppwd_     = obj.getStringField("apppassword");
		app_map_[appid].base.timelimit_  = obj.getStringField("timelimit");
		app_map_[appid].base.verifycode_ = obj.getStringField("verifycode");

		std::map<std::string,std::string> * ptr_list_ = &app_map_[appid].api_list_;
		m_dump_center.select_mongo_muti_record(mtx,
			[&ptr_list_](mongo::BSONObj va)
		{
			if (va.isValid())
			{
				ptr_list_->insert(std::make_pair(va.getStringField("apiid"),""));
			}
		},
			mongo_app_api_map_table,
			QUERY("appid"<<appid));

		ptr_list_ = &app_map_[appid].device_list_;
		m_dump_center.select_mongo_muti_record(mtx,
			[&ptr_list_](mongo::BSONObj va)
		{
			if (va.isValid())
			{
				ptr_list_->insert(std::make_pair(va.getStringField("vid"),""));
			}
		},
			mongo_app_device_map_table,
			QUERY("appid"<<appid));
	}
	return true;
}

std::string http::http_page_manager::end(void * data)
{
    mongo::BSONArrayBuilder * va = (mongo::BSONArrayBuilder *)data;

    if(va)
    {
        return BSON("tpages" << tpage_ << "cpage" << cpage_ << "tsize" << tsize_
                    << "csize" << csize_ << "valuelist" << va->arr()).jsonString();
    }

    return "";
};

int http::http_worker::check_auth(std::string verifycode, std::string api, std::string deviceid)
{
    int ret = error_map::error_ok;
    http::redis_context * rtx = m_dump_center.get_redis_context();
    http::mongo_context * mtx = m_dump_center.get_mongo_context();

	int state = 0;
	int verifycode_ttl =0; 
	mongo::BSONObj obj;
	std::string json_record = "";
	while(ret==error_map::error_ok)
	{
		json_record = "";
		switch(state)
		{
		case 0:
			state = 1;
			if (verifycode.empty())
			{
				ret =  error_map::error_verifycode;
			}
			if (api.empty())
			{
				ret =  error_map::error_api;
			}
			break;
		case 1:
			state =2;
			if(!m_dump_center.redis_exists_key(rtx, verifycode))//访问令牌是否存在
			{
				ret =  error_map::error_no_verifycode;
			}
			break;
		case 2://访问令牌验证
			state =3;
			{
				app_obj::app_base app;
				if (m_dump_config.find_app(verifycode,&app))//从预加载数据中查找
				{
					m_appid = app.appid_;
					verifycode_ttl = atol(app.timelimit_.c_str());
				}
				else
				{
					//查找appid是否在用户表存在 mongo
					json_record = m_dump_center.select_mongo_one_record(mtx, mongo_app_table, BSON("verifycode" << verifycode));
					obj = http::http_static_api::safe_fromjson(json_record);
					if(obj.isValid())
					{
						ret =  error_map::error_no_appid;
					}
					else
					{
						m_appid = obj.getStringField("appid");
						verifycode_ttl = atol(obj.getStringField("timelimit"));
						m_dump_config.save_user(json_record);
					}
				}
			}
			break;
		case 3://权限判断
			state = 4;
			if (!m_dump_config.app_has_api(m_appid,api))
			{
				if(0 == m_dump_center.count_mongo_data(mtx, mongo_app_api_map_table, BSON("appid" << m_appid << "apiid" << api)))
				{
					ret =  error_map::error_no_api;
				}else
				{
					m_dump_config.app_add_api(m_appid,api);
				}
			}
			break;
		case 4://设备校验
			state = 5;
			if(deviceid.size()>0)
			{
				m_devices_list.clear();
				http_static_api::get_device_list(deviceid, &m_devices_list);
				std::list<std::string>::iterator it;

				for(it = m_devices_list.begin(); it != m_devices_list.end(); it++)
				{
					if (!m_dump_config.app_has_device(m_appid,*it))
					{
						ret =  error_map::error_device_list;
						break;
					}

					mongo::BSONObjBuilder bb;
					bb << "appid" << m_appid << "vid" << *it;

					if(0 == m_dump_center.count_mongo_data(mtx, mongo_app_device_map_table, bb.obj()))
					{
						ret =  error_map::error_device_list;
					}
					else
					{
						m_dump_config.app_add_device(m_appid,*it);
					}
				}
			}
			break;
		case 5:
			state = INT_MAX;
			if (verifycode.size()>0 && m_appid.size()>0)
			{
				//刷新verifycode的有效期
				m_dump_center.set_redis_key_ttl(rtx, verifycode, verifycode_ttl);
			}
			break;
		}
		if (state == INT_MAX)
		{
			break;
		}
	}

    if(rtx) m_dump_center.return_redis_context(rtx);

    if(mtx) m_dump_center.return_mongo_context(mtx);

    return ret;
}

void http::http_worker::process_file()
{
    if(m_file_manager->load_file("index.html"))
    {
        m_reply_.http_status       = http_reply::ok;
        m_reply_.content      = m_file_manager->get_content();
        m_reply_.content_type = m_file_manager->get_type();
    }
    else
    {
        m_reply_ = http_reply::stock_reply(http_reply::not_found);
    }
}

void http::http_worker::process_oauth_appAuthorize()
{
    std::string appid  =  get_param("appid");
    std::string apppwd =  get_param("apppassword");
    http::mongo_context * mtx = m_dump_center.get_mongo_context();
    http::redis_context * rtx = m_dump_center.get_redis_context();

	app_obj::app_base app;
	int state = 0;//状态机
	mongo::BSONObj obj;
	std::string json_record = "";
	m_reply_.status = error_map::error_ok;
	while(m_reply_.status==error_map::error_ok)
	{
		json_record = "";
		switch(state)
		{
		case 0:
			state = 1;
			if(appid.empty() || apppwd.empty())
			{
				m_reply_.status = error_map::error_appid_or_pw;
			}
			break;
		case 1:
			state = 2;
			{
				if (m_dump_config.find_app(appid, apppwd, &app))
				{
					state = 110;
				}
			}
			break;
		case 2:
			state = 110;
			json_record = m_dump_center.select_mongo_one_record(mtx,mongo_app_table,BSON("appid"<<appid<<"apppassword"<<apppwd));
			obj = http::http_static_api::safe_fromjson(json_record);
			if (json_record.size()>0 && obj.isValid())
			{
				app.verifycode_ = obj.getStringField("verifycode");
				app.timelimit_  = obj.getStringField("timelimit");
				m_dump_config.save_user(json_record,mtx);
			}
			else
			{
				state = 321;//用户不存在
			}
			break;
		case 110://用户存在,存在访问令牌
			state = 123;
			if(m_dump_center.redis_exists_key(rtx, app.verifycode_))//key存在
			{
				m_dump_center.set_redis_key_ttl(rtx, app.verifycode_, atol(app.timelimit_.c_str()));//ttl刷新
			}
			else
			{
				state = 111;
			}
			break;
		case 111://用户存在,不存在访问令牌
			state = 321;
			app.verifycode_ = auth_manager::create_verifycode(appid, apppwd);
			if (app.verifycode_.size()>0)
			{
				if(m_dump_center.updata_app_verifycode(mtx, app.verifycode_, appid, apppwd) && //save verifycode
					m_dump_center.save_verifycode(rtx, app.verifycode_, appid, apppwd, atol(app.timelimit_.c_str()))) //save and set ttl
				{
					//reload
					json_record = m_dump_center.select_mongo_one_record(mtx,mongo_app_table,BSON("appid"<<appid<<"apppassword"<<apppwd));
					m_dump_config.save_user(json_record,mtx);
					state = 123;
				}
			}
			break;
		case 123://ok
			m_reply_.result = BSON("verifycode" << app.verifycode_ << "expires_in" <<  atol(app.timelimit_.c_str())).jsonString();
			m_reply_.status = error_map::error_ok;
			state = INT_MAX;
			break;
		case 321://error
			m_reply_.status = error_map::error_appid_or_pw;
			break;
		}

		if (state ==INT_MAX)//处理成功消息
		{
			break;
		}
	}

    if(mtx) m_dump_center.return_mongo_context(mtx);

    if(rtx) m_dump_center.return_redis_context(rtx);
}

void http::http_worker::process_gps_getDeviceList()
{
    http_page_manager pager_manager;
    http::mongo_context * mtx = m_dump_center.get_mongo_context();
    int data_count = m_dump_center.count_mongo_data(mtx, mongo_app_device_map_table, BSON("appid" << m_appid));
    mongo::BSONArrayBuilder value_builder;

    if(pager_manager.start(data_count, atol(get_param("cpage").c_str()), atol(get_param("pagesize").c_str())))
    {
        std::list<std::string> data_list;
		m_dump_center.select_mongo_muti_record(mtx,
			[&data_list](mongo::BSONObj va)
		{
			if (va.isValid())
			{
				data_list.push_back(va.jsonString());
			}
		},
			mongo_app_device_map_table,
			QUERY("appid" << m_appid),&BSON("vid" << 1 << "_id" << 0),
			pager_manager.get_off_record_size(), pager_manager.get_current_size());

        std::string device_id = "";

        for(std::list<std::string>::iterator it = data_list.begin();
                it != data_list.end(); it++)
        {
			mongo::BSONObj obj = http::http_static_api::safe_fromjson(*it);

			if(obj.isEmpty())
			{
				continue;
			}

			device_id = obj.getStringField("vid");

			if(!device_id.empty())
			{
				std::string device_type = m_dump_center.load_device_type(mtx, device_id);
				value_builder.append(BSON("deviceid" << device_id << "devicetype" << device_type));
			}
        }
    }

    if((m_reply_.result = pager_manager.end((void*)&value_builder)).empty())
        m_reply_.status = error_map::error_unknow;
    else
        m_reply_.status = error_map::error_ok;

    if(mtx) m_dump_center.return_mongo_context(mtx);
}

void http::http_worker::process_gps_getDeviceGPS()
{
    std::string str = "";
    http_page_manager pager_manager;
    int loadmapaddinfo = atol(get_param("loadmapaddinfo").c_str());
    http::mongo_context * mtx = m_dump_center.get_mongo_context();
    http::redis_context * rtx = m_dump_center.get_redis_context();
    mongo::BSONArrayBuilder values_builder;
	std::string device = "";
	if (m_devices_list.size()>0)
	{
		device = m_devices_list.front();
	}

    if(pager_manager.start(m_devices_list.size(), atol(get_param("cpage").c_str()), atol(get_param("pagesize").c_str())))
    {
        m_devices_list.clear();
        http_static_api::get_device_list(device, &m_devices_list,
                                       pager_manager.get_off_record_size(),
                                       pager_manager.get_current_size());
        struct device_map device_map_;

        for(std::list<std::string>::iterator it = m_devices_list.begin();
                it != m_devices_list.end(); it++)
        {
            device_map_.start(*it, get_param("map"), loadmapaddinfo == 1);
            str = m_dump_center.select_mongo_one_record(mtx, mongo_pos_month_table,
                    BSON("vid" << device_map_.device_id));
			device_map_.run(str);
			device_map_.end(&values_builder);
        }
    }

    if((m_reply_.result = pager_manager.end((void*)&values_builder)).empty())
        m_reply_.status = error_map::error_unknow;
    else
        m_reply_.status = error_map::error_ok;

    if(mtx) m_dump_center.return_mongo_context(mtx);

    if(rtx) m_dump_center.return_redis_context(rtx);
}

//一个id，单个查询
void http::http_worker::process_gps_getDeviceRoute()
{
    int utc_stime = atol(get_param("starttime").c_str());
    int utc_etime = atol(get_param("endtime").c_str());
	if (utc_etime>utc_stime)
	{
		getDeviceRoute(utc_stime,utc_etime);
	}
	else
	{
		m_reply_.status = error_map::error_datetime;
	}
}

void http::http_worker::process_gps_getDeviceDayRoute()
{
	std::string day = get_param("day");
	std::string starttime = get_param("starttime");
	std::string endtime = get_param("endtime");

	if (timer_manager::is_format_data(day) && timer_manager::is_format_time(starttime) && 
		timer_manager::is_format_time(starttime))
	{
		std::string str = day + " " + starttime;
		int utc_stime = timer_manager::str_to_datetime(str);
		str = day + " " + endtime;
		int utc_etime = timer_manager::str_to_datetime(str);
		if (utc_etime>utc_stime)
		{
			getDeviceRoute(utc_stime,utc_etime);
		}
		else
		{
			m_reply_.status = error_map::error_datetime;
		}
	}
	else
	{
		m_reply_.status = error_map::error_datetime;
	}
}

void http::http_worker::process_alarm_getAppAlarm()
{
// 	http_page_manager pager_manager;
// 	http::mongo_context * mtx = m_dump_center.get_mongo_context();
// 
// 	mongo::BSONArrayBuilder arrbuilder;
// 	m_dump_center.select_mongo_muti_record(mtx,
// 		[&arrbuilder](mongo::BSONObj va)
// 	{
// 		std::string vid = va.getStringField("vid");
// 		if (va.isValid() && vid.size()>0)
// 		{
// 			arrbuilder<<vid;
// 		}
// 	},
// 		mongo_app_device_map_table,
// 		QUERY("appid" << m_appid));
// 
// 	int data_count = m_dump_center.count_mongo_data(mtx, mongo_app_device_map_table, BSON("vid" << (BSON("$in"<<arrbuilder.arr()))));
// 	mongo::BSONArrayBuilder values_builder;
// 	if(pager_manager.start(data_count, atol(get_param("cpage").c_str()), atol(get_param("pagesize").c_str())))
// 	{
// 		std::list<std::string> data_list;
// 		m_dump_center.select_mongo_muti_record(mtx,
// 			[&data_list](mongo::BSONObj va)
// 		{
// 			if (va.isValid())
// 			{
// 				data_list.push_back(va.jsonString());
// 			}
// 		},
// 			mongo_alarm_table,
// 			BSON("vid" << deviceid << "posttime" << mongo::GTE << utc_stime << mongo::LTE << utc_etime),
// 			&BSON("_id" << 0),
// 			pager_manager.get_off_record_size(), pager_manager.get_current_size());
// 
// 		int error_id = 0;
// 		std::string str = "";
// 		struct device_alarm alarm;
// 
// 		for(std::list<std::string>::iterator it = data_list.begin();
// 			it != data_list.end(); it++)
// 		{
// 			alarm.reset();
// 			alarm.device_id = deviceid;
// 			if (!(error_id = process_device_alarm(mtx,*it,&alarm)))
// 			{
// 				values_builder.append(BSON("alarm_id" << alarm.alarm_id << "cn"
// 					<< alarm.cn << "en" << alarm.en << "pt" << timer_manager::datetime_to_str(alarm.pt)));
// 			}
// 			else
// 			{
// 				values_builder.append(BSON("error_id" << error_id));
// 			}
// 		}
// 	}
// 
// 	int data_count = m_dump_center.count_mongo_data(mtx, mongo_alarm_table, BSON("vid" << deviceid
// 		<< "posttime" << mongo::GTE << utc_stime << mongo::LTE << utc_etime));
// 
// 	if((m_reply_.result = pager_manager.end((void*)&values_builder)).empty())
// 		m_reply_.status = error_map::error_unknow;
// 	else
// 		m_reply_.status = error_map::error_ok;
// 
// 	if(mtx) m_dump_center.return_mongo_context(mtx);
}

void http::http_worker::process_alarm_getDeviceAlarm()
{
    http_page_manager pager_manager;
    http::mongo_context * mtx = m_dump_center.get_mongo_context();
    http::redis_context * rtx = m_dump_center.get_redis_context();

	std::string deviceid = m_devices_list.front();
    
	mongo::BSONArrayBuilder values_builder;
    if(pager_manager.start(m_devices_list.size(), atol(get_param("cpage").c_str()), atol(get_param("pagesize").c_str())))
    {
        m_devices_list.clear();
        http_static_api::get_device_list(deviceid, &m_devices_list,
                                       pager_manager.get_off_record_size(),
                                       pager_manager.get_current_size());
        std::list<std::string> data_list;

        for(std::list<std::string>::iterator it = m_devices_list.begin();
                it != m_devices_list.end(); it++)
        {
            m_dump_center.load_last_alarm(mtx, *it, &data_list);
        }

		int error_id = 0;
        std::string str = "";
        struct device_alarm alarm;

        for(std::list<std::string>::const_iterator it = data_list.begin();
                it != data_list.end(); it++)
        {
            alarm.reset();
			if (!(error_id = process_device_alarm(mtx,*it,&alarm)))
			{
				values_builder.append(BSON("alarm_id" << alarm.alarm_id << "cn"
					<< alarm.cn << "en" << alarm.en << "pt" << timer_manager::datetime_to_str(alarm.pt)));
			}
			else
			{
				values_builder.append(BSON("error_id" << error_id));
			}
        }
    }

    if((m_reply_.result = pager_manager.end((void*)&values_builder)).empty())
        m_reply_.status = error_map::error_unknow;
    else
        m_reply_.status = error_map::error_ok;

    if(mtx) m_dump_center.return_mongo_context(mtx);

    if(rtx) m_dump_center.return_redis_context(rtx);
}

void http::http_worker::process_alarm_getDeviceHisAlarm()
{
    http_page_manager pager_manager;
    http::mongo_context * mtx = m_dump_center.get_mongo_context();
    http::redis_context * rtx = m_dump_center.get_redis_context();
    int utc_stime = atol(get_param("starttime").c_str());
    int utc_etime = atol(get_param("endtime").c_str());
	std::string deviceid = m_devices_list.front();
    int data_count = m_dump_center.count_mongo_data(mtx, mongo_alarm_table, BSON("vid" << deviceid
                     << "posttime" << mongo::GTE << utc_stime << mongo::LTE << utc_etime));
    mongo::BSONArrayBuilder values_builder;

    if(pager_manager.start(data_count, atol(get_param("cpage").c_str()), atol(get_param("pagesize").c_str())))
    {
        std::list<std::string> data_list;
		m_dump_center.select_mongo_muti_record(mtx,
			[&data_list](mongo::BSONObj va)
		{
			if (va.isValid())
			{
				data_list.push_back(va.jsonString());
			}
		},
			mongo_alarm_table,
			BSON("vid" << deviceid << "posttime" << mongo::GTE << utc_stime << mongo::LTE << utc_etime),
			&BSON("_id" << 0),
			pager_manager.get_off_record_size(), pager_manager.get_current_size());

		int error_id = 0;
		std::string str = "";
		struct device_alarm alarm;

        for(std::list<std::string>::iterator it = data_list.begin();
                it != data_list.end(); it++)
        {
            alarm.reset();
            alarm.device_id = deviceid;
			if (!(error_id = process_device_alarm(mtx,*it,&alarm)))
			{
				values_builder.append(BSON("alarm_id" << alarm.alarm_id << "cn"
					<< alarm.cn << "en" << alarm.en << "pt" << timer_manager::datetime_to_str(alarm.pt)));
			}
			else
			{
				values_builder.append(BSON("error_id" << error_id));
			}
        }
    }

    if((m_reply_.result = pager_manager.end((void*)&values_builder)).empty())
        m_reply_.status = error_map::error_unknow;
    else
        m_reply_.status = error_map::error_ok;

    if(mtx) m_dump_center.return_mongo_context(mtx);

    if(rtx) m_dump_center.return_redis_context(rtx);
}

void http::http_worker::dispatch_method(std::string method)
{
	//load method from int to string
	log_manager::LOG("dispatch_method",method);
	std::string api_fun_str = m_dump_config.get_api_name(atol(method.c_str()));
	if(api_fun_str.empty())
	{
		//load from db
		http::mongo_context * mtx = m_dump_center.get_mongo_context();
		api_fun_str = m_dump_center.load_api_fun_name(mtx,method);
		if (api_fun_str.size()>0)
		{
			m_dump_config.api_add(atol(method.c_str()),api_fun_str);
		}
		else
		{
			method = "";//权限不存在
		}
		if(mtx) m_dump_center.return_mongo_context(mtx);
	}

	if(0 == api_fun_str.compare("/oauth/appAuthorize"))
	{
		process_oauth_appAuthorize();
		return;
	}
	
	std::string verifycode = get_param("verifycode");
	std::string deviceid = get_param("deviceid");

	//m_apiid = api_fun_str;
	if (error_map::error_ok!=(m_reply_.status = check_auth(verifycode,method,deviceid)))
	{
		return;
	}

	if(0 == api_fun_str.compare("/gps/getDeviceList"))
    {
        process_gps_getDeviceList();
    }
    else if(0 == api_fun_str.compare("/gps/getDeviceGPS"))
    {
        process_gps_getDeviceGPS();
    }
    else if(0 == api_fun_str.compare("/gps/getDeviceRoute"))
    {
        process_gps_getDeviceRoute();
    }
	else if(0 == api_fun_str.compare("/gps/getDeviceDayRoute"))
	{
		process_gps_getDeviceDayRoute();
	}
    else if(0 == api_fun_str.compare("/alarm/getDeviceAlarm"))
    {
        process_alarm_getDeviceAlarm();
    }
    else if(0 == api_fun_str.compare("/alarm/getDeviceHisAlarm"))
    {
        process_alarm_getDeviceHisAlarm();
    }
}

void http::http_worker::pocket_reply_body()
{
    if(m_reply_.http_status == http_reply::ok)
    {
        mongo::BSONObjBuilder http_bb;
        m_reply_.result_str = error_map::to_buffer(m_reply_.status);
        http_bb << "state" << m_reply_.status;
        http_bb << "result_str" << m_reply_.result_str;
        http_bb << "result" << mongo::fromjson(m_reply_.result);
		m_reply_.content = http_bb.obj().jsonString();
		//m_reply_.content_type = "application/json;charset=gbk";
		//m_reply_.content = http_string_encode::gbk_2_utf8(http_bb.obj().jsonString());
        m_reply_.content_type = "application/json;charset=utf-8";
    }
}

void http::http_worker::getDeviceRoute(int utc_stime,int utc_etime)
{
	http_page_manager pager_manager;
	http::mongo_context * mtx = m_dump_center.get_mongo_context();
	http::redis_context * rtx = m_dump_center.get_redis_context();

	std::string sTable = mongo_pos_month_table;

	std::string device = m_devices_list.front();

	if(timer_manager::morethan_one_month(utc_stime,utc_etime))
		sTable = mongo_pos_table;

	int data_count = m_dump_center.count_mongo_data(mtx, sTable, BSON("vid" << device
		<< "ts" << mongo::GTE << utc_stime << mongo::LTE << utc_etime));
	mongo::BSONArrayBuilder values_builder;

	if(pager_manager.start(data_count, atol(get_param("cpage").c_str()), atol(get_param("pagesize").c_str())))
	{
		std::list<std::string> data_list;
		m_dump_center.select_mongo_muti_record(mtx,
			[&data_list](mongo::BSONObj va)
		{
			if (va.isValid())
			{
				data_list.push_back(va.jsonString());
			}
		},
			sTable,
			BSON("vid" << device << "ts" << mongo::GTE << utc_stime << mongo::LTE << utc_etime),
			&BSON("_id" << 0),
			pager_manager.get_off_record_size(), pager_manager.get_current_size());

		//rename field
		struct device_map device_map_;

		while(!data_list.empty())
		{
			device_map_.reset();
			device_map_.start(device, get_param("map"));
			device_map_.run(data_list.front());
			device_map_.end(&values_builder);
			data_list.pop_front();
		}
	}

	if((m_reply_.result = pager_manager.end((void*)&values_builder)).empty())
		m_reply_.status = error_map::error_unknow;
	else
		m_reply_.status = error_map::error_ok;

	if(mtx) m_dump_center.return_mongo_context(mtx);

	if(rtx) m_dump_center.return_redis_context(rtx);
}

extern "C" void http_server_run(std::string config)
{
    if(!m_dump_config.load_config(config))
    {
        printf("load config failed!\n");
        return;
    }

    if(!m_dump_center.create_mongo_context(m_dump_config.mongo_host, m_dump_config.mongo_auth,m_dump_config.mongo_num))
    {
        printf("mongodb create failed!\n");
        return;
    }

    if(!m_dump_center.create_redis_context(m_dump_config.redis_host, m_dump_config.redis_port))
    {
        printf("redis create failed!\n");
        return;
    }

	m_dump_config.pre_load();

    m_file_manager = new http::http_file_manager("./web/");

	yux::standard_worker_factory<http::http_worker> slaver_factory(m_dump_config.factory_size);
	slaver_factory.set_register_mode(true);
	if (m_dump_config.sweep_interval>0){
		http::factory_cleaner cleaner(slaver_factory);
		cleaner.start_sweep(m_dump_config.sweep_interval);
	}

    yux::asio_tcp_server server(m_dump_config.listen_port, slaver_factory.get_accept_handler(), m_dump_config.thread_num);
    server.start();
    server.join();
}