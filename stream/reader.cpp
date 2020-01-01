#include <stdio.h>
#include "../src/imediareader.h"

using namespace std;
using namespace httplib;

class MediaReader : public IMediaReader{
    FILE* mFp;
public:
    bool open(const httplib::Request& req, 
                    const std::string& path, 
                    int64_t from, 
                    int64_t to) {
        mFp = fopen(path.c_str(), "rb");
        return mFp != nullptr;
    }

    void read(int64_t offset, 
            std::function<void(const char *data, size_t data_len)> dataSink,
            std::function<void(int info)> notify) {
        const int bufSize = 4096;
        char buf[bufSize];

        auto ret = fread(buf, 1, bufSize, mFp);
        if (ret < 0)
            notify(READER_ERROR);
        else{
            if (ret > 0)
                dataSink(buf, ret);
            if (ret < bufSize)
                notify(READER_EOF);
        }
    }

    int64_t size() {
        return -1;
    }

    void close(){
        fclose(mFp);
    }
};