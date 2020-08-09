#include "/xmuduo/muduo/net/http/HttpServer.h"

#include "/xmuduo/muduo/base/Logging.h"
#include "/xmuduo/muduo/net/http/HttpContext.h"
#include "/xmuduo/muduo/net/http/HttpRequest.h"
#include "/xmuduo/muduo/net/http/HttpResponse.h"

#include <boost/bind.hpp>

using namespace muduo;
using namespace muduo::net;

namespace muduo
{
namespace net
{
namespace detail
{
bool processRequestLine(const char* begin,const char* end,
									 HttpContext* context)
{
	bool succeed = false;
	const char* start = begin;
	const char* space = std::find(start,end,' ');
	HttpRequest& request = context ->request();
	if(space != end && request.setMethod(start,space))
	{
		start = space + 1;
		space = std::find(start,end,' ');
		if(space != end)
		{
			request.setPath(start,space);
			start = space + 1;
			succeed = end -start == 8 && std::equal(start,end-1,"HTTP/1.");
			if(succeed)
			{
				if(*(end-1) == '1')
				{
					request.setVersion(HttpRequest::kHttp11);
				}
				else if(*(end-1) == '0')
				{
					request.setVersion(HttpRequest::kHttp10);
				}
				else
				{
					succeed = false;
				}
			}
		}
	}
	return succeed;
}

bool parseRequest(Buffer* buf,HttpContext* context,Timestamp receiveTime)
{
	bool ok = true;
	bool hasMore = true;
	while(hasMore)
	{
		if(context->expectRequestLine())
		{
			const char* crlf = buf->findCRLF();
			if(crlf)
			{
				ok = processRequestLine(buf->peek(),crlf,context);
				if(ok)
				{
					context->request().setReceiveTime(receiveTime);
					buf->retrieveUntil(crlf+2);
					context->receiveRequestLine();
				}
				else
				{
					hasMore = false;
				}
			}
			else
			{
				hasMore = false;
			}	
		}
		else if(context->expectHeaders())
		{
			const char* crlf = buf->findCRLF();
			if(crlf)
			{
				const char* colon = std::find(buf->peek(),crlf,':');
				if(colon != crlf)
				{
					context->request().addHeader(buf->peek(),colon,crlf);
				}
				else
				{
					context->receiveHeaders();
					hasMore = !context->gotAll();
				}
				buf->retrieveUntil(crlf+2);
			}
			else
			{
				hasMore = false;
			}
		}
		else if(context->expectBody()) //do not support body
		{
		}
	}
	return ok;
}

void defaultHttpCallback(const HttpRequest&,HttpResponse* resp)
{
	resp->setStatusCode(HttpResponse::k404NotFound);
	resp->setStatusMessage("Not Found");
	resp->setCloseConnection(true);
}

}
}
}

HttpServer::HttpServer(EventLoop* loop,
					   const InetAddress& listenAddr,
					   const string& name)
	:	server_(loop,listenAddr,name),
		httpCallback_(detail::defaultHttpCallback)
{
	server_.setConnectionCallback(
		boost::bind(&HttpServer::onConnection,this,_1));
	server_.setMessageCallback(
		boost::bind(&HttpServer::onMessage,this,_1,_2,_3));
}

HttpServer::~HttpServer()
{
}

void HttpServer::start()
{
	LOG_TRACE << "HttpServer[" << server_.name()
			<< "] starts listenning on " << server_.hostport();
	server_.start();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn)
{
	if(conn->connected())
	{
		conn->setContext(HttpContext()); //bind httpcontext to tcp connection
	}	
}

void HttpServer::onMessage(const TcpConnectionPtr& conn,
						   Buffer* buf,
						   Timestamp receiveTime)
{
	HttpContext* context = boost::any_cast<HttpContext>(conn->getMutableContext());
	
	if(!detail::parseRequest(buf,context,receiveTime))
	{
		conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
		conn->shutdown();
	}

	if(context->gotAll())
	{
		onRequest(conn,context->request());
		context->reset();
	}
}

void HttpServer::onRequest(const TcpConnectionPtr& conn,const HttpRequest& req)
{
	const string& connection = req.getHeader("Connection");
	bool close = connection == "close" ||
		(req.getVersion() == HttpRequest::HttpRequest::kHttp10 && connection != "Keep-Alive");
	//LOG_TRACE << close;
	HttpResponse response(close);
	httpCallback_(req,&response);
	Buffer buf;
	response.appendToBuffer(&buf);
	conn->send(&buf);
	if(response.closeConnection())
	{
		conn->shutdown();
	}
}
