#include <stdio.h>
#include "../src/imediareader.h"

using namespace std;
using namespace httplib;

#define KEY_LEN 8

//以加密文件做媒体，从http推送解密后的数据
//这里我们假设了一种简单的加密：
//加密文件中，前8个字节是密钥，之后跟随加密后的视频数据。
//视频加密的方式是把视频数据与8字节秘钥做异或运算
class MediaReader : IMediaReader{
    FILE* mFp;
    int64_t mSize;
    int64_t mEndPos;
    uint8_t mKey[KEY_LEN];
    int mKeyOffset{0};
public:
    bool open(const httplib::Request& req, 
                    const std::string& path, 
                    int64_t from, 
                    int64_t to) {
        mFp = fopen(path.c_str(), "rb");
        if (!mFp)
            return false;

        fread(mKey, 1, KEY_LEN, mFp);
        
        //计算有效范围
        fseek(mFp, 0, SEEK_END);
        mSize = ftell(mFp) - KEY_LEN;
        mEndPos = to;
        if (mEndPos == -1 || mEndPos >= mSize){
            mEndPos  = mSize - 1 + KEY_LEN;
        }

        //定位到开始位置
        auto encryptedOffset = from + KEY_LEN;//对应的加密文件（实体文件）的偏移位置
        mKeyOffset = from - (from / KEY_LEN * KEY_LEN);//seek不会刚好落在8字节开头，所以要记录密钥偏移
        fseek(mFp, encryptedOffset, SEEK_SET);
        
        return true;
    }

    void decrypt(char* buf, size_t ret){
        char* pbuf = buf;
        do{
            *pbuf = (*pbuf) ^ (mKey[mKeyOffset++]);
            ++pbuf;
            if (mKeyOffset >= KEY_LEN) {
                mKeyOffset = 0;
            }
        }while((size_t)(pbuf - buf)<ret);
    }

    //这里顺带演示下由server多次调用read，被动传数据的情况
    void read(int64_t offset, 
            std::function<void(const char *data, size_t data_len)> dataSink,
            std::function<void(int info)> notify) {
        const int bufSize = KEY_LEN * 256;
        char buf[bufSize];
        auto len = mEndPos - ftell(mFp) + 1;
        if (len > bufSize)
            len = bufSize;
        auto ret = fread(buf, 1, len, mFp);
        if (ret < 0){
            notify(READER_ERROR);
            return;
        }
        decrypt(buf, ret);
        dataSink(buf, ret);
    }

    int64_t size() {
        return mSize;
    }

    void close(){
        fclose(mFp);
    }
};