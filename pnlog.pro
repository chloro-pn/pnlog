TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        back_end.cpp \
        capture.cpp \
        convert.cpp \
        file_out_stream.cpp \
        main.cpp \
        outer.cpp \
        pnlog.cpp \
        std_out_stream.cpp \
        str_appender.cpp \
        thread_pool.cpp \
        type.cpp

DISTFILES += \
    LICENSE \
    README.md \
    pnlog.pro.user

HEADERS += \
    back_end.h \
    blocking_queue.h \
    capture.h \
    char_array.h \
    condition_variable_type.h \
    convert.h \
    event_pool.h \
    file_out_stream.h \
    out_stream_base.h \
    outer.h \
    pnlog.h \
    spin_lock.h \
    std_out_stream.h \
    str_appender.h \
    thread_pool.h \
    type.h
