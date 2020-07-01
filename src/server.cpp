#include "server.h"
#include "httplib.h"
#include <functional>
#include "imediareader.h"
#include <stdio.h>
#include <atomic>
#include <cinttypes>

using namespace httplib;
using namespace std;

#include "reader.cpp"


class ZPortServerImpl{
    Server mHttpServer;
    atomic_int mMediaIdGenerator{0};

    string mLogTag{"[server] "};
    void log(const char* format, ...){
        va_list args;
        auto fmt = mLogTag+format+"\n";
        va_start(args, format);
        vprintf(fmt.c_str(), args);
        va_end(args);
    }
public:
    bool start(int port){
        mHttpServer.Get("/media", bind(&ZPortServerImpl::onNewMediaRequest, this, placeholders::_1, placeholders::_2));
        log("start");
        return mHttpServer.listen("0.0.0.0", port);
    }

    void stop(){
        log("stop");
        mHttpServer.stop();
    }

    void onNewMediaRequest(const httplib::Request& req, Response& res){
        shared_ptr<MediaReader> reader(new MediaReader);
        auto path = req.get_param_value("path");
        int64_t from = 0;
        int64_t to = -1;
        if (!req.ranges.empty()){
            if (req.ranges.size()>1){
                log("mulitpart request not support yet");
            }
            auto range = req.ranges[0];
            if (range.first > 0)    from = range.first;
            if (range.second > 0)   to = range.second;
        }

        auto id = ++mMediaIdGenerator;
        log("rout request (path=%s, range={%" PRId64 ",%" PRId64 "}) to MediaReader %d",
            path.c_str(), from, to , id);

        reader->setId(id);
        reader->log("open");
        if (!reader->open(req, path, from, to)){
            reader->log("open failed");
            res.status = 400;
            return;
        }

        auto len = reader->size();
        reader->log("content_length: %" PRId64, len);
        if (len > 0){
            //本来应该使用set_content_provider，但是set_content_provider后取不到Done，无法在出错时通知到httplib
            //所以这里通过修改私有字段content_length，配合set_chunked_content_provider，实现了带Done参数的set_content_provider概念
            res.set_chunked_content_provider(
                [reader](size_t offset, DataSink sink, Done done){
                    reader->read(offset, sink, [&](int info){
                        if (info == READER_ERROR){//忽略EOF
                            reader->log("read error");
                            done();
                        }
                    });
                },
                [reader]{
                    reader->log("close");
                    reader->close();
                }
            );
            res.content_length = len;//hack for Done
        }else{
            res.set_chunked_content_provider(
                [reader](size_t offset, DataSink sink, Done done){
                    reader->read(offset, sink, [&](int info){
                        if (info == READER_ERROR){
                            reader->log("read error");
                            done();
                        }
                        else if (info == READER_EOF){
                            reader->log("eof");
                            done();
                        }
                    });
                },
                [reader]{
                    reader->log("close");
                    reader->close();
                }
            );
        }
    }
};

ZPortServer::ZPortServer()
    :mImpl(new ZPortServerImpl){
}

ZPortServer::~ZPortServer(){
    delete mImpl;
}

bool ZPortServer::start(int port) {
    return mImpl->start(port);
}

void ZPortServer::stop(){
    mImpl->stop();
}