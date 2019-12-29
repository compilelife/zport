#include <stdio.h>
#include "../src/imediareader.h"

using namespace std;
using namespace httplib;

class MediaReader : IMediaReader{
    FILE* mFp;
    int64_t mSize;
    int64_t mEndPos;
public:
    bool open(const httplib::Request& req, 
                    const std::string& path, 
                    int64_t from, 
                    int64_t to) {
        mFp = fopen(path.c_str(), "rb");
        if (!mFp)
            return false;
        
        //计算有效范围
        fseek(mFp, 0, SEEK_END);
        mSize = ftell(mFp);
        mEndPos = to;
        if (mEndPos == -1 || mEndPos >= mSize){
            mEndPos  = mSize - 1;
        }

        //定位到开始位置
        fseek(mFp, from, SEEK_SET);
        
        return true;
    }

    void read(int64_t offset, 
            std::function<void(const char *data, size_t data_len)> dataSink,
            std::function<void(int info)> notify) {
        const int bufSize = 4096;
        char buf[bufSize];

        do{
            auto left = mEndPos - offset + 1;
            if (left > bufSize)
                left = bufSize;
            else if (left <= 0)
                break;

            auto ret = fread(buf, 1, left, mFp);
            if (ret < 0)
                notify(READER_ERROR);
            else if (ret == 0)
                break;
            else
                dataSink(buf, ret);
        }while (true);
    }

    int64_t size() {
        return mSize;
    }

    void close(){
        fclose(mFp);
    }
};