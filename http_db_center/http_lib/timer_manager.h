#ifndef _TIMER_MANAGER_H_
#define _TIMER_MANAGER_H_

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <boost/timer.hpp>
#include <boost/date_time.hpp>
#include <boost/regex.hpp>
#include <boost/regex.h>

namespace timer_manager
{
	static bool morethan_one_month(time_t utc_stime,time_t utc_etime)
	{
		try
		{
			struct tm p_s;
			localtime_s(&p_s, &utc_stime);
			struct tm p_e;
			localtime_s(&p_e, &utc_etime);

			if (p_e.tm_year > p_s.tm_year)
			{
				return true;
			}else if( (p_e.tm_year == p_s.tm_year) && (p_e.tm_mon > p_s.tm_mon))
			{
				return true;
			}
		}
		catch (...)
		{
		}
		return false;
	}

	//输出：2015-12-29 15:36:08
	static std::string datetime_to_str(time_t tm)
	{
		try
		{
			time_t tm_t  = tm ;//时区减去8小时，因为存的时候加了8小时- 28800;
			std::string str = "";
			if (tm==-1)
			{
				return "";
			}

			struct tm *p;
			p=localtime(&tm_t);

			char buf[64] = {0};

			sprintf(buf,"%04d-%02d-%02d %02d:%02d:%02d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,
				p->tm_hour,p->tm_min,p->tm_sec);
			str = buf;
			return str;
		}
		catch (...)
		{
		}
		return "";
	}

	//输入：2015-12-29 15:36:08
	static time_t str_to_datetime(std::string tm_str)
	{
		try
		{
			time_t tt = 0;
			struct tm p = {0};

			sscanf(tm_str.c_str(),"%04d-%02d-%02d %02d:%02d:%02d",&p.tm_year,&p.tm_mon,&p.tm_mday,
				&p.tm_hour,&p.tm_min,&p.tm_sec);
			p.tm_year -= 1900;
			p.tm_mon  -= 1;
			tt = mktime(&p);
			return tt;
		}
		catch (...)
		{
		}
		return -1;
	}

	//输出："2015-11-01"
	static std::string date_to_str(int year,int mon,int day)
	{
		boost::gregorian::date d;
		try
		{
			d = boost::gregorian::date(year,mon,day);
			return boost::gregorian::to_iso_extended_string(d);
		}catch(std::exception &e){
			printf("mongo delete error! --> %s.\n", e.what());
		}
		return "";
	}

	//输出："11:22:33"
	static std::string time_to_str(int hour,int minute,int second)
	{
		boost::posix_time::time_duration t;
		std::stringstream s_stream;
		try
		{
			t = boost::posix_time::time_duration(hour,minute,second);
			s_stream << t;
			return s_stream.str();
		}catch(std::exception &e){
			printf("mongo delete error! --> %s.\n", e.what());
		}
		return "";
	}

	//"2015-11-11"
	static bool is_format_data(std::string str)
	{
		try
		{
			boost::regex reg( "^[0-9]{4}-(((0[13578]|(10|12))-(0[1-9]|[1-2][0-9]|3[0-1]))|(02-(0[1-9]|[1-2][0-9]))|((0[469]|11)-(0[1-9]|[1-2][0-9]|30)))$" );
			bool r=boost::regex_match( str , reg);
			if (r)//是否匹配
			{
				return true;
			}
		}catch(std::exception &e){
			printf("mongo delete error! --> %s.\n", e.what());
		}
		return false;
	}

	//"2015-11-11"
	static bool is_format_time(std::string str)
	{
		try
		{
			boost::regex reg( "^(([0-1]?[0-9])|([2][0-3])):([0-5]?[0-9])(:([0-5]?[0-9]))?$" );
			bool r=boost::regex_match( str , reg);
			if (r)//是否匹配
			{
				return true;
			}
		}catch(std::exception &e){
			printf("mongo delete error! --> %s.\n", e.what());
		}
		return false;
	}
	//获得服务器当前时间，精确到毫秒级
	static int64_t now_timestamp(int type=0)
	{
		boost::posix_time::ptime pt = boost::posix_time::microsec_clock::universal_time();
		boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
		boost::posix_time::time_duration duration=pt-epoch;
		switch(type)
		{
		case 0:
			return duration.total_seconds();//秒
		case 1:
			return duration.total_milliseconds();//毫秒
		case 2:
			return duration.total_microseconds();//微秒
		}
		return duration.total_seconds();//秒
	}  

	class run_timer{
		int64_t m_tc;
		std::string m_name;
	public:
		run_timer(char * name = "")
			:m_name(name)
		{
			m_tc = now_timestamp(1);
		}

		~run_timer()
		{
			int64_t endtm = now_timestamp(1);
			endtm -= m_tc;
			printf("%s系统运行耗时：%d毫秒\r\n",m_name.c_str(),endtm);
		}
	};
}

#endif