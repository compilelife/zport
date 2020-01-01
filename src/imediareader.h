#ifndef I_MEDIA_READER
#define I_MEDIA_READER

#include "httplib.h"
#include <string>
#include <functional>
#include <cstdarg>

#define READER_EOF 1    //目前只对size()返回-1时有效
#define READER_ERROR 2

/**
 * \brief 代表一种媒体资源
 * 
 * 用户需要继承该类来定义自己的媒体资源
 * “媒体资源”主要要定义的行为有open/close/read/size
 */
class IMediaReader{
private:
    std::string mId;
public:
    virtual ~IMediaReader(){}

    void setId(int id){
        mId = std::to_string(id);
    }
    void log(const char* format, ...){
        va_list args;
        auto fmt = "["+mId+"] "+format +"\n";
        va_start(args, format);
        vprintf(fmt.c_str(), args);
        va_end(args);
    }
public:
    /**
     * 打开媒体资源
     * 
     * \param req 原始的客户端请求。一般不需要用到这个参数。
     * \param path 客户端请求时的path参数
     * \param from 客户端请求资源的起始位置。从0开始。当size()返回-1时可以忽略该参数
     * \param to 客户端请求资源的结束为止。-1表示到媒体资源结束。当size()返回-1时可以忽略该参数
     * \return 是否打开成功。如果打开失败，框架会确保不再调用IMedia的其他接口
     */
    virtual bool open(const httplib::Request& req, 
                    const std::string& path, 
                    int64_t from, 
                    int64_t to) = 0;

    /**
     * \brief 关闭媒体资源
     * 
     * 一般在媒体资源读取完毕，或出错时调用
     */
    virtual void close() = 0;

    /**
     * \brief 拉取媒体数据
     * 
     * 对于size()>0的媒体资源，zport会反复调用该接口直到数据满足(to-from+1)，或遇到READER_ERROR;
     * 对于size()<0的媒体资源，zport会反复调用该接口，直到遇到READER_EOF或EADER_ERROR;
     * 
     * \param offset 请求offset位置的数据。对于第一次调用，offset总是等于0，后续调用中offset会逐次累加上次dataSink中的data_len
     * \param notify 用于IMediaReader通知READER_ERROR或READER_EOF
     */
    virtual void read(int64_t offset, 
            std::function<void(const char *data, size_t data_len)> dataSink,
            std::function<void(int info)> notify) = 0;

    /**
     * \return 媒体资源大小
     */
    virtual int64_t size() = 0;
};

#endif