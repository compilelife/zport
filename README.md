zport 是一个轻量且使用方便的http推流库。通过它，你可以让自定义的媒体数据轻易地在任意播放器上播放。

zport特点：
- 易用的接口，可以轻易对接你的媒体源
- 支持seek
- 轻量，c++编写，无其他依赖，跨平台，容易移植、集成

# 使用

为了方便最低成本理解代码，并使用该项目。作者避免了引入复杂的代码层级结构。

代码分两部分：zport框架 + MediaReader

zport框架代码在`src`目录，一般无需修改。

`MediaReader`是你需要实现的内容，通过实现`MediaReader`把要推流的资源描述清楚，

比如`file/reader.cpp`是一个“读取本地文件进行推流的MediaReader”，最简单的编译方法：

```
g++ -o server -std=c++11 -I./file -I./src src/*.cpp main.cpp
```

执行`./server`就可以通过`http://127.0.0.1:1000/media?path=/path/to/file`访问本地文件了

## IMediaReader

`IMediaReader`是提供给用户继承实现的。

主要的方法是：`open/close/read/size`。参考`file/reader.cpp`的实现。

特别指出的是`read`的实现：

```c++
virtual void read(int64_t offset, 
            std::function<void(const char *data, size_t data_len)> dataSink,
            std::function<void(int info)> notify)
```

zport会在推流过程中不断回调该方法，直到读到所有数据。

用户有两种实现方案：

1. 在单次read中多次调用dataSink，把所有数据给zport
2. 每次read，只作一次dataSink，取可用数据给zport

上述两种方法对zport而言，并没有区别，可以按照你喜欢的方式使用。

** 不可seek资源 **

对于不可seek资源，通过`size()`返回小于0的数值，告知zport，zport就会以`chunked`方式推流。

实现`IMediaReader::read`时，通过`notify(READER_EOF)`告知zport数据完成

** 支持seek的资源 **

对于支持seek的资源，就需要通过`size()`返回整个资源的长度，并正确响应`open`时传入的请求，特别是`from/to`指定的范围。

实现`IMediaReader::read`时，需要一直返回足够的数据(`to-from+1`)后，zport才会停止回调（数据不足，会忽略`READER_EOF`)。中途只能通过`READER_ERROR`出错退出。

## CMakeLists.txt编译

```shell
mkdir build
cd build
cmake .. -DREADER=file -DBUILD_EXE=1 -DCMAKE_BUILD_TYPE=Debug
```

cmake支持的参数：

- READER: 指定reader.cpp所在的目录，比如指定`file`，则编译`file/reader.cpp`
- BUILD_EXE: 指定是否编译为可执行程序，与BUILD_TEST冲突。不指定或0，则编译为动态库
- BUILD_TEST: 编译测试代码（需要先下载[googletest](https://github.com/google/googletest)代码到test目录下）
- CMAKE_BUILD_TYPE: Debug编译调试模式代码；Release编译发布模式代码

# 开发计划

- 注释
- 清晰的日志打印
- 支持https
- test

# 代码结构

- src: zport代码
- file/decrypt: MediaReader的各种example
- test：单元测试代码
- mockserver: 一个简单的http server，测试用

# 关于http MediaReader的实现

要代理远程http的资源，你需要学习一种http client的API，以拉取远程资源

并且，一般的http模块，提供的API都是”推数据“模式，也就是注册一个回调，由模块告诉你数据ready了，来拿走。在做http推流的时候，我们希望数据是”拉“模式——”我要多少数据，立即给我“。两种模式的转换方法需要一定技巧，在example在实现有点喧宾夺主

“拉http资源 -> 转换 -> 推流”算是共性需求，写了example，难免让“拿来主义”盛行——拷贝代码就走人了

虽然是共性需求，但每个需求中的服务器不同，对于转换的实现不同，性能要求不同，都会在很大程度上影响如何实现。也是要花精力去设计和调优，example不一定能覆盖所有需求。参考第三点，如果出了代码质量问题，这锅zport不背

鉴于以上4点（和作者的私心），如果有需要http MediaReader example的，请扫描下方二维码给予作者支持，然后发送邮件到`505610414@qq.com`，标题注明"zport+您的微信号"，会自动回复代码到您的邮箱。

# 赞助

**如果对本项目有兴趣，可以微信扫码支持我**

![support](images/support.jpg)

**如您需要作者为您定制该项目，可以加QQ 505610414详谈**