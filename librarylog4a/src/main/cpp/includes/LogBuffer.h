//
// Created by pqpo on 2017/11/16.
//

#ifndef LOG4A_LOGBUFFER_H
#define LOG4A_LOGBUFFER_H

#include <string>
#include <math.h>
#include <unistd.h>
#include <sys/mman.h>
#include <thread>
#include<vector>
#include <mutex>
#include <condition_variable>
#include "AsyncFileFlush.h"
#include "FlushBuffer.h"
#include "LogBufferHeader.h"
#include <zlib.h>

using namespace log_header;


/**
 *
 * buffer_ptr 是缓存文件映射内存的指针，前面说过缓存文件的格式为：第一个字节会用于存储日志路径名称长度，后面紧跟日志路径，之后才是日志信息
 * data_ptr 指向的就是日志内容开始的地方(指针)
 * write_ptr 指向的是目前写内存的起始位置。
 * 知道这几个指针的作用之后就容易理解了。
 *
 *
 */
class LogBuffer {
public:
    LogBuffer(char* ptr, size_t capacity);
    ~LogBuffer();

    void initData(char *log_path, size_t log_path_len, bool is_compress);
    size_t length();
    size_t append(const char* log, size_t len);
    void release();
    size_t emptySize();
    char *getLogPath();
    void setAsyncFileFlush(AsyncFileFlush *fileFlush);
    void async_flush();
    void async_flush(AsyncFileFlush *fileFlush);
    void async_flush(AsyncFileFlush *fileFlush, LogBuffer *releaseThis);
    void changeLogPath(char *log_path);

public:
    bool map_buffer = true;

private:
    void clear();
    void setLength(size_t len);
    bool initCompress(bool compress);
    bool openSetLogFile(const char *log_path);

    FILE* log_file = nullptr;
    AsyncFileFlush *fileFlush = nullptr;
    char* const buffer_ptr = nullptr;
    char* data_ptr = nullptr;
    char* write_ptr = nullptr;

    size_t buffer_size = 0;
    std::recursive_mutex log_mtx;

    LogBufferHeader logHeader;
    z_stream zStream;
    bool is_compress = false;

};


#endif //LOG4A_LOGBUFFER_H
