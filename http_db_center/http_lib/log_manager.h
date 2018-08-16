#ifndef _LOG_MANAGER_H_
#define _LOG_MANAGER_H_

#include "boost/date_time.hpp"

class log_manager
{
public:

#define LOG(msg,err) print(msg,err)
#define ERROR_LOG(msg) print("error",msg)
#define SYSTEM_LOG(msg) print("system",msg)



	static void print(const char *label,const std::string msg,bool hex_debug=true)
	{
		static boost::mutex mutex;
		boost::mutex::scoped_lock lock(mutex);
		FILE *pf=fopen("debug.log","a+");
		if (pf)
		{
			boost::posix_time::ptime pt=boost::posix_time::second_clock::local_time();
			fprintf(pf,"[%s] --> %s -->",label,boost::posix_time::to_iso_extended_string(pt).c_str());
			if (hex_debug){
				fprintf(pf,"[ hex-dump ]");
				for (size_t i=0;i<msg.size();i++){
					if (i%10==0){
						fprintf(pf,"\n");
					}
					fprintf(pf,"%02X ",(unsigned char)msg[i]);
				}
			}else{
				fprintf(pf,"%s",msg.c_str());
			}
			fprintf(pf,"\n");
			fclose(pf);
		}
	}
};


#endif