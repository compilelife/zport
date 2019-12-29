#ifndef I_MEDIA_READER
#define I_MEDIA_READER

#include "httplib.h"
#include <string>
#include <functional>

#define READER_EOF 1    //目前只对size()返回-1时有效
#define READER_ERROR 2

class IMediaReader{
public:
    virtual ~IMediaReader(){}
public:
    virtual bool open(const httplib::Request& req, 
                    const std::string& path, 
                    int64_t from, 
                    int64_t to) = 0;

    virtual void close() = 0;
    virtual void read(int64_t offset, 
            std::function<void(const char *data, size_t data_len)> dataSink,
            std::function<void(int info)> notify) = 0;
    virtual int64_t size() = 0;
};

#endif