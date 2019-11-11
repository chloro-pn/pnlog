# pnlog
* [pnlog是什么？](#what-is-pnlog)

* [pnlog的特点?](#characteristic)

* [pnlog下一步计划？](#next-step)

* [如何使用pnlog?](#how-to-use-pnlog)
***
### <span id = "what-is-pnlog"> pnlog是什么</span> #
pnlog是一个基于c++11标准的日志记录库。
### <span id = "characteristic"> pnlog的特点</span> #
* 高效

    采用异步模型，日志首先会记录在缓冲buf中，当buf填满之后会由后台进程整块写入文件。

* 线程安全

    多个线程向同一文件写入日志是线程安全的（包括标准输出和标准错误）。

* 可打开多个日志文件自由选择写入

    在记录之前可以打开多个日志文件，每个文件占有一索引号，日志记录时可以根据索引号选择对应的日志文件记录。pnlog在开始前默认将0号索引分配给stdout，1号索引分配给stderr。
    
* 日志分级

    pnlog将日志分级为trace、debug、error、fatal。当调用fatal记录日志后，会将之前记录的所有日志存储文件并调用exit(-1)终止程序。
    
### <span id = "next-step">下一步计划</span>
* 跨平台

    由于文件操作时使用的是*_s系列函数，故目前只支持windows操作系统。
* 自定义日志格式
    
    目前并不支持自定义格式，尝试采用自定义格式时性能下降很快。
    
* 日志时间记录
    
    目前日志记录可以有选择的记录文件路径和行数，但不支持记录当前时间。
    
* 自定义类的日志记录机制
    
    目前仅支持std::to_string可以转化的类型、c式字符串、std::string等类向日志输入，不支持自定义类向日志的输入。
    
### <span id = "how-to-use-pnlog"> 如何使用pnlog</span> #
pnlog库提供两个全局对象：capture和backend，capture对象用来捕获并记录日志，backend对象用来进行日志文件相关的操作。

第一步：选择一个索引号并打开日志文件。

<code> backend.open(2, new FileOutStream("e://log.txt")); </code>

第二步：记录日志（可以选择记录文件名称和行号的重载函数）

<code>	capture.log_debug(2, piece("hello world !my number is ", 23));</code>

<code>  capture.log_error(1, \_\_LINE\_\_,\_\_FILE\_\_,piece("there is something wrong!"));</code>

第三步：程序结束前调用backend.stop();

<code> backend.stop() </code>

关于piece：

piece是一个函数模板，该模板支持输入多于或等于一个的任意类型的参数，只要该类型能够被std::to_string转化、或是c式字符串，或是std::string类型。piece将输入的参数按序添加入一个std::string对象并返回。
