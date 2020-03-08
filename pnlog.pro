TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        src/back_end.cpp \
        src/capture.cpp \
        src/char_array_allocator.cpp \
        src/convert.cpp \
        src/file_out_stream.cpp \
        src/outer.cpp \
        src/pnlog.cpp \
        src/std_out_stream.cpp \
        src/str_appender.cpp \
        src/thread_pool.cpp \
        src/timer.cpp \
        src/type.cpp

HEADERS += \
    include/back_end.h \
    include/blocking_queue.h \
    include/capture.h \
    include/char_array.h \
    include/char_array_allocator.h \
    include/char_array_wrapper.h \
    include/condition_variable_type.h \
    include/convert.h \
    include/event_pool.h \
    include/file_out_stream.h \
    include/out_stream_base.h \
    include/outer.h \
    include/pnlog.h \
    include/release_assert.h \
    include/spin_lock.h \
    include/std_out_stream.h \
    include/str_appender.h \
    include/thread_pool.h \
    include/timer.h \
    include/type.h

DISTFILES += \
    README.md \
    src/CMakeLists.txt
