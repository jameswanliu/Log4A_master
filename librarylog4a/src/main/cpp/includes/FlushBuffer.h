//
// Created by pqpo on 2018/2/10.
//

#ifndef LOG4A_FLUSHBUFFER_H
#define LOG4A_FLUSHBUFFER_H

#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <stdio.h>


/**
 *
 * buffer_ptr 是缓存文件映射内存的指针，前面说过缓存文件的格式为：第一个字节会用于存储日志路径名称长度，后面紧跟日志路径，之后才是日志信息
 * data_ptr 指向的就是日志内容开始的地方(指针)
 * write_ptr 指向的是目前写内存的起始位置。
 * 知道这几个指针的作用之后就容易理解了。
 *
 *
 */

class FlushBuffer {

    public:
        FlushBuffer(FILE* log_file, size_t size = 128);
        ~FlushBuffer();
        void write(void* data, size_t len);
        void reset();
        size_t length();
        void* ptr();
        FILE* logFile();

    void releaseThis(void *release);

private:
        FILE* log_file = nullptr;
        void* release = nullptr;
        char* data_ptr = nullptr;//日志容纳开始的指针
        char* write_ptr = nullptr;//写内存起始的指针位置
        size_t capacity;//容量大小

        size_t emptySize();

};


#endif //LOG4A_FLUSHBUFFER_H
