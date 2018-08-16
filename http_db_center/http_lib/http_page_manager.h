#ifndef _HTTP_PAGE_MANAGER_H_
#define _HTTP_PAGE_MANAGER_H_

#include <string.h>

namespace http
{
	#define Page_Size 100
	//分页数据
	class http_page_manager
	{
	public:
		http_page_manager()
		{
			init();
		}

		void init()
		{
			page_size_ = Page_Size;
			tpage_  = 0;
			cpage_  = 0;
			tsize_  = 0;
			csize_  = 0;
			offsize_ = 0;
		}

		//输入数据总条数和当前页
		bool start(int tsize_i,int cpage = 1,int page_size = Page_Size)
		{
			if (tsize_i>0)
			{
				tsize_ = tsize_i;
				page_size_ = (page_size <1 || page_size>Page_Size) ? Page_Size:page_size;
				tpage_ = (tsize_i  +  page_size_  - 1) / page_size_;
				cpage_ = (cpage_ < 1 || cpage_ > tpage_) ? 1:cpage;
				csize_   = (cpage_ == tpage_) ? (((tsize_i-1) % page_size_)+1) : page_size_;
				offsize_ = (cpage_ - 1) * page_size_;
				return true;
			}
			return false;
		}

		std::string end(void * data);

// 		std::string end(mongo::BSONArrayBuilder & values)
// 		{
// 			std::string str = "";
// 			str  = BSON("tpages" << tpage_ << "cpage" << cpage_ << "tsize" << tsize_
// 				<< "csize" << csize_ << "valuelist" << values.arr()).jsonString();
// 			return str;
// 		}

		int get_current_size()
		{
			return csize_;
		}

		int get_off_record_size()
		{
			return offsize_;
		}

		int get_current_page()
		{
			return cpage_;
		}

	private:
		//分页数据初始化
		int page_size_;//每页数据条数
		int tpage_ ;//总页数
		int cpage_ ;//当前页数
		int tsize_ ;//总数据条数
		int csize_ ;//当前页数据条数
		int offsize_ ;//前面忽略的数据条数

	};



};//namespace http

#endif