#ifndef SERVER_H
#define SERVER_H

class ZPortServerImpl;

/**
 * \brief Zport的服务控制类
 */
class ZPortServer
{
public:
    ZPortServer();
    ~ZPortServer();
public:
    /**
     * \brief 在指定端口启动http服务
     * 
     * 服务启动后会阻塞当前线程，直到服务退出
     * 
     * \param port 要启动http服务的端口
     * \return false - 服务启动失败，或服务运行期间有错误发生
     */
    bool start(int port);

    /**
     * 停止http服务
     */
    void stop();
private:
    ZPortServerImpl* mImpl;
};

#endif