#include "server.h"
#include "httplib.h"
#include <functional>
#include "imediareader.h"
#include <iostream>

using namespace httplib;
using namespace std;

#include "reader.cpp"

class ZPortServerImpl{
    Server mHttpServer;
public:
    bool start(int port){
        mHttpServer.Get("/media", bind(&ZPortServerImpl::onNewMediaRequest, this, placeholders::_1, placeholders::_2));
        return mHttpServer.listen("0.0.0.0", port);
    }

    void stop(){
        mHttpServer.stop();
    }

    void onNewMediaRequest(const Request& req, Response& res){
        shared_ptr<MediaReader> reader(new MediaReader);
        auto path = req.get_param_value("path");
        int64_t from = 0;
        int64_t to = -1;
        if (!req.ranges.empty()){
            if (req.ranges.size()>1){
                cout<<"mulitpart request not support yet"<<endl;
            }
            auto range = req.ranges[0];
            if (range.first > 0)    from = range.first;
            if (range.second > 0)   to = range.second;
        }

        cout<<"rout request (path="<<path<<",range={"<<from<<","<<to<<"}) to MediaReader"<<endl;

        cout<<"open"<<endl;
        if (!reader->open(req, path, from, to)){
            cout<<"open failed"<<endl;
            res.status = 400;
            return;
        }

        auto len = reader->size();
        cout<<"media total size: "<<len<<endl;
        if (len > 0){
            res.set_chunked_content_provider(
                [reader](size_t offset, DataSink sink, Done done){
                    reader->read(offset, sink, [&done](int info){
                        if (info == READER_ERROR){//忽略EOF
                            done();
                        }
                    });
                },
                [reader]{reader->close();}
            );
            res.content_length = len;//hack for Done
        }else{
            res.set_chunked_content_provider(
                [reader](size_t offset, DataSink sink, Done done){
                    reader->read(offset, sink, [&done](int info){
                        if (info == READER_ERROR || info == READER_EOF){
                            done();
                        }
                    });
                },
                [reader]{reader->close();}
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