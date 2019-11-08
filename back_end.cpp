#include "back_end.h"
#include "capture.h"
#include "std_out_stream.h"
#include "platform.h"
#include <iostream>

static_assert(BufContainer::buf_size_ >= CapTure::buf_size_, "container buf_size should >= capture buf_size!");

//������������̰߳�ȫ�ģ���ǰ������ӵ�Ŧ����
//ǰ��ͨ���̰߳�ȫ�������һ���߳��е��ô˺���
void BackEnd::write(size_type index, const char* ptr, size_type n) {
  if (stop_.load() == true) {
    return;
  }
	rangecheck(index);
	if (out_stream(index) == nullptr) {
		pf::fprintf(stderr, "write non-opened file : %d\n", static_cast<int>(index));
		stop();
		exit(-1);
	}
	if (buf_container(index).inited() == false) {
		out_stream(index)->write(ptr, n);
	}
	else {
		bool succ = buf_container(index).write(ptr, n);
		if (succ == false) {
			pf::fprintf(stderr, "buf_container write error:%d\n",static_cast<int>(index));
			stop();
			exit(-1);
		}
	}
}

BackEnd::BackEnd(size_type size) :pool_(size),stop_(false) {
	for (size_type i = 0; i < FILES; ++i) {
		out_streams_[i] = nullptr;
	}
	out_stream(0).reset(new StdOutStream(stdout));
	out_stream(1).reset(new StdOutStream(stderr));
	pool_.start();
}

void BackEnd::open(size_type index,out_stream_base* out, size_type log_container_size) {
	rangecheck(index);
	if (out_stream(index) != nullptr) {
		pf::fprintf(stderr, "register opened file");
		stop();
		exit(-1);
	}

	out_stream(index) = std::shared_ptr<out_stream_base>(out);
	buf_container(index).init(log_container_size);

	pool_.push_task([this,index]()->bool {
		return buf_container(index).backEnd(out_stream(index));
		//��һ��bug��Ȼ������Ĳ��������⡣emm
		});
}

inline
std::shared_ptr<out_stream_base>& BackEnd::out_stream(size_type index) {
	rangecheck(index);
	return out_streams_[index];
}

inline
BufContainer& BackEnd::buf_container(size_type index) {
	rangecheck(index);
	return buf_containers_[index];
}

//��Ҫ���̰߳�ȫ��,�����ڲ�ͬ�̵߳���stop������
//������ԭ�ӱ���ʵ���̰߳�ȫ��
void BackEnd::stop() {
  bool exp = false;
	if (stop_.compare_exchange_strong(exp,true)) {
		for (size_type i = 2; i < FILES; ++i) {
			buf_container(i).stop();
		}
		pool_.stop();
	}
}

BackEnd::~BackEnd() {

}

void BackEnd::rangecheck(size_type index) const {
	if (index < 0 || index >= FILES) {
		pf::fprintf(stderr, "index out of range");
		const_cast<BackEnd*>(this)->stop();
		exit(-1);
	}
}