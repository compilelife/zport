#include "src/server.h"

int main(int argc, char* argv[]){
    ZPortServer server;
    server.start(10000);
    return 0;
}