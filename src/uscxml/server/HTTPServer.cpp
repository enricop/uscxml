/**
 *  @file
 *  @author     2012-2013 Stefan Radomski (stefan.radomski@cs.tu-darmstadt.de)
 *  @copyright  Simplified BSD
 *
 *  @cond
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the FreeBSD license as published by the FreeBSD
 *  project.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  You should have received a copy of the FreeBSD license along with this
 *  program. If not, see <http://www.opensource.org/licenses/bsd-license>.
 *  @endcond
 */

#include "uscxml/config.h"

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#endif

#include "uscxml/server/HTTPServer.h"

#include <string>
#include <iostream>

extern "C" {
#include <event2/dns.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/keyvalq_struct.h>
#include <event2/http_struct.h>
#include <event2/thread.h>
}

#include <glog/logging.h>
#include <boost/algorithm/string.hpp>

#ifndef _WIN32
#include <netinet/in.h>                 // for INADDR_ANY
#include <stdint.h>                     // for uint16_t
#include <stdlib.h>                     // for NULL, free
#include <unistd.h>                     // for gethostname
//#include <netdb.h>
//#include <arpa/inet.h>
#endif

#if (defined EVENT_SSL_FOUND && defined OPENSSL_FOUND && defined OPENSSL_HAS_ELIPTIC_CURVES)
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <event2/bufferevent_ssl.h>
#endif

#include "uscxml/Message.h"
#include "uscxml/Convenience.h"         // for toStr

#ifdef BUILD_AS_PLUGINS
#include <Pluma/Connector.hpp>
#endif

namespace uscxml {

HTTPServer::HTTPServer(unsigned short port, SSLConfig* sslConf) {
	_port = port;
	_base = event_base_new();
	_http = evhttp_new(_base);
	_thread = NULL;

	unsigned int allowedMethods =
	    EVHTTP_REQ_GET |
	    EVHTTP_REQ_POST |
	    EVHTTP_REQ_HEAD |
	    EVHTTP_REQ_PUT |
	    EVHTTP_REQ_DELETE |
	    EVHTTP_REQ_OPTIONS |
	    EVHTTP_REQ_TRACE |
	    EVHTTP_REQ_CONNECT |
	    EVHTTP_REQ_PATCH;

	evhttp_set_allowed_methods(_http, allowedMethods); // allow all methods

	_handle = NULL;
	while((_handle = evhttp_bind_socket_with_handle(_http, INADDR_ANY, _port)) == NULL) {
		_port++;
	}
	determineAddress();

#if (defined EVENT_SSL_FOUND && defined OPENSSL_FOUND && defined OPENSSL_HAS_ELIPTIC_CURVES)
	if (!sslConf) {
		_https = NULL;
		_sslHandle = NULL;
		_sslPort = 0;
	} else {
		_sslPort = sslConf->port;

		// Initialize OpenSSL
		SSL_library_init();
		ERR_load_crypto_strings();
		SSL_load_error_strings();
		OpenSSL_add_all_algorithms();

		_https = evhttp_new(_base);
		evhttp_set_allowed_methods(_https, allowedMethods); // allow all methods

		SSL_CTX* ctx = SSL_CTX_new (SSLv23_server_method ());
		SSL_CTX_set_options(ctx,
		                    SSL_OP_SINGLE_DH_USE |
		                    SSL_OP_SINGLE_ECDH_USE |
		                    SSL_OP_NO_SSLv2);

		EC_KEY* ecdh = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
		SSL_CTX_set_tmp_ecdh (ctx, ecdh);

		SSL_CTX_use_certificate_chain_file(ctx, sslConf->publicKey.c_str());
		SSL_CTX_use_PrivateKey_file(ctx, sslConf->privateKey.c_str(), SSL_FILETYPE_PEM);
		SSL_CTX_check_private_key(ctx);

		evhttp_set_bevcb(_https, sslBufferEventCallback, ctx);
		evhttp_set_gencb(_https, sslGeneralBufferEventCallback, NULL);

		_sslHandle = NULL;
		while((_sslHandle = evhttp_bind_socket_with_handle(_https, INADDR_ANY, _sslPort)) == NULL) {
			_sslPort++;
		}
	}
#endif

//	evhttp_set_timeout(_http, 5);

	// generic callback
	evhttp_set_gencb(_http, HTTPServer::httpRecvReqCallback, NULL);
}

HTTPServer::~HTTPServer() {
	_isRunning = false;
	_thread->join();
	delete _thread;
}

HTTPServer* HTTPServer::_instance = NULL;
tthread::recursive_mutex HTTPServer::_instanceMutex;

HTTPServer* HTTPServer::getInstance(unsigned short port, SSLConfig* sslConf) {
//	tthread::lock_guard<tthread::recursive_mutex> lock(_instanceMutex);
	if (_instance == NULL) {
#ifdef _WIN32
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
#ifndef _WIN32
		evthread_use_pthreads();
#else
		evthread_use_windows_threads();
#endif
		_instance = new HTTPServer(port, sslConf);
		_instance->start();
	}
	return _instance;
}

#if (defined EVENT_SSL_FOUND && defined OPENSSL_FOUND && defined OPENSSL_HAS_ELIPTIC_CURVES)
// see https://github.com/ppelleti/https-example/blob/master/https-server.c
struct bufferevent* HTTPServer::sslBufferEventCallback(struct event_base *base, void *arg) {
	struct bufferevent* r;
	SSL_CTX *ctx = (SSL_CTX *) arg;
	r = bufferevent_openssl_socket_new (base,
	                                    -1,
	                                    SSL_new (ctx),
	                                    BUFFEREVENT_SSL_ACCEPTING,
	                                    BEV_OPT_CLOSE_ON_FREE);
	return r;
}


void HTTPServer::sslGeneralBufferEventCallback (struct evhttp_request *req, void *arg) {
	struct evbuffer *evb = NULL;
	const char *uri = evhttp_request_get_uri (req);
	struct evhttp_uri *decoded = NULL;

	/* We only handle POST requests. */
	if (evhttp_request_get_command (req) != EVHTTP_REQ_POST) {
		evhttp_send_reply (req, 200, "OK", NULL);
		return;
	}

	printf ("Got a POST request for <%s>\n", uri);

	/* Decode the URI */
	decoded = evhttp_uri_parse (uri);
	if (! decoded) {
		printf ("It's not a good URI. Sending BADREQUEST\n");
		evhttp_send_error (req, HTTP_BADREQUEST, 0);
		return;
	}

	/* Decode the payload */
	struct evkeyvalq kv;
	memset (&kv, 0, sizeof (kv));
	struct evbuffer *buf = evhttp_request_get_input_buffer (req);
	evbuffer_add (buf, "", 1);    /* NUL-terminate the buffer */
	char *payload = (char *) evbuffer_pullup (buf, -1);
	if (0 != evhttp_parse_query_str (payload, &kv)) {
		printf ("Malformed payload. Sending BADREQUEST\n");
		evhttp_send_error (req, HTTP_BADREQUEST, 0);
		return;
	}

	/* Determine peer */
	char *peer_addr;
	ev_uint16_t peer_port;
	struct evhttp_connection *con = evhttp_request_get_connection (req);
	evhttp_connection_get_peer (con, &peer_addr, &peer_port);

	/* Extract passcode */
	const char *passcode = evhttp_find_header (&kv, "passcode");
	char response[256];
	evutil_snprintf (response, sizeof (response),
	                 "Hi %s!  I %s your passcode.\n", peer_addr,
	                 (0 == strcmp (passcode, "R23")
	                  ?  "liked"
	                  :  "didn't like"));
	evhttp_clear_headers (&kv);   /* to free memory held by kv */

	/* This holds the content we're sending. */
	evb = evbuffer_new ();

	evhttp_add_header (evhttp_request_get_output_headers (req),
	                   "Content-Type", "application/x-yaml");
	evbuffer_add (evb, response, strlen (response));

	evhttp_send_reply (req, 200, "OK", evb);

	if (decoded)
		evhttp_uri_free (decoded);
	if (evb)
		evbuffer_free (evb);
}
#endif

/**
 * This callback is registered for all HTTP requests
 */
void HTTPServer::httpRecvReqCallback(struct evhttp_request *req, void *callbackData) {
	std::stringstream raw;

	evhttp_request_own(req);
	Request request;
	request.curlReq = req;

	switch (evhttp_request_get_command(req)) {
	case EVHTTP_REQ_GET:
		request.data.compound["type"] = Data("get", Data::VERBATIM);
		break;
	case EVHTTP_REQ_POST:
		request.data.compound["type"] = Data("post", Data::VERBATIM);
		break;
	case EVHTTP_REQ_HEAD:
		request.data.compound["type"] = Data("head", Data::VERBATIM);
		break;
	case EVHTTP_REQ_PUT:
		request.data.compound["type"] = Data("put", Data::VERBATIM);
		break;
	case EVHTTP_REQ_DELETE:
		request.data.compound["type"] = Data("delete", Data::VERBATIM);
		break;
	case EVHTTP_REQ_OPTIONS:
		request.data.compound["type"] = Data("options", Data::VERBATIM);
		break;
	case EVHTTP_REQ_TRACE:
		request.data.compound["type"] = Data("trace", Data::VERBATIM);
		break;
	case EVHTTP_REQ_CONNECT:
		request.data.compound["type"] = Data("connect", Data::VERBATIM);
		break;
	case EVHTTP_REQ_PATCH:
		request.data.compound["type"] = Data("patch", Data::VERBATIM);
		break;
	default:
		request.data.compound["type"] = Data("unknown", Data::VERBATIM);
		break;
	}
	raw << boost::to_upper_copy(request.data.compound["type"].atom);

	request.data.compound["remoteHost"] = Data(req->remote_host, Data::VERBATIM);
	request.data.compound["remotePort"] = Data(toStr(req->remote_port), Data::VERBATIM);
	request.data.compound["httpMajor"] = Data(toStr((unsigned short)req->major), Data::VERBATIM);
	request.data.compound["httpMinor"] = Data(toStr((unsigned short)req->minor), Data::VERBATIM);
	request.data.compound["uri"] = Data(HTTPServer::getBaseURL() + req->uri, Data::VERBATIM);

	char* pathCStr = evhttp_decode_uri(evhttp_uri_get_path(evhttp_request_get_evhttp_uri(req)));
	request.data.compound["path"] = Data(pathCStr, Data::VERBATIM);
	free(pathCStr);

	raw << " " << request.data.compound["path"].atom;
	const char* query = evhttp_uri_get_query(evhttp_request_get_evhttp_uri(req));
	if (query)
		raw << "?" << std::string(query);

	raw << " HTTP/" << request.data.compound["httpMajor"].atom << "." << request.data.compound["httpMinor"].atom;
	raw << std::endl;

	struct evkeyvalq *headers;
	struct evkeyval *header;
	struct evbuffer *buf;

	// insert headers to event data
	headers = evhttp_request_get_input_headers(req);
	for (header = headers->tqh_first; header; header = header->next.tqe_next) {
		request.data.compound["header"].compound[header->key] = Data(header->value, Data::VERBATIM);
		raw << header->key << ": " << header->value << std::endl;
	}
	raw << std::endl;

	// This was used for debugging
//	if (boost::ends_with(request.data.compound["path"].atom, ".png")) {
//		evhttp_send_error(req, 404, NULL);
//		return;
//	}

	// seperate path into components
	{
		std::stringstream ss(request.data.compound["path"].atom);
		std::string item;
		while(std::getline(ss, item, '/')) {
			if (item.length() == 0)
				continue;
			request.data.compound["pathComponent"].array.push_back(Data(item, Data::VERBATIM));
		}
	}
	// parse query string
	struct evkeyvalq params;
	struct evkeyval *param;

	evhttp_parse_query_str(query, &params);
	for (param = params.tqh_first; param; param = param->next.tqe_next) {
		request.data.compound["query"].compound[param->key] = Data(param->value, Data::VERBATIM);
	}
	evhttp_clear_headers(&params);

	// get content
	buf = evhttp_request_get_input_buffer(req);
	if (evbuffer_get_length(buf))
		request.data.compound["content"] = Data("", Data::VERBATIM);
	while (evbuffer_get_length(buf)) {
		int n;
		char cbuf[1024];
		n = evbuffer_remove(buf, cbuf, sizeof(buf)-1);
		if (n > 0) {
			request.data.compound["content"].atom.append(cbuf, n);
		}
	}

	raw << request.data.compound["content"].atom;

	// decode content
	if (request.data.compound.find("content") != request.data.compound.end() &&
	        request.data.compound["header"].compound.find("Content-Type") != request.data.compound["header"].compound.end()) {
		std::string contentType = request.data.compound["header"].compound["Content-Type"].atom;
		if (false) {
		} else if (iequals(contentType, "application/x-www-form-urlencoded")) {
			// this is a form submit
			std::stringstream ss(request.data.compound["content"].atom);
			std::string item;
			std::string key;
			std::string value;
			while(std::getline(ss, item, '&')) {
				if (item.length() == 0)
					continue;
				size_t equalPos = item.find('=');
				if (equalPos == std::string::npos)
					continue;

				key = item.substr(0, equalPos);
				value = item.substr(equalPos + 1, item.length() - (equalPos + 1));
				char* keyCStr = evhttp_decode_uri(key.c_str());
				char* valueCStr = evhttp_decode_uri(value.c_str());
				request.data.compound["content"].compound[keyCStr] = Data(valueCStr, Data::VERBATIM);
				free(keyCStr);
				free(valueCStr);
				key.clear();
			}
			request.data.compound["content"].atom.clear();
		} else if (iequals(contentType, "application/json")) {
			request.data.compound["content"] = Data::fromJSON(request.data.compound["content"].atom);
		}
	}

	request.raw = raw.str();

	// try with the handler registered for path first
	bool answered = false;
	if (callbackData != NULL)
		answered = ((HTTPServlet*)callbackData)->httpRecvRequest(request);

	if (!answered)
		HTTPServer::getInstance()->processByMatchingServlet(request);
}


void HTTPServer::processByMatchingServlet(const Request& request) {
	tthread::lock_guard<tthread::recursive_mutex> lock(_mutex);

	servlet_iter_t servletIter = _servlets.begin();

	std::string actualPath = request.data.compound.at("path").atom;
	std::map<std::string, HTTPServlet*, comp_strsize_less> matches;

	while(servletIter != _servlets.end()) {
		// is the servlet path a prefix of the actual path?
		std::string servletPath = "/" + servletIter->first;
		if (iequals(actualPath.substr(0, servletPath.length()), servletPath) && // actual path is a prefix
		        iequals(actualPath.substr(servletPath.length(), 1), "/")) {     // and next character is a '/'
			matches.insert(std::make_pair(servletPath, servletIter->second));
		}
		servletIter++;
	}

	// process by best matching servlet until someone feels responsible
	std::map<std::string, HTTPServlet*, comp_strsize_less>::iterator matchesIter = matches.begin();
	while(matchesIter != matches.end()) {
		if (matchesIter->second->httpRecvRequest(request)) {
			return;
		}
		matchesIter++;
	}

	LOG(INFO) << "Got an HTTP request at " << actualPath << " but no servlet is registered there or at a prefix";
	evhttp_send_error(request.curlReq, 404, NULL);
}

void HTTPServer::reply(const Reply& reply) {
	// we need to reply from the thread calling event_base_dispatch, just add to ist base queue!
	Reply* replyCB = new Reply(reply);
	HTTPServer* INSTANCE = getInstance();
	event_base_once(INSTANCE->_base, -1, EV_TIMEOUT, HTTPServer::replyCallback, replyCB, NULL);
}

void HTTPServer::replyCallback(evutil_socket_t fd, short what, void *arg) {
	Reply* reply = (Reply*)arg;

	if (reply->content.size() > 0 && reply->headers.find("Content-Type") == reply->headers.end()) {
		LOG(INFO) << "Sending content without Content-Type header";
	}

	std::map<std::string, std::string>::const_iterator headerIter = reply->headers.begin();
	while(headerIter != reply->headers.end()) {
		evhttp_add_header(evhttp_request_get_output_headers(reply->curlReq), headerIter->first.c_str(), headerIter->second.c_str());
		headerIter++;
	}

	if (reply->status >= 400) {
		evhttp_send_error(reply->curlReq, reply->status, NULL);
		return;
	}

	struct evbuffer *evb = NULL;

	if (!iequals(reply->type, "HEAD") && reply->content.size() > 0) {
		evb = evbuffer_new();
		evbuffer_add(evb, reply->content.data(), reply->content.size());
	}

	evhttp_send_reply(reply->curlReq, reply->status, NULL, evb);

	if (evb != NULL)
		evbuffer_free(evb);
//  evhttp_request_free(reply->curlReq);
	delete(reply);
}

bool HTTPServer::registerServlet(const std::string& path, HTTPServlet* servlet) {
	HTTPServer* INSTANCE = getInstance();
	tthread::lock_guard<tthread::recursive_mutex> lock(INSTANCE->_mutex);

	// remove trailing and leading slash
	std::string actualPath = path;
	if (boost::ends_with(actualPath, "/"))
		actualPath = actualPath.substr(0, actualPath.size() - 1);
	if (boost::starts_with(actualPath, "/"))
		actualPath = actualPath.substr(1);
	std::string suffixedPath = actualPath;

	// if this servlet allows to adapt the path, do so
	int i = 2;
	while(INSTANCE->_servlets.find(suffixedPath) != INSTANCE->_servlets.end()) {
		if (!servlet->canAdaptPath())
			return false;
		std::stringstream ss;
		ss << actualPath << i++;
		suffixedPath = ss.str();
	}

	std::stringstream servletURL;
	servletURL << "http://" << INSTANCE->_address << ":" << INSTANCE->_port << "/" << suffixedPath;
	servlet->setURL(servletURL.str());

	INSTANCE->_servlets[suffixedPath] = servlet;

//	LOG(INFO) << "HTTP Servlet listening at: " << servletURL.str() << std::endl;

	// register callback
	evhttp_set_cb(INSTANCE->_http, ("/" + suffixedPath).c_str(), HTTPServer::httpRecvReqCallback, servlet);

	return true;
}

std::string HTTPServer::getBaseURL() {
	HTTPServer* INSTANCE = getInstance();
	std::stringstream servletURL;
	servletURL << "http://" << INSTANCE->_address << ":" << INSTANCE->_port;
	return servletURL.str();
}

void HTTPServer::unregisterServlet(HTTPServlet* servlet) {
	HTTPServer* INSTANCE = getInstance();
	tthread::lock_guard<tthread::recursive_mutex> lock(INSTANCE->_mutex);
	servlet_iter_t servletIter = INSTANCE->_servlets.begin();
	while(servletIter != INSTANCE->_servlets.end()) {
		if (servletIter->second == servlet) {
			evhttp_del_cb(INSTANCE->_http, std::string("/" + servletIter->first).c_str());
			INSTANCE->_servlets.erase(servletIter);
			break;
		}
		servletIter++;
	}
}

void HTTPServer::start() {
	_isRunning = true;
	_thread = new tthread::thread(HTTPServer::run, this);
}

void HTTPServer::run(void* instance) {
	HTTPServer* INSTANCE = (HTTPServer*)instance;
	while(INSTANCE->_isRunning) {
		event_base_dispatch(INSTANCE->_base);
	}
	LOG(INFO) << "HTTP Server stopped" << std::endl;
}

void HTTPServer::determineAddress() {
	char hostname[1024];
	gethostname(hostname, 1024);
	_address = std::string(hostname);
}

}