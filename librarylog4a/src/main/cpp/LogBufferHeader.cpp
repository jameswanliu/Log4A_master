//
// Created by pqpo on 2018/2/10.
//

#include <ctime>
#include "includes/LogBufferHeader.h"

using namespace log_header;
/**
 *
 * size_t实际上是无符号长整型，在32位系统上位32位，在64位系统中位64位。
 *
 * @param data
 * @param size
 */
//struct Header {
//    char magic;
//    size_t log_len;
//    size_t log_path_len;
//    char* log_path;
//    char isCompress;
//};

LogBufferHeader::LogBufferHeader(void *data, size_t size) : data_ptr((char *) data),
                                                            data_size(size) {
}

LogBufferHeader::~LogBufferHeader() {
}

void *LogBufferHeader::originPtr() {
    return data_ptr;
}

Header *LogBufferHeader::getHeader() {
    Header *header = new Header();
    if (isAvailable()) {
        header->magic = kMagicHeader;
        size_t log_len = 0;
        memcpy(&log_len, data_ptr + sizeof(char),
               sizeof(size_t));//从存储char 字节长度的位置开始分配size_t长度字符给 log_len
        header->log_len = log_len;
        size_t log_path_len = 0;
        memcpy(&log_path_len, data_ptr + sizeof(char) + sizeof(size_t),
               sizeof(size_t));//log_len 占用了size_t 从存储区域的data_ptr + sizeof(char) + sizeof(size_t) 开始分配size_t长度字符给 log_path_len
        header->log_path_len = log_path_len;
        char *log_path = new char[log_path_len + 1];//前size_t 存储日志路径长度， +1后开始存储日志路径
        memset(log_path, 0, log_path_len + 1);// 复制字符 0（一个无符号字符）到参数 str 所指向的字符串的前 n 个字符。
        memcpy(log_path, data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t),
               log_path_len);//从指针 data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t), log_path_len)开始 分配 log_path_len个字符到log_path
        header->log_path = log_path;
        char isCompress = (data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t) +
                           log_path_len)[0];
        header->isCompress = isCompress == 1;
    }
    return header;
}

size_t LogBufferHeader::getHeaderLen() {
    if (isAvailable()) {
        return calculateHeaderLen(getLogPathLen());
    }
    return 0;
}

void *LogBufferHeader::ptr() {
    return data_ptr + getHeaderLen();
}

void *LogBufferHeader::write_ptr() {
    return data_ptr + getHeaderLen() + getLogLen();
}

void LogBufferHeader::initHeader(Header &header) {
    if ((sizeof(char) + sizeof(size_t) + sizeof(size_t) + header.log_path_len) > data_size) {
        return;
    }
    memcpy(data_ptr, &header.magic, sizeof(char));
    memcpy(data_ptr + sizeof(char), &header.log_len, sizeof(size_t));
    memcpy(data_ptr + sizeof(char) + sizeof(size_t), &header.log_path_len, sizeof(size_t));
    memcpy(data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t), header.log_path,
           header.log_path_len);
    char isCompress = 0;
    if (header.isCompress) {
        isCompress = 1;
    }
    memcpy(data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t) + header.log_path_len,
           &isCompress,
           sizeof(char));

}

size_t LogBufferHeader::getLogLen() {
    if (isAvailable()) {
        size_t log_len = 0;
        memcpy(&log_len, data_ptr + sizeof(char), sizeof(size_t));
        // log长度总是大于 0 并小于 buffer_size 减去 header 长度的
        if (log_len <= 0 || log_len > (data_size - getHeaderLen())) {
            log_len = 0;
        }
        return log_len;
    }
    return 0;
}

size_t LogBufferHeader::getLogPathLen() {
    if (isAvailable()) {
        size_t log_path_len = 0;
        memcpy(&log_path_len, data_ptr + sizeof(char) + sizeof(size_t), sizeof(size_t));
        // logpath 的长度不能大于整个buffer 减去header 中其它数据的长度
        if (log_path_len <= 0 || log_path_len > data_size - calculateHeaderLen(0)) {
            log_path_len = 0;
        }
        return log_path_len;
    }
    return 0;
}

char *LogBufferHeader::getLogPath() {
    if (isAvailable()) {
        size_t log_path_len = getLogPathLen();
        if (log_path_len > 0) {
            char *log_path = new char[log_path_len + 1];
            memset(log_path, 0, log_path_len + 1);
            memcpy(log_path, data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t),
                   log_path_len);
            return log_path;
        }
    }
    return nullptr;
}

void LogBufferHeader::setLogLen(size_t log_len) {
    if (isAvailable()) {
        memcpy(data_ptr + sizeof(char), &log_len, sizeof(size_t));
    }
}

bool LogBufferHeader::isAvailable() {
    return data_ptr[0] == kMagicHeader;
}

bool LogBufferHeader::getIsCompress() {
    if (isAvailable()) {
        char isCompress = (data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t) +
                           getLogPathLen())[0];
        return isCompress == 1;
    }
    return false;
}

size_t LogBufferHeader::calculateHeaderLen(size_t log_path_len) {
    return sizeof(char) + sizeof(size_t) + sizeof(size_t) + log_path_len + sizeof(char);
}






