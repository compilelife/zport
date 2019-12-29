#ifndef SERVER_H
#define SERVER_H

class ZPortServerImpl;

class ZPortServer
{
public:
    ZPortServer();
    ~ZPortServer();
public:
    bool start(int port);
    void stop();
private:
    ZPortServerImpl* mImpl;
};

#endif