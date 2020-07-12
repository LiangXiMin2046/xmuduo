#ifndef MUDUO_BASE_FILE_UTIL_H
#define MUDUO_BASE_FILE_UTIL_H

#include "/xmuduo/muduo/base/Types.h"
#include "/xmuduo/muduo/base/StringPiece.h"

#include<boost/noncopyable.hpp>

namespace muduo
{
namespace FileUtil
{
	class SmallFile : boost::noncopyable
	{
	public:
		SmallFile(StringPiece filename);
		~SmallFile();

		template<typename String>
		int readToString(int maxSize,String* content,int64_t* fileSize,
							int64_t* modifyTime,int64_t* createTime);
		
		int readToBuffer(int* size);
		const char* buffer() const {return buf_;}

		static const int kBufferSize = 65536;
	private:	
		int fd_;
		int err_;
		char buf_[kBufferSize];
	};
	
	template<typename String>
	int readFile(StringPiece filename,int maxSize,String* content,int64_t* fileSize = NULL,int64_t* modifyTime = NULL,
			int64_t* createTime = NULL)
	{
		SmallFile file(filename);
		return file.readToString(maxSize,content,fileSize,modifyTime,createTime);
	}
}

}
#endif
