//g++ -std=c++11 encrypt.cpp
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define KEY_LEN 8

//这是对应的加密代码，供参考
static void encrypt(const char* path, const char* savepath){
    uint8_t key[KEY_LEN];
    srand(time(nullptr));

    for (size_t i = 0; i < KEY_LEN; i++){
        key[i] = rand()%256;
    }

    FILE* from = fopen(path, "rb");
    FILE* to = fopen(savepath, "wb+");
    if (from == nullptr || to == nullptr){
        exit(-1);
    }
    
    fwrite(key, 1, KEY_LEN, to);
    while (!feof(from)){
        const int size = KEY_LEN*256;
        char buf[size];
        auto ret = fread(buf, 1, size, from);
        for (size_t i = 0; i < ret; i++){
            buf[i] ^= key[i%KEY_LEN];
        }
        fwrite(buf, 1, ret, to);
    }
    
    fclose(from);
    fclose(to);
}

int main(int argc, char* argv[]){
    encrypt(argv[1],argv[2]);
    return 0;
}