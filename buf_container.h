#pragma once
#include "char_array.h"
#include "type.h"
#include <list>


namespace pnlog {
  class BufContainer {
    friend class BackEnd;
  private:
    using CharArrayType = CharArray;
    std::list<CharArrayType> bufs_;
    size_type max_size_; //max size of bufs_

  public:

    explicit BufContainer(size_type);

    bool empty() const;
    
    bool full() const;

    CharArray get_first_buf();

    void push_buf(CharArray&&);

    void push_buf(const CharArray&);

    ~BufContainer();
  };
}//namespace pnlog