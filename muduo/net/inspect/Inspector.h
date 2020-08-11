#ifndef MUDUO_NET_INSPECT_INSPECTOR_H
#define MUDUO_NET_INSPECT_INSPECTOR_H

#include "/xmuduo/muduo/base/Mutex.h"
#include "/xmuduo/muduo/net/http/HttpRequest.h"
#include "/xmuduo/muduo/net/http/HttpServer.h"

#include <map>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace muduo
{
namespace net
{
class ProcessInspector;
//internal inspector,usually a singleton
class Inspector : boost::noncopyable
{
public:
	typedef std::vector<string> ArgList;
	typedef boost::function<string(HttpRequest::Method,
			 const ArgList& args)> Callback;
	Inspector(EventLoop* loop,
			  const InetAddress& httpAddr,
			  const string& name);
	~Inspector();

	//response request
	void add(const string& module,
			 const string& command,
			 const Callback& cb,
			 const string& help);

private:
	typedef std::map<string,Callback> CommandList;
	typedef std::map<string,string>	HelpList;
	void start();
	void onRequest(const HttpRequest& req,HttpResponse* resp);

	HttpServer server_;
	boost::scoped_ptr<ProcessInspector> processInspector_;
	MutexLock mutex_;
	std::map<string,CommandList> commands_;
	std::map<string,HelpList> helps_;	
};

}
}

#endif
