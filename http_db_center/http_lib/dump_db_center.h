#ifndef _DUMP_DB_CENTER_H_
#define _DUMP_DB_CENTER_H_

#include <boost/lexical_cast.hpp>

#include "log_manager.h"
#include "mongo/client/dbclient.h"
#include "hiredis/hiredis.h"

#define mongo_pos_table            "wang.platform_his_pos_data"
#define mongo_pos_month_table      "wang.platform_his_pos_data_month"
#define mongo_alarm_map_table      "wang.platform_alarm_map"
#define mongo_alarm_table          "wang.platform_his_alarm"
#define mongo_app_table            "wang.platform_app_authentication"
#define mongo_api_table            "wang.platform_api_list"
#define mongo_device_table         "wang.platform_terminal_list"
#define mongo_app_api_map_table    "wang.platform_app_permission_map"
#define mongo_app_device_map_table "wang.platform_app_terminal_map"

namespace http{

	struct mongo_context{
		std::string host;
		std::string username;
		std::string userpwd;
		std::string dbname;
		boost::scoped_ptr<mongo::DBClientConnection> conn_ptr;
		boost::mutex mutex;
		bool healthy;
	};

	struct redis_context{
		// for connect/reconnect
		std::string host;
		int port;
		std::string auth;
		// for write
		redisContext *ctx;
		boost::mutex mutex;
	};

	class dump_db_center
	{
		typedef mongo_context * mongo_context_ptr;

		bool create_mongo_context(std::string mongo_host,std::string mongo_auth,mongo_context & mongo_ctx)
		{
			std::string err;
			mongo_ctx.conn_ptr.reset(new mongo::DBClientConnection(false,0,30));
			// configure mongodb connection
			if (!mongo_ctx.conn_ptr->connect(mongo_host,err)){
				printf("mongodb connect error --> %s\n",err.c_str());
				return false;
			}
			mongo_ctx.host=mongo_host;
			// make mongodb connection available
			if (!mongo_auth.empty()){
				int colon_pos,at_pos;
				colon_pos=mongo_auth.find(':');
				at_pos=mongo_auth.rfind('@');
				if ((colon_pos==-1)||(at_pos==-1)||(at_pos<colon_pos)){
					printf("mongodb auth format error!\n");
					return false;
				}
				std::string mongo_dbname,mongo_username,mongo_pwd;
				mongo_dbname=mongo_auth.substr(at_pos+1);
				mongo_username=mongo_auth.substr(0,colon_pos);
				mongo_pwd=mongo_auth.substr(colon_pos+1,at_pos-colon_pos-1);
				if (!mongo_ctx.conn_ptr->auth(mongo_dbname,mongo_username,mongo_pwd,err)){
					printf("mongodb auth error --> %s --> %s:%s @ %s\n",err.c_str(),mongo_username.c_str()
						,mongo_pwd.c_str(),mongo_dbname.c_str());
					return false;
				}
				printf("mongodb authed ok!\n");
				mongo_ctx.username=mongo_username;
				mongo_ctx.userpwd=mongo_pwd;
				mongo_ctx.dbname=mongo_dbname;
			}
			mongo_ctx.healthy=true;
			return true;
		}
	public:
		bool create_mongo_context(std::string mongo_host,std::string mongo_auth,int mongonum = MAX_MONGO_CTX){
			std::string err;
			mongo_ctx_count = mongonum;
			mongo_ctx_no=0;
			mongo_ctx_array = new mongo_context_ptr[mongo_ctx_count];
			assert(mongo_ctx_array);
			for(int i =0;i<mongo_ctx_count;i++)
			{
				mongo_ctx_array[i]= new mongo_context();
				if (!create_mongo_context(mongo_host,mongo_auth,*mongo_ctx_array[i])){
					printf("what's wrong with mongodb?\n");
					return false;
				}
			}
			return true;
		}

		void reconnect_mongo(mongo_context *mongo_ctx_ptr)
		{
			if (!mongo_ctx_ptr->healthy){
				std::string err;
				mongo_ctx_ptr->conn_ptr.reset(new mongo::DBClientConnection(false,0,30));
				if (!mongo_ctx_ptr->conn_ptr->connect(mongo_ctx_ptr->host,err)){
					log_manager::LOG("mongodb connect failed",err);
					//throw std::runtime_error("mongodb connect failed");
				}
				if (!mongo_ctx_ptr->conn_ptr->auth(mongo_ctx_ptr->dbname,mongo_ctx_ptr->username,
					mongo_ctx_ptr->userpwd,err)){
						log_manager::LOG("mongodb auth failed",err);
						//throw std::runtime_error("mongodb auth failed");
				}
				mongo_ctx_ptr->healthy=true;
			}
		}

		int count_mongo_data(mongo_context *mongo_ctx_ptr,std::string table,mongo::BSONObj bo)
		{
			int i=0;
			unsigned long long num_ = 0;
			while(true){
				try
				{
					if (!mongo_ctx_ptr->healthy)
					{
						reconnect_mongo(mongo_ctx_ptr);
					}
					num_ =  mongo_ctx_ptr->conn_ptr->count(table,bo);
					break;
				}
				catch(std::exception &e)
				{
					log_manager::LOG("mongo count error",e.what());
					printf("mongo count error! --> %s.\n",e.what());
					mongo_ctx_ptr->healthy=false;
					i++;
					boost::this_thread::sleep(boost::posix_time::seconds(10));
					if (i==20){
						break;
					}
				}
			}
			
			return (int)num_;
		}

		bool select_mongo_distinct_field(mongo_context *mongo_ctx_ptr,std::string table)
		{
			int i=0;
			bool ret = false;
			while(true){
				try
				{
					std::vector<std::string> results;
					mongo::BSONObj cmdResult;
					if (!mongo_ctx_ptr->healthy)
					{
						reconnect_mongo(mongo_ctx_ptr);
					}
					bool ret = mongo_ctx_ptr->conn_ptr->runCommand("wang",BSON("distinct" << "sinolbs_his_pos_data" 
						<< "key" << "VID" <<"key"<< "TT" << "query" << BSON("TT"<<8809)),cmdResult);
					int ok = cmdResult.getIntField("ok");
					if (ret && cmdResult.isValid() && ok)
					{
						cmdResult.getObjectField("values").Vals(results);
					}
					ret = true;
					break;
				}
				catch(std::exception &e)
				{
					log_manager::LOG("select_mongo_distinct_field error",e.what());
					printf("select_mongo_distinct_field error! --> %s.\n",e.what());
					mongo_ctx_ptr->healthy=false;
					i++;
					boost::this_thread::sleep(boost::posix_time::seconds(10));
					if (i==20){
						break;
					}
				}
			}
			return ret;
		}

		//获取一条记录,可以返回想要的字段
		//condition的json字符串："{ query: { vid: "015899898985", dealresult: 0 }, orderby: { endtime: 1 } }"
		std::string select_mongo_one_record(mongo_context *mongo_ctx_ptr,std::string table,mongo::Query condition,
			const mongo::BSONObj *fieldsToReturn = 0)
		{
			int i=0;
			std::string value = "";
			while(true){
				try
				{
					mongo::BSONObj obj;
					if (!mongo_ctx_ptr->healthy)
					{
						reconnect_mongo(mongo_ctx_ptr);
					}
					obj = mongo_ctx_ptr->conn_ptr->findOne(table,condition,fieldsToReturn);
					if (!obj.isEmpty())
					{
						value = obj.jsonString();
						
					}
					break;
				}
				catch(std::exception &e)
				{
					printf("mongo_select_one error! --> %s.\n",e.what());
					log_manager::LOG("mongo_select_one error",e.what());
					mongo_ctx_ptr->healthy=false;
					i++;
					boost::this_thread::sleep(boost::posix_time::seconds(10));
					if (i==20){
						break;
					}
				}
			}
			return value;
		}

		
		template <typename Fn>
		bool select_mongo_muti_record(mongo_context *mongo_ctx_ptr,
			Fn handler,//回调函数
			std::string table, //表名
			mongo::Query   condition,//查询条件和排序都可以防盗这里//"{ query: { vid: "015899898985", dealresult: 0 }, orderby: { endtime: 1 } }"
			mongo::BSONObj * fieldsToReturn = 0,//返回字段 //"EC" << 0;--不返回本字段 ，"EC" << 1;--返回本字段
			int page_skip  = 0,     //忽略的记录条数
			int page_limit = 0//本页记录条数
			)
		{
			int i=0;
			bool ret = false;
			while(true){
				try
				{
					if (page_skip<0)  page_skip = 0;
					if (page_limit<0) page_limit = 0;
					if (!mongo_ctx_ptr->healthy)
					{
						reconnect_mongo(mongo_ctx_ptr);
					}

					std::auto_ptr<mongo::DBClientCursor> cursor = mongo_ctx_ptr->conn_ptr->query(table, condition, 
						page_limit, page_skip,fieldsToReturn, 0, 0);
					while(cursor->more())
					{
						handler(cursor->next());
					}
					ret = true;
					break;
				}
				catch(std::exception &e)
				{
					printf("mongo_select_mt error! --> %s.\n",e.what());
					log_manager::LOG("mongo_select_mt error",e.what());
					mongo_ctx_ptr->healthy=false;
					i++;
					boost::this_thread::sleep(boost::posix_time::seconds(10));
					if (i==20){
						break;
					}
				}
			}
			return ret;
		}

		//filterB : BSON("appid"<<"longgee"<<"apppassword"<<"longger")
		bool insert_mongo_data(mongo_context *mongo_ctx_ptr,std::string table, mongo::BSONObj filterB)
		{
			int i=0;
			bool ret = false;
			while(true){
				try
				{
					mongo::Query condition(filterB);
					if (!mongo_ctx_ptr->healthy)
					{
						reconnect_mongo(mongo_ctx_ptr);
					}
					mongo_ctx_ptr->conn_ptr->insert(table,condition.obj);
					std::string error = mongo_ctx_ptr->conn_ptr->getLastError();
					ret = error.empty();
					break;
				}
				catch(std::exception &e)
				{
					printf("insert_mongo_data error! --> %s.\n",e.what());
					log_manager::LOG("insert_mongo_data error",e.what());
					mongo_ctx_ptr->healthy=false;
					i++;
					boost::this_thread::sleep(boost::posix_time::seconds(10));
					if (i==20){
						break;
					}
				}
			}
			
			return ret;
		}

		//condition : BSON("appid"<<"longgee"<<"apppassword"<<"longger")
		bool delete_mongo_data(mongo_context *mongo_ctx_ptr,std::string table,mongo::Query condition)
		{
			int i=0;
			bool ret = false;
			while(true){
				try
				{
					if (!mongo_ctx_ptr->healthy)
					{
						reconnect_mongo(mongo_ctx_ptr);
					}
					mongo_ctx_ptr->conn_ptr->remove(table,condition);
					std::string error = mongo_ctx_ptr->conn_ptr->getLastError();
					ret = error.empty();
					break;
				}
				catch(std::exception &e)
				{
					printf("delete_mongo_data error! --> %s.\n",e.what());
					log_manager::LOG("delete_mongo_data error",e.what());
					mongo_ctx_ptr->healthy=false;
					i++;
					boost::this_thread::sleep(boost::posix_time::seconds(10));
					if (i==20){
						break;
					}
				}
			}
			
			return ret;
		}

		/*
		condition : BSON("appid"<<"longgee"<<"apppassword"<<"longger")

		mongo::BSONObjBuilder updateB;
		updateB << "$set" << BSON("verifycode"<<verifycode);
		*/
		bool update_mongo_data(mongo_context *mongo_ctx_ptr,std::string table,mongo::Query condition
			,mongo::BSONObj updateB)
		{
			int i=0;
			bool ret = false;
			while(true){
				try
				{
					if (!mongo_ctx_ptr->healthy)
					{
						reconnect_mongo(mongo_ctx_ptr);
					}
					mongo_ctx_ptr->conn_ptr->update(table,condition,updateB,false, true);//更新所有匹配项
					std::string err = mongo_ctx_ptr->conn_ptr->getLastError();
					if (err.empty())
					{
						ret = true;
					}
					break;
				}
				catch(std::exception &e)
				{
					printf("update_mongo_data error! --> %s.\n",e.what());
					log_manager::LOG("update_mongo_data error",e.what());
					mongo_ctx_ptr->healthy=false;
					i++;
					boost::this_thread::sleep(boost::posix_time::seconds(10));
					if (i==20){
						break;
					}
				}
			}
			
			return ret;
		}

		mongo_context *get_mongo_context(){
			//printf("fetching mongo connection!\n");
			{
				boost::mutex::scoped_lock lock(mongo_ctx_mutex);
				mongo_ctx_no++;
				if (mongo_ctx_no==mongo_ctx_count){
					mongo_ctx_no=0;
				}
			}
			int no=mongo_ctx_no;
			mongo_ctx_array[no]->mutex.lock();
			return (mongo_ctx_array[no]);
		}

		void return_mongo_context(mongo_context *mongo_ctx_ptr){
			mongo_ctx_ptr->mutex.unlock();
		}

	// for redis

	private:

		bool create_redis_connection(redis_context * redis_ctx_ptr){
			redis_ctx_ptr->ctx=redisConnect(redis_ctx_ptr->host.c_str(),redis_ctx_ptr->port);
			if (redis_ctx_ptr->ctx->err){
				printf("redis connnect error -->%s\n",redis_ctx_ptr->ctx->errstr);
				redisFree(redis_ctx_ptr->ctx);
				redis_ctx_ptr->ctx=NULL;
				return false;
			}
			if (!redis_ctx_ptr->auth.empty()){
				redisCommand(redis_ctx_ptr->ctx,"auth %s",redis_ctx_ptr->auth.c_str());
			}
			return true;
		}

	public:

		bool create_redis_context(std::string host,int port){
			redis_ctx.ctx=NULL;
			redis_ctx.host=host;
			redis_ctx.port=port;
			redis_ctx.auth="sinolbs";
			if (!create_redis_connection(&redis_ctx)){
				printf("redis for motor storage connect failed!\n");
				return false;
			}
			return true;
		}

		bool set_redis_data(redis_context *redis_ctx_ptr,std::string redis_cmd){
			bool ret = false;
			if (redis_ctx_ptr){
				int n=0;
				while (true){
					redisReply *reply=NULL;
					if (redis_ctx_ptr->ctx){
						reply=(redisReply *)redisCommand(redis_ctx_ptr->ctx,redis_cmd.c_str());
					}
					if (reply){
						if(reply->type == REDIS_REPLY_STRING ||
							reply->type == REDIS_REPLY_ARRAY || 
							reply->type == REDIS_REPLY_INTEGER ||
							reply->type == REDIS_REPLY_NIL ||
							reply->type == REDIS_REPLY_STATUS)
						{
							ret = true;
						}
						else if(reply->type == REDIS_REPLY_ERROR)
						{

						}
						freeReplyObject(reply);
						break; 
					}else{
						printf("redis reply is null\n");
						if (!create_redis_connection(redis_ctx_ptr)){
							if (n++==10){
								break;
							}
							boost::this_thread::sleep(boost::posix_time::seconds(2));
						}
					}
				}
			}
			return ret;
		}

		//添加回调方式返回数据
		redisReply * get_redis_data(redis_context *redis_ctx_ptr,std::string redis_cmd){
			redisReply * reply = NULL;
			bool ret = false;
			if (redis_ctx_ptr){
				int n=0;
				while (true){
					if (redis_ctx_ptr->ctx){
						reply=(redisReply *)redisCommand(redis_ctx_ptr->ctx,redis_cmd.c_str());
					}
					if (reply){
						if(reply->type == REDIS_REPLY_STRING ||
							reply->type == REDIS_REPLY_ARRAY || 
							reply->type == REDIS_REPLY_INTEGER ||
							reply->type == REDIS_REPLY_NIL ||
							reply->type == REDIS_REPLY_STATUS)
						{
							ret = true;
						}
						else if(reply->type == REDIS_REPLY_ERROR)
						{
						}
						break; // unlock the mutex , DON'T RETURN
					}else{
						printf("redis reply is null\n");
						if (!create_redis_connection(redis_ctx_ptr)){
							if (n++==10){
								break;
							}
							boost::this_thread::sleep(boost::posix_time::seconds(2));
						}
					}
				}
			}
			return reply;
		}

		std::string get_redis_string_value(redis_context *redis_ctx_ptr,std::string redis_cmd)
		{
			std::stringstream value_stream;
			redisReply *reply = NULL;
			reply = get_redis_data(redis_ctx_ptr,redis_cmd);
			if (reply)
			{
				if (reply->type==REDIS_REPLY_STRING){
					value_stream << reply->str;
				}else if(reply->type==REDIS_REPLY_INTEGER){
					value_stream << reply->integer;
				}
				freeReplyObject(reply);
				return value_stream.str();
			}
			return "";
		}

		int get_redis_integer_value(redis_context *redis_ctx_ptr,std::string redis_cmd)
		{
			int value = 0;
			redisReply *reply = NULL;
			reply = get_redis_data(redis_ctx_ptr,redis_cmd);
			if (reply)
			{
				if (reply->type==REDIS_REPLY_STRING){
					value = atol(reply->str);
				}else if(reply->type==REDIS_REPLY_INTEGER){
					value = reply->integer;
				}
				freeReplyObject(reply);
			}
			return value;
		}

		bool get_redis_value_list(redis_context *redis_ctx_ptr,std::string redis_cmd,
			std::list<std::string>* values)
		{
			redisReply *reply = NULL;
			reply = get_redis_data(redis_ctx_ptr,redis_cmd);
			if (reply)
			{
				if(reply->type == REDIS_REPLY_ARRAY)
				{
					for (int i = 0; i < reply->elements; ++i)
						values->push_back(reply->element[i]->str);
				}
				else if (reply->type==REDIS_REPLY_STRING){
					values->push_back(reply->str);
				}else if(reply->type==REDIS_REPLY_INTEGER){
					std::stringstream value_stream;
					value_stream << reply->integer;
					values->push_back(value_stream.str());
				}
				freeReplyObject(reply);
				return true;
			}
			return false;
		}

		bool set_redis_key_ttl(redis_context *redis_ctx_ptr,std::string key_,int tm)
		{
			std::stringstream cmd_stream;
			cmd_stream << "EXPIRE " << key_ << " " << tm;
			return set_redis_data(redis_ctx_ptr,cmd_stream.str());
		}

		bool get_redis_key_ttl(redis_context *redis_ctx_ptr,std::string key_,int & tm)
		{
			std::stringstream cmd_stream;
			cmd_stream << "TTL " << key_;
			tm = -1;
			std::string tm_str = get_redis_string_value(redis_ctx_ptr,cmd_stream.str());
			if(!tm_str.empty())
			{
				tm = atol(tm_str.c_str());
				return true;
			}
			return false;
		}

		bool redis_exists_key(redis_context *redis_ctx_ptr,std::string key_)
		{
			int exist = 0;

			if (!key_.empty())
			{
				std::stringstream cmd_stream;
				cmd_stream << "EXISTS " << key_;
				redisReply *reply = NULL;
				reply = get_redis_data(redis_ctx_ptr,cmd_stream.str());
				if (reply)
				{
					if(reply->type==REDIS_REPLY_INTEGER){
						exist = reply->integer;
					}
					freeReplyObject(reply);
				}
			}
			return (exist == 1);//键值存在返回1，不存在返回0
		}

   public:

		redis_context * get_redis_context(){
			redis_ctx.mutex.lock();
			return &redis_ctx;
		}

		void return_redis_context(redis_context *redis_ctx_ptr){
			redis_ctx_ptr->mutex.unlock();
		}
		// some api
	public:

		bool save_verifycode(http::redis_context *rtx_ptr,std::string verifycode,std::string user,std::string pwd,int expiry_date)
		{
			int ret = 0;
			std::stringstream cmd_stream;
			cmd_stream << "set " << verifycode << " " << user;
			if (set_redis_data(rtx_ptr,cmd_stream.str()) && 
				set_redis_key_ttl(rtx_ptr,verifycode,expiry_date))
			{
				return true;
			}
			return false;
		}

		bool updata_app_verifycode(http::mongo_context * mtx_ptr,std::string verifycode,std::string appid,std::string pwd)
		{
			mongo::BSONObjBuilder set_condition;
			set_condition << "$set" << BSON("verifycode"<<verifycode);

			return update_mongo_data(mtx_ptr,mongo_app_table,BSON("appid"<<appid<<"apppassword"<<pwd),set_condition.obj());
		}

		std::string load_device_type(http::mongo_context * mtx_ptr,std::string device_id)
		{
			std::string json_record = select_mongo_one_record(mtx_ptr,mongo_device_table,BSON("vid"<<device_id));
			mongo::BSONObj obj;
			try
			{
				obj = mongo::fromjson(json_record);
				return obj.getStringField("tt");
			}
			catch (std::exception* e)
			{
				printf("load_device_type %s",e->what());
			}
			return "";//获取终端类型
		}

		//获取终端实时报警 -- 获取最后一条报警,测试ok
		bool load_last_alarm(http::mongo_context * mtx, std::string device_id, std::list<std::string> * values)
		{
			std::string str = "";
			mongo::Query query(BSON("vid" << device_id << "dealresult" << 0));//加载未处理的报警
			assert(mtx != NULL);
			query.sort("endtime", -1); //-1降序处理
			str = select_mongo_one_record(mtx, mongo_alarm_table,
				query.obj);
			if (!str.empty())
			{
				values->push_back(str);
			}
			return true;
		}

		//加载信息报警对照信息
		std::string load_alarm_mapping(http::mongo_context * mtx,std::string alarm_id,std::string alarm_type)
		{
			std::string str = select_mongo_one_record(mtx, mongo_alarm_map_table,
				BSON("alarmid" << alarm_id << "tt" << alarm_type));
			return str;
		}

		std::string load_api_fun_name(http::mongo_context * mtx,std::string apiid)
		{
			std::string apiname_str = "";
			std::string json_record = select_mongo_one_record(mtx,mongo_api_table,BSON("apiid" << apiid ));
			mongo::BSONObj obj;
			try
			{
				obj = mongo::fromjson(json_record);
				return obj.getStringField("apiname");
			}
			catch (std::exception & e)
			{
				printf("load_device_type error! --> %s.\n",e.what());
				log_manager::LOG("load_device_type error",e.what());
			}
			return "";
		}

	private:
		enum { MAX_MONGO_CTX=10 , MAX_REDIS_CTX=10 };

		int mongo_ctx_count;
		int mongo_ctx_no;
		boost::mutex mongo_ctx_mutex;
		mongo_context ** mongo_ctx_array;
		redis_context redis_ctx;
	};
};

#endif