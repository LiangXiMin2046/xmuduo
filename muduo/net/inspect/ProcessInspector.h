#ifndef MUDUO_NET_INSPECT_PROCESSINSPECTOR_H
#define MUDUO_NET_INSPECT_PROCESSINSPECTOR_H

#include "/xmuduo/muduo/net/inspect/Inspector.h"
#include <boost/noncopyable.hpp>

namespace muduo
{
namespace net
{
class ProcessInspector : boost::noncopyable
{
public:
	void registerCommands(Inspector* ins); //register command interface
private:
	static string pid(HttpRequest::Method,const Inspector::ArgList&);	
	static string procStatus(HttpRequest::Method,const Inspector::ArgList&);
	static string openedFiles(HttpRequest::Method,const Inspector::ArgList&);
	static string threads(HttpRequest::Method,const Inspector::ArgList&);
};

}
}
#endif
