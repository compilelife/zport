#include "gtest/gtest.h"
#include "../src/server.h"
#include <memory>
#include <future>
#include <thread>
#include <unistd.h>
#include <regex>

using namespace std;
#define PORT 10000

// TEST(Base, StartStop){
//     shared_ptr<ZPortServer> server(new ZPortServer);

//     promise<bool> startRet;
//     thread([server, &startRet]{
//         startRet.set_value(server->start(PORT));
//     }).detach();

//     usleep(10000);//wait http server started

//     server->stop();
//     auto ret = startRet.get_future().get();
//     ASSERT_TRUE(ret);
// }

TEST(Base, regex){
    regex re("http(s)?://([^/:]+)(:(\\d+))?(/.+)?");
    smatch m;
    string url = "https://www.damaijiankang:10080/index.html";
    if (regex_match(url, m, re)){
        cout<<m.size()<<endl;
        for (size_t i = 0; i < m.size(); i++)
        {
            cout<<m[i]<<endl;
        }
    }
}
