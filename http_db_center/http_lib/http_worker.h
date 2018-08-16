#ifndef HTTP_WORKER_H
#define HTTP_WORKER_H

#include <string.h>
#include <boost/lexical_cast.hpp>

#include "yux/asio_tcp_worker.hpp"
#include "yux/asio_tcp_server.hpp"
#include "yux/worker_accessor.hpp"
#include "yux/standard_worker_factory.hpp"

#include "http_reply.h"
#include "http_processor.h"
#include "timer_manager.h"
#include "log_manager.h"

struct error_map 
{
public:
	enum{
		error_unknow = -1,//未知的错误
		error_ok = 0,
		error_param,//参数错误
		error_appid_or_pw,//用户名或密码有误
		error_verifycode,//访问令牌错误
		error_api,      //api错误
		error_no_verifycode,//令牌不存在，过期或不存在这个令牌
		error_no_appid,     //令牌关联的用户不存在
		error_no_api,       //app没有权限
		error_device_list,    //设备列表有误
		error_create_verifycode,//生成令牌失败
		error_datetime,//错误的日期时间
	};

	static std::string to_buffer(int err)
	{
		std::string str_error = "";

		switch(err)
		{
		case error_unknow:
			str_error = "unknown error";
			break;
		case error_ok:
			str_error = "sucess";
			break;
		case error_param:
			str_error = "parameter error";
			break;
		case error_appid_or_pw:
			str_error = "username or password error";
			break;
		case error_verifycode:
		    str_error = "access token error";
			break;
		case error_api:
			str_error = "api id error";
			break;
		case error_no_verifycode:
			str_error = "access token does not exist,expired or not exist";
			break;
		case error_no_api:
			str_error = "app no rights";
			break;
		case error_device_list:
			str_error = "device error";
			break;
		case error_create_verifycode:
			str_error = "failed to create token";
			break;
		case error_datetime:
			str_error = "datatime error";
			break;
		default:
			str_error = "undefined error";
		}

		return str_error;
	}
};

namespace http
{
    class http_worker: public yux::asio_tcp_worker<http_worker>
    {
        friend class yux::worker_accessor<http_worker>;

	public:
		http_worker():page_(zone()){

		}
	
    private:
        void handle_start()
        {
            printf("WOKRER%d IS START!!!\n");
			log_manager::SYSTEM_LOG("WOKRER%d IS START!!!\n");
        }

        bool handle_read(const char *data_ptr, int data_size)
        {
			int data_pos=0;
			std::string msg(data_ptr,data_size);
			log_manager::LOG("handle_read",msg);

			//包长度大于2000，抛掉
			if (data_size>MAX_PACKET_SIZE)
			{
				printf("throw packet bigger than %d!\n",MAX_PACKET_SIZE);
				return true;
			}
			while (true){
				m_processor_.reset();
				if (!m_processor_.feed(data_ptr,data_size,data_pos)){
					break;
				}
				if (data_pos==0){
					printf("feed error!\n");
					break;
				}

 				process_req();
				
				if (data_pos==data_size){
					break;
				}
				data_ptr+=data_pos;
				data_size-=data_pos;
			}
			return data_pos!=0;
			//return false;//一次连接就断开
        }

        void handle_stop()
        {
            printf("WORKER%d IS STOP!!!\n");
			log_manager::SYSTEM_LOG("WOKRER%d IS START!!!\n");
        }

		void process_req()
		{
			std::string apiid = m_processor_.get_uri_method();
			timer_manager::run_timer run_t("process_req");
			if (!strcmp(m_processor_.get_method().c_str(),"POST")
				|| !strcmp(m_processor_.get_method().c_str(),"GET"))
			{
				m_reply_.http_status =http_reply::ok;
				
				if (apiid.empty())
				{
					m_reply_.status = error_map::error_api;
				}else
				{
					dispatch_method(apiid);
				}
			}
			else
			{
				m_reply_.http_status =http_reply::bad_request;
			}
			
			render_page();
		}
		int check_auth(std::string verifycode,std::string api,std::string deviceID);
		void dispatch_method(std::string method);
		void process_file();
		void process_oauth_appAuthorize();
		void process_gps_getDeviceList();
		void process_gps_getDeviceGPS();
		void process_gps_getDeviceRoute();
		void process_gps_getDeviceDayRoute();
		void process_alarm_getAppAlarm();
		void process_alarm_getDeviceAlarm();
		void process_alarm_getDeviceHisAlarm();

		void getDeviceRoute(int utc_stime,int utc_etime);

		std::string get_param(const char *name)
		{
			return m_processor_.get_param(name);
		}

		void render_page()
		{
			pocket_reply_body();
			
			m_reply_.headers.resize(2);
			m_reply_.headers[0].name = "Content-Length";
			m_reply_.headers[0].value = boost::lexical_cast<std::string>(m_reply_.content.size());
			m_reply_.headers[1].name = "Content-Type";
			m_reply_.headers[1].value = m_reply_.content_type;
			std::string strReply = m_reply_.to_buffers();
			strReply+="\r\n\r\n";
// 			do_merge(strReply.c_str(),strReply.size());

			yux::pool_string str(zone());
			str += strReply;
			page_.append_to(str);
			do_merge(str);
//			do_reply(strReply.c_str(),strReply.size());
		}

		void pocket_reply_body();

	private:
		enum{MAX_PACKET_SIZE = 2000};


		http_processor m_processor_;

		http_reply m_reply_;

		std::string m_appid;

		std::list<std::string> m_devices_list;

		yux::pool_string page_;
    };

	class factory_cleaner{

		boost::thread *sweep_thread_ptr_;
		yux::standard_worker_factory<http_worker> &factory_;

	public:

		factory_cleaner(yux::standard_worker_factory<http_worker> &factory):factory_(factory){
			sweep_thread_ptr_=NULL;
		}

	private:

		static bool sweep_judge_function(http_worker *worker_ptr,int timeout,int timestamp){
			int nowtime=(int)time(NULL);
			return nowtime-worker_ptr->heartbeat()>timeout;
		}

		void sweep_routine(int timeout){
			int timestamp=(int)time(NULL);
			boost::function1<bool,http_worker *> sweep_judger=boost::bind(&factory_cleaner::sweep_judge_function,_1,timeout,timestamp);
			boost::function1<void,http_worker *> dismisser=boost::bind(&http_worker::do_dismiss,_1);
			while (true){
				boost::this_thread::sleep(boost::posix_time::seconds(timeout));
				factory_.for_each(sweep_judger,dismisser);
			}
		}

	public:

		void start_sweep(int timeout){
			if (!sweep_thread_ptr_){
				sweep_thread_ptr_=new boost::thread(boost::bind(&factory_cleaner::sweep_routine,this,timeout));
			}
		}

	};

};//http namespace

#endif