#pragma once
#include <memory>
#include <functional>

class chain {
private:
	std::unique_ptr<chain> next_;
	std::function<void()> func_;

public:
	chain() = default;
	
	template<class Func,class... Args>
	chain(const Func& f, const Args&... args) {
		func_ = [=]()->void {
			f(args...);
		};
	}
	
	template<class Func,class... Args>
	chain& after(const Func& f, const Args&... args) {
		next_.reset(new chain);
		next_->func_ = [=]()->void {
			f(args...);
		};
		return *next_;
	}

	void operator()() {
		if (func_) {
			func_();
		}
		if (next_) {
			(*next_)();
		}
	}

	std::function<void()> getFunc() {
		return func_;
	}

	~chain() = default;
};