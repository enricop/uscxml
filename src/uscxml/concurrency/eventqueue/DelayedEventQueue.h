#ifndef DELAYEDEVENTQUEUE_H_JA6WRBVP
#define DELAYEDEVENTQUEUE_H_JA6WRBVP

#include "uscxml/concurrency/tinythread.h"

#include <event2/thread.h>
#include <event2/http.h>
#include <event2/event.h>

#include <inttypes.h>

#include <map>
#include <string>
#include <iostream>

namespace uscxml {

class DelayedEventQueue {
public:

	enum OpMask {
		DEQ_READ = EV_READ,
		DEQ_WRITE = EV_WRITE,
		DEQ_SIGNAL = EV_SIGNAL
	};
	
	struct callbackData {
		void *userData;
		void (*callback)(void*, const std::string eventId);
		std::string eventId;
		bool persist;
		struct event *event;
		DelayedEventQueue* eventQueue;
	};

	DelayedEventQueue();
	virtual ~DelayedEventQueue();

	void addEvent(std::string eventId, int fd, short opMask, void (*callback)(void*, const std::string eventId), void* userData, bool persist = true);
	void addEvent(std::string eventId, void (*callback)(void*, const std::string eventId), uint32_t delayMs, void* userData, bool persist = false);
	void cancelEvent(std::string eventId);

	void start();
	void stop();
	static void run(void*);

	bool isEmpty() {
		return _callbackData.empty();
	}

	static void timerCallback(evutil_socket_t fd, short what, void *arg);
	static void fileCallback(evutil_socket_t fd, short what, void *arg);
	static void dummyCallback(evutil_socket_t fd, short what, void *arg);

	bool _isStarted;
	tthread::thread* _thread;
	tthread::recursive_mutex _mutex;

	std::map<std::string, callbackData> _callbackData;
	struct event_base* _eventLoop;
};

}


#endif /* end of include guard: DELAYEDEVENTQUEUE_H_JA6WRBVP */
