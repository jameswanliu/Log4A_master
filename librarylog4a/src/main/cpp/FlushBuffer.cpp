//
// Created by pqpo on 2018/2/10.
//
#include "includes/FlushBuffer.h"

FlushBuffer::FlushBuffer(FILE *log_file, size_t size) : capacity(size), log_file(log_file) {}

FlushBuffer::~FlushBuffer() {
    if (data_ptr != nullptr) {
        delete[] data_ptr;
    }
    if (release != nullptr) {
        delete release;
    }
}

size_t FlushBuffer::length() {
    if (data_ptr != nullptr && write_ptr != nullptr) {
        return write_ptr - data_ptr;
    }
    return 0;
}

void *FlushBuffer::ptr() {
    return data_ptr;//内容指针
}

size_t FlushBuffer::emptySize() {
    return capacity - length();//空闲容量
}

void FlushBuffer::write(void *data, size_t len) {

    if (data_ptr == nullptr) {
        capacity = (size_t) fmax(capacity, len);
        data_ptr = new char[capacity]{0};
        write_ptr = data_ptr;
    }

    size_t empty_size = emptySize();
    if (len < empty_size) {//如果当前空闲容量大于待写入的大小 则执行
        memcpy(write_ptr, data, len);//将内容cpy 到 写入的存储指针位置
        write_ptr += len;// 移动写入的指针
    } else {
        size_t now_len = length();//当前的占用内存的长度大小
        size_t new_capacity = now_len + len;//加上待写入的长度大小 等于新的容量
        char *data_tmp = new char[new_capacity]{0};//根据新的容量创建指针
        memcpy(data_tmp, data_ptr, now_len);//并复制到新的内存中
        memcpy(data_tmp + now_len, data, len);
        char *old_data = data_ptr;
        data_ptr = data_tmp;
        write_ptr = data_ptr + new_capacity;
        delete[] old_data;
    }
}

void FlushBuffer::reset() {
    if (data_ptr != nullptr) {
        memset(data_ptr, 0, capacity);
        write_ptr = data_ptr;
    }
}

FILE *FlushBuffer::logFile() {
    return log_file;
}

void FlushBuffer::releaseThis(void *release) {
    this->release = release;
}


