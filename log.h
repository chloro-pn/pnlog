#pragma once
#include "back_end.h"
#include "capture.h"
#include "convert.h"
#include <string>

extern BackEnd backend;
extern CapTure logger;

template<class... Types>
struct inner;

template<class T, class... Remain>
struct inner<T, Remain...> {
	static std::string get(const T& t, const Remain&... r) {
		std::string it = convert_to_string(t);
		std::string appender = inner<Remain...>::get(r...);
		it += appender;
		return it;
	}
};

template<class lastT>
struct inner<lastT> {
	static std::string get(const lastT& t) {
		return convert_to_string(t);
	}
};

template<>
struct inner<> {
	static std::string get() {
		return std::string();
	}
};

template<class... Types>
std::string str_appender(const Types&... args) {
	return inner<Types...>::get(args...);
}

#define LOG(x,y,z) logger.log(x,y,__LINE__,__FILE__,z)

#define LOG_TRACE(x,y) if(logger.getLevel() <= CapTure::Level::TRACE)  \
LOG(x,CapTure::Level::TRACE,y)

#define LOG_DEBUG(x,y) if(logger.getLevel() <= CapTure::Level::DEBUG)  \
LOG(x,CapTure::Level::DEBUG,y)

#define LOG_ERROR(x,y) if(logger.getLevel() <= CapTure::Level::ERROR)  \
LOG(x,CapTure::Level::ERROR,y)

#define LOG_FATAL(x,y) if(logger.getLevel() <= CapTure::Level::FATAL)  \
LOG(x,CapTure::Level::FATAL,y)

#define LOG_OPEN(x,y) backend.register_out_stream(x,y)

#define LOG_LEVEL(x) logger.setLevel(x)

#define LOG_END backend.stop()