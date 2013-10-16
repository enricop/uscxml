#ifndef XmlBridgeInvoker_H_W09J90F0
#define XmlBridgeInvoker_H_W09J90F0

#include <map>
#include <iostream>

#include <uscxml/Interpreter.h>
#include <glog/logging.h>
#include "uscxml/plugins/invoker/system/timio.h"

#ifdef BUILD_AS_PLUGINS
#include "uscxml/plugins/Plugins.h"
#endif

namespace uscxml {

#define MESwriteCMD "MESwrite";
#define MESreadCMD "MESread";

class XmlBridgeInvoker : public InvokerImpl {
public:
//	enum Action {
//		MES,
//		COMMAND = 1,
//		REPLY = 2,
//		TIMEOUT = 4,
//		EXISTING = 8
//	};

	XmlBridgeInvoker() : _thread(NULL) {}
	~XmlBridgeInvoker();

	std::set<std::string> getNames() {
		std::set<std::string> names;
		names.insert("xmlbridge");
		return names;
	}

	boost::shared_ptr<InvokerImpl> create(InterpreterImpl* interpreter);
	void send(const SendRequest& req);
	void invoke(const InvokeRequest& req);
	Data getDataModelVariables();

	void buildMESreq(unsigned int cmd, const std::string reply_raw_data);
	void buildTIMreply(const char &cmdid, const std::string reply_raw_data);

	/* move invoker to new thread */
	static void run(void* instance) {
		while(((XmlBridgeInvoker*)instance)->_isRunning) {
			//((XmlBridgeInvoker*)instance)->_watcher->updateEntries();
			tthread::this_thread::sleep_for(tthread::chrono::milliseconds(20));
		}
	}

protected:
	std::string _DBid;
	bool _isRunning;
	tthread::thread* _thread;
	tthread::recursive_mutex _mutex;
};

class XmlBridgeInputEvents {
public:
	~XmlBridgeInputEvents();

	void sendTIMreq(const char& cmdid, const std::string reqData);
	void sendMESreply(std::string DBid, const char& cmdid, const std::string replyData);

	void handleTIMreply(const char& cmdid, const std::string replyData);
	void handleMESreq(unsigned int DBid, unsigned int offset, const std::string reqData="");

	void registerInvoker(std::string DBid, XmlBridgeInvoker* invokref) {
		_invokers[DBid] = invokref;
	}
	void registerTimio(TimIO* ioref) {
		_timio = ioref;
	}
	static XmlBridgeInputEvents& getInstance() {
		static XmlBridgeInputEvents instance;
		return instance;
	}

private:
	XmlBridgeInputEvents() : _lastChecked(0) {
		LOG(INFO) << "Instantiating XmlBridgeInputEvents Singleton";
	}
	XmlBridgeInputEvents& operator=( const XmlBridgeInputEvents& );

	time_t _lastChecked;

	/** One invoker/interpreter for each datablock */
	std::map<std::string, XmlBridgeInvoker*> _invokers;
	TimIO* _timio;
};

#ifdef BUILD_AS_PLUGINS
PLUMA_INHERIT_PROVIDER(XmlBridgeInvoker, InvokerImpl);
#endif

}

#endif /* end of include guard: XmlBridgeInvoker_H_W09J90F0 */
