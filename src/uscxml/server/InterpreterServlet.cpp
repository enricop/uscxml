#include "InterpreterServlet.h"
#include "uscxml/Interpreter.h"
#include <glog/logging.h>

namespace uscxml {

InterpreterServlet::InterpreterServlet(InterpreterImpl* interpreter) {
	_interpreter = interpreter;

	std::stringstream path;
	path << _interpreter->getName();
	int i = 2;
	while(!HTTPServer::registerServlet(path.str(), this)) {
		path.clear();
		path.str();
		path << _interpreter->getName() << i++;
	}
	_path = path.str();
}

boost::shared_ptr<IOProcessorImpl> InterpreterServlet::create(InterpreterImpl* interpreter) {
	// we instantiate directly in Interpreter
	boost::shared_ptr<IOProcessorImpl> io = boost::shared_ptr<InterpreterServlet>(new InterpreterServlet(interpreter));
	return io;
}

bool InterpreterServlet::httpRecvRequest(const HTTPServer::Request& req) {
	tthread::lock_guard<tthread::recursive_mutex> lock(_mutex);

	//  evhttp_request_own(req.curlReq);

	_requests[toStr((uintptr_t)req.curlReq)] = req;

	Event event = req;

	event.name = "http." + event.data.compound["type"].atom;
	event.origin = toStr((uintptr_t)req.curlReq);

	if (event.data.compound["content"]) {
		if (event.data.compound["content"].compound.size() > 0) {
			std::map<std::string, Data>::iterator compoundIter = event.data.compound["content"].compound.begin();
			while(compoundIter != event.data.compound["content"].compound.end()) {
//				std::cout << compoundIter->second.atom << std::endl;
				Data json = Data::fromJSON(compoundIter->second.atom);
				if (json) {
//					std::cout << Data::toJSON(json) << std::endl;
					compoundIter->second = json;
				}
				compoundIter++;
			}
		}
	}
	
	_interpreter->receive(event);
	return true;
}

Data InterpreterServlet::getDataModelVariables() {
	Data data;
	assert(_url.length() > 0);
	data.compound["location"] = Data(_url, Data::VERBATIM);
	return data;
}

void InterpreterServlet::send(const SendRequest& req) {
	LOG(ERROR) << "send not supported by http iorprocessor, use the fetch element";
}

}