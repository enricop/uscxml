#ifndef HTTPSERVER_H_AIH108EG
#define HTTPSERVER_H_AIH108EG

#include <string>
#include <map>

#include <event2/http.h>

#include "uscxml/concurrency/tinythread.h"
#include "uscxml/Message.h"

namespace uscxml {

class HTTPServlet;

class HTTPServer {
public:
	class Request : public Event {
	public:
		Request() : curlReq(NULL) {}
		std::string content;
		struct evhttp_request* curlReq;

		operator bool() {
			return curlReq != NULL;
		}
	};

	class SSLConfig {
	public:
		SSLConfig() : port(8443) {}
		std::string privateKey;
		std::string publicKey;
		unsigned short port;
	};
	
	class Reply {
	public:
		Reply(Request req) : status(200), type(req.data.compound["type"].atom), curlReq(req.curlReq) {}
		int status;
		std::string type;
		std::map<std::string, std::string> headers;
		std::string content;
		struct evhttp_request* curlReq;
	};

	struct CallbackData {
		HTTPServlet* servlet;
		evhttp_request* httpReq;
	};

	static HTTPServer* getInstance(unsigned short port = 8080, SSLConfig* sslConf = NULL);
	static std::string getBaseURL();

	static void reply(const Reply& reply);
	static bool registerServlet(const std::string& path, HTTPServlet* servlet); ///< Register a servlet, returns false if path is already taken
	static void unregisterServlet(HTTPServlet* servlet);

private:
	struct comp_strsize_less {
		bool operator()(std::string const& l, std::string const& r) const {
			if (l.size() < r.size())
				return false;

			return l < r;
		};
	};

	HTTPServer(unsigned short port, SSLConfig* sslConf = NULL);
	~HTTPServer();

	void start();
	void stop();
	static void run(void* instance);

	void determineAddress();

	static void replyCallback(evutil_socket_t fd, short what, void *arg);
	static void httpRecvReqCallback(struct evhttp_request *req, void *callbackData);
	void processByMatchingServlet(const Request& request);

	static std::map<std::string, std::string> mimeTypes;
	std::map<std::string, HTTPServlet*> _servlets;
	typedef std::map<std::string, HTTPServlet*>::iterator servlet_iter_t;

	struct event_base* _base;
	struct evhttp* _http;
	struct evhttp_bound_socket* _handle;

	unsigned short _port;
	std::string _address;

	static HTTPServer* _instance;

	static tthread::recursive_mutex _instanceMutex;
	tthread::thread* _thread;
	tthread::recursive_mutex _mutex;
	bool _isRunning;

	friend class HTTPServlet;
	
#if (defined EVENT_SSL_FOUND && defined OPENSSL_FOUND)
	struct evhttp* _https;
  struct evhttp_bound_socket* _sslHandle;
	unsigned short _sslPort;

	static struct bufferevent* sslBufferEventCallback(struct event_base *base, void *arg);
	static void sslGeneralBufferEventCallback (struct evhttp_request *req, void *arg);
#endif
};

class HTTPServlet {
public:
	virtual bool httpRecvRequest(const HTTPServer::Request& request) = 0;
	virtual void setURL(const std::string& url) = 0; /// Called by the server with the actual URL
	virtual bool canAdaptPath() {
		return true;
	}
};

}

#endif /* end of include guard: HTTPSERVER_H_AIH108EG */
