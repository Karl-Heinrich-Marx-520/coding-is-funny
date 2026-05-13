# C++ 网络编程学习路线（C++20+，含可编译示例与 CMake）

目录

1. 前言与使用建议
2. 网络基础（OSI/TCP/UDP/IP）
3. BSD sockets（同步编程）
4. 线程模型与并发
5. 异步模型与 IO 复用（reactor/proactor、epoll/kqueue/IOCP、io_uring）
6. C++20 特性在网络中的应用（coroutines、moves、concepts、ranges）
7. 常用库与生态（Boost.Asio / standalone Asio、Boost.Beast、OpenSSL、gRPC、nlohmann/json、vcpkg/Conan）
8. 协议与实践（HTTP/1.1/2/3、WebSocket、QUIC、TLS、gRPC）
9. 性能优化（zero-copy、缓冲、池化、批处理）
10. 调试与测试（ASAN/TSAN、profilers、benchmarks、CI）
11. 安全实践（证书、TLS、鉴权、常见漏洞）
12. 项目清单（从易到难）、3/6 个月学习计划、里程碑
13. 常见学习陷阱与面试常问题目

---

说明：所有示例均以 C++20 为准并给出 CMake 构建指令。示例以 POSIX（Linux/macOS）为主；Windows 下需对 socket API（Winsock）做少量改动（WSAStartup、closesocket 等），在示例中会标注。

---

## 1. 前言与使用建议

学习目标

- 明确从零到可用的路线：从网络基础到生产级工程/性能调优
- 学会使用现代 C++（C++20）工具链与生态构建网络应用

建议

- 在 Linux 或 WSL 下编译运行（推荐），安装 cmake、g++/clang、valgrind、OpenSSL、vcpkg 或 Conan
- 每章做练习并完成对应小项目里程碑

预计总投入：3~6 个月（视深度），见第12章详细计划

---

## 2. 网络基础（OSI/TCP/UDP/IP）

学习目标

- 理解分层模型、TCP 与 UDP 的差异、三次握手/四次挥手、端口与套接字语义、MTU、重传与拥塞控制

核心概念

- OSI/TCP/IP 分层、端到端语义、可靠性 vs 无连接、流量控制 vs 拥塞控制、Nagle、Keepalive

可运行示例：DNS / 地址解析（getaddrinfo）

文件：dns_lookup.cpp

```cpp
// dns_lookup.cpp  — POSIX
#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

int main(int argc, char** argv){
    if (argc < 2) { std::cerr << "Usage: dns_lookup <hostname>\n"; return 1; }
    const char* host = argv[1];
    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = 0;
    addrinfo* res = nullptr;
    int err = getaddrinfo(host, nullptr, &hints, &res);
    if (err) { std::cerr << "getaddrinfo: " << gai_strerror(err) << "\n"; return 1; }
    for (addrinfo* p = res; p != nullptr; p = p->ai_next) {
        char buf[INET6_ADDRSTRLEN];
        void* addr = nullptr;
        if (p->ai_family == AF_INET) {
            addr = &reinterpret_cast<sockaddr_in*>(p->ai_addr)->sin_addr;
        } else {
            addr = &reinterpret_cast<sockaddr_in6*>(p->ai_addr)->sin6_addr;
        }
        inet_ntop(p->ai_family, addr, buf, sizeof(buf));
        std::cout << (p->ai_family==AF_INET ? "IPv4: " : "IPv6: ") << buf << "\n";
    }
    freeaddrinfo(res);
    return 0;
}
```

CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.10)
project(dns_lookup)
set(CMAKE_CXX_STANDARD 20)
add_executable(dns_lookup dns_lookup.cpp)
```

构建运行：

```
mkdir build && cd build
cmake ..
cmake --build .
./dns_lookup example.com
```

练习题

- 分别用 getaddrinfo 查询 A/AAAA 记录并打印
- 解释 TCP 三次握手的每一步为何必要

小项目（里程碑）

- 实现一个工具：输入域名返回所有 IP、地理位置（可用 IP->Geo API）
预计时长：1 周

推荐资源

- RFC 793/TCP，RFC 791/IP，经典网络教材《TCP/IP 详解（卷1）》

---

## 3. BSD sockets（同步编程）

学习目标

- 掌握 socket()、bind()、listen()、accept()、connect()、send()/recv()、shutdown()，理解阻塞与非阻塞行为

核心概念

- 文件描述符/套接字、地址族、SO_REUSEADDR、listen backlog、半关闭

可运行示例：阻塞 TCP Echo Server / Client（POSIX）

server.cpp

```cpp
// echo_server.cpp (POSIX blocking)
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

int main(int argc,char**argv){
    const char* port = (argc>1)?argv[1]:"5555";
    addrinfo hints{}, *res;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (int err = getaddrinfo(nullptr, port, &hints, &res); err) { std::cerr<<gai_strerror(err)<<"\n"; return 1; }
    int listen_fd = -1;
    for (auto p = res; p; p = p->ai_next){
        listen_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listen_fd<0) continue;
        int opt=1; setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
        if (bind(listen_fd,p->ai_addr,p->ai_addrlen)==0) break;
        close(listen_fd); listen_fd=-1;
    }
    freeaddrinfo(res);
    if (listen_fd<0){ std::cerr<<"bind failed\n"; return 1; }
    if (listen(listen_fd, SOMAXCONN)==-1){ perror("listen"); return 1; }
    std::cout<<"Listening on port "<<port<<"\n";
    while (true){
        sockaddr_storage peer; socklen_t plen=sizeof(peer);
        int client = accept(listen_fd, (sockaddr*)&peer, &plen);
        if (client==-1){ perror("accept"); continue; }
        char buf[4096];
        ssize_t n;
        while ((n=recv(client,buf,sizeof(buf),0))>0){
            ssize_t s=0;
            while (s<n) { ssize_t w=send(client, buf+s, n-s, 0); if (w<=0) break; s+=w; }
        }
        close(client);
    }
    close(listen_fd);
}
```

client.cpp

```cpp
// echo_client.cpp
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <string>

int main(int argc,char**argv){
    if (argc<3){ std::cerr<<"Usage: client host port\n"; return 1;}
    const char* host=argv[1]; const char* port=argv[2];
    addrinfo hints{},*res;
    hints.ai_family=AF_UNSPEC; hints.ai_socktype=SOCK_STREAM;
    if (int err=getaddrinfo(host,port,&hints,&res); err){ std::cerr<<gai_strerror(err)<<"\n"; return 1;}
    int fd=-1;
    for (auto p=res;p;p=p->ai_next){
        fd=socket(p->ai_family,p->ai_socktype,p->ai_protocol);
        if (fd<0) continue;
        if (connect(fd,p->ai_addr,p->ai_addrlen)==0) break;
        close(fd); fd=-1;
    }
    freeaddrinfo(res);
    if (fd<0){ std::cerr<<"connect failed\n"; return 1;}
    std::string line;
    while (std::getline(std::cin,line)){
        line.push_back('\n');
        send(fd,line.data(),line.size(),0);
        char buf[4096];
        ssize_t n=recv(fd,buf,sizeof(buf)-1,0);
        if (n<=0) break;
        buf[n]=0; std::cout<<buf;
    }
    close(fd);
}
```

CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.10)
project(echo_posix)
set(CMAKE_CXX_STANDARD 20)
add_executable(echo_server echo_server.cpp)
add_executable(echo_client echo_client.cpp)
```

构建运行：

```
mkdir build && cd build
cmake ..
cmake --build .
./echo_server 5555
# 在另一个终端
./echo_client 127.0.0.1 5555
```

（Windows：需将 socket close -> closesocket，调用 WSAStartup）

练习题

- 把 echo server 改造成 line-based（按 \n 分割）协议
- 改造为处理多个 client（每个连接 fork / thread / select）

小项目（里程碑）

- 期中目标：实现一个命令行聊天室（支持多个 client，简单命令如 /nick）
预计时长：1–2 周

推荐资源

- Stevens《UNIX 网络编程》第一卷（套接字接口）

---

## 4. 线程模型与并发

学习目标

- 掌握常见并发模型：thread-per-connection、thread pool、reactor + worker、IOCP（Windows）
- 理解同步原语：mutex、condition_variable、atomic、lock-free 数据结构

核心概念

- 数据竞争、内存顺序、锁的粒度、死锁与活锁、线程池设计

可运行示例：简单线程池（非网络依赖，可用于处理 socket）

thread_pool.cpp

```cpp
// thread_pool.cpp
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <iostream>

class ThreadPool {
public:
    ThreadPool(size_t n = std::thread::hardware_concurrency()){
        stop=false;
        for (size_t i=0;i<n;++i) workers.emplace_back([this]{ worker(); });
    }
    ~ThreadPool(){
        {
            std::unique_lock lk(m);
            stop=true; cv.notify_all();
        }
        for (auto &w: workers) w.join();
    }
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args){
        using R = std::invoke_result_t<F, Args...>;
        auto task = std::make_shared<std::packaged_task<R()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        auto fut = task->get_future();
        {
            std::unique_lock lk(m);
            q.emplace([task]{ (*task)(); });
            cv.notify_one();
        }
        return fut;
    }
private:
    void worker(){
        while (true){
            std::function<void()> job;
            {
                std::unique_lock lk(m);
                cv.wait(lk, [this]{ return stop || !q.empty(); });
                if (stop && q.empty()) return;
                job = std::move(q.front()); q.pop();
            }
            job();
        }
    }
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> q;
    std::mutex m; std::condition_variable cv; bool stop;
};

int main(){
    ThreadPool tp(4);
    auto f = tp.submit([](int x){ return x*x; }, 8);
    std::cout<<"8*8="<<f.get()<<"\n";
    // 模拟任务
    for (int i=0;i<8;++i) tp.submit([i]{ std::cout<<"task "<<i<<" done\n"; std::this_thread::sleep_for(std::chrono::milliseconds(100)); });
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
```

练习题

- 使用线程池将 echo_server 的每个 accepted socket 分配到任务中处理
- 实现一个可伸缩的线程池（动态扩容）

小项目（里程碑）

- 实现一个连接数受限的聊天室：固定大小线程池＋socket accept queue
预计时长：1 周

推荐资源

- Anthony Williams《C++ Concurrency in Action》

---

## 5. 异步模型与 IO 复用（reactor/proactor、epoll/kqueue/IOCP、io_uring）

学习目标

- 理解事件驱动（reactor）与完成驱动（proactor）模型
- 掌握 epoll/kqueue/IOCP 基本用法，了解 io_uring（Linux 新接口）

核心概念

- 边触发（edge）vs 水平触发（level）、准备就绪 vs 完成事件、非阻塞 IO、事件循环设计

可运行示例：使用 standalone Asio 的 C++20 协程异步 echo server（现代建议）

async_echo.cpp

```cpp
// async_echo.cpp — requires standalone Asio, C++20
#include <asio.hpp>
#include <asio/awaitable.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/ip/tcp.hpp>
#include <iostream>

using asio::ip::tcp;
using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;

awaitable<void> session(tcp::socket sock){
    try{
        char buf[1024];
        for(;;){
            std::size_t n = co_await sock.async_read_some(asio::buffer(buf), use_awaitable);
            co_await asio::async_write(sock, asio::buffer(buf, n), use_awaitable);
        }
    } catch(const std::exception& e){
        std::cerr<<"session error: "<<e.what()<<"\n";
    }
}

awaitable<void> listener(asio::io_context& ctx, unsigned short port){
    tcp::acceptor acceptor(ctx, tcp::endpoint(tcp::v4(), port));
    for(;;){
        tcp::socket sock = co_await acceptor.async_accept(use_awaitable);
        co_spawn(ctx, session(std::move(sock)), detached);
    }
}

int main(){
    try{
        asio::io_context ctx(1);
        unsigned short port = 5555;
        co_spawn(ctx, listener(ctx, port), detached);
        ctx.run();
    } catch(const std::exception& e){
        std::cerr<<"fatal: "<<e.what()<<"\n";
    }
}
```

CMakeLists.txt（FetchContent 拉取 Asio header）

```cmake
cmake_minimum_required(VERSION 3.14)
project(async_echo CXX)
set(CMAKE_CXX_STANDARD 20)
include(FetchContent)
FetchContent_Declare(
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG asio-1-28-1 # 可换成最新稳定 tag
)
FetchContent_MakeAvailable(asio)
add_executable(async_echo async_echo.cpp)
target_compile_definitions(async_echo PRIVATE ASIO_STANDALONE)
target_include_directories(async_echo PRIVATE ${asio_SOURCE_DIR}/asio/include)
if(UNIX)
  target_link_libraries(async_echo PRIVATE pthread)
endif()
```

构建运行：

```
mkdir build && cd build
cmake ..
cmake --build .
./async_echo
# 用 telnet 或 nc 连接： nc 127.0.0.1 5555
```

练习题

- 将 epoll/kqueue 原生示例实现一个最小事件循环（accept/read/write）
- 比较线程池模型与异步事件循环在高并发下的差异（基准）

小项目（里程碑）

- 使用 Asio 实现一个支持 10k 并发连接的简易 echo server，并做基准（wrk/ab 风格）
预计时长：2–3 周（含调优）

推荐资源

- Asio 文档、libuv、Linux epoll man page、Windows IOCP 文档、io_uring 文档与 blog

---

## 6. C++20 特性在网络中的应用（coroutines、moves、concepts、ranges）

学习目标

- 在网络代码中使用 coroutines 简化异步流程、用 move 语义避免拷贝、用 concepts 限制接口、用 ranges 简化流/缓冲操作

核心概念

- co_await/co_return/co_yield、std::span、std::byte、移动语义在 buffer 管理中重要性、概念（concepts）用于类型约束

可运行示例：C++20 概念 + std::span 的“发送抽象”

buffer_demo.cpp

```cpp
// buffer_demo.cpp
#include <concepts>
#include <span>
#include <vector>
#include <iostream>
#include <cstddef>

template<typename T>
concept ByteContainer = std::contiguous_iterator<typename std::ranges::iterator_t<T>>; // 简单示例（示意）

void send_bytes(std::span<const std::byte> data){
    // 在真实代码中，这里会调用 send/async_write
    std::cout<<"sending "<<data.size()<<" bytes\n";
}

int main(){
    std::vector<std::byte> v{std::byte{0x1}, std::byte{0x2}, std::byte{0x3}};
    send_bytes(std::span<const std::byte>(v.data(), v.size()));
    return 0;
}
```

补充：协程与 Asio 示例见上一章（async_echo），这是协程在网络中最常见的实际应用。

练习题

- 用 concepts 约束一个“可写缓冲”接口
- 在 echo server 中用 std::span 替换裸指针缓冲

小项目（里程碑）

- 将项目中热门的回调式 API 重写为 coroutine 风格
预计时长：1–2 周

推荐资源

- C++20 标准草案、cppreference、“C++ Coroutines: Understanding the Basics”文章

---

## 7. 常用库与生态

学习目标

- 熟悉主流网络生态：Boost.Asio/standalone Asio、Boost.Beast（HTTP/WebSocket）、OpenSSL、gRPC/Protobuf、nlohmann/json、spdlog、fmt、vcpkg/Conan

核心概念

- 何时使用底层 sockets，何时使用高阶库（节省开发成本、易维护）

可运行示例：nlohmann/json 使用（header-only）

json_demo.cpp

```cpp
#include <nlohmann/json.hpp>
#include <iostream>
using json = nlohmann::json;
int main(){
    json j = { {"name","Alice"}, {"age",30}, {"skills", {"C++","networking"}} };
    std::cout<<j.dump(2)<<"\n";
}
```

CMake（FetchContent）

```cmake
cmake_minimum_required(VERSION 3.14)
project(json_demo CXX)
set(CMAKE_CXX_STANDARD 20)
include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp)
# 或 FetchContent 拉取整个 repo; 这里只示意
add_executable(json_demo json_demo.cpp)
target_include_directories(json_demo PRIVATE ${json_SOURCE_DIR})
```

练习题

- 使用 nlohmann/json 把聊天消息序列化为 JSON 并通过 socket 发送
- 用 Boost.Beast 写一个最小 HTTP GET 客户端（参考官方示例）

小项目（里程碑）

- 用 Boost.Beast 实现一个支持 WebSocket 的聊天室（处理升级/心跳/广播）
预计时长：2–4 周（学习 Beast + 实现）

推荐资源

- Boost.Beast / Boost.Asio 示例、gRPC 官方教程、OpenSSL 手册、vcpkg/Conan 使用文档

---

## 8. 协议与实践（HTTP/1.1/2/3、WebSocket、QUIC、TLS、gRPC）

学习目标

- 理解 HTTP 1.1/2/3 核心差异、WebSocket 协议、QUIC 的设计目标、TLS 握手基本步骤、gRPC 的工作流

核心概念

- HTTP/1.1 文本协议，HTTP/2 二进制流与多路复用，HTTP/3 基于 QUIC（UDP），WebSocket 双向全双工，TLS 建立安全信道

可运行示例：简单的 HTTP GET（原始 socket）

http_client.cpp

```cpp
// http_client.cpp (POSIX)
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

int main(){
    const char* host = "example.com";
    const char* port = "80";
    addrinfo hints{},*res;
    hints.ai_family=AF_UNSPEC; hints.ai_socktype=SOCK_STREAM;
    if (getaddrinfo(host, port, &hints, &res)) return 1;
    int fd=-1;
    for (auto p=res;p;p=p->ai_next){
        fd=socket(p->ai_family,p->ai_socktype,p->ai_protocol);
        if (fd<0) continue;
        if (connect(fd,p->ai_addr,p->ai_addrlen)==0) break;
        close(fd); fd=-1;
    }
    freeaddrinfo(res);
    if (fd<0) return 1;
    const char* req = "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
    send(fd, req, strlen(req), 0);
    char buf[4096];
    ssize_t n;
    while ((n=recv(fd,buf,sizeof(buf)-1,0))>0){
        buf[n]=0; std::cout<<buf;
    }
    close(fd);
    return 0;
}
```

TLS（OpenSSL）示例：见第11章（安全）中的 OpenSSL 客户端示例（附 CMake find_package(OpenSSL)）

gRPC：
- gRPC 需要安装 protoc、gRPC C++，示例包括 proto 文件、生成步骤和 CMake 配置（复杂，推荐使用 vcpkg 或官方 docker 镜像快速上手）。

练习题

- 用 raw socket 发起 HTTP 请求并解析响应头（只解析状态码）
- 使用 Boost.Beast 实现一个简单 HTTP 服务端

小项目（里程碑）

- 使用 Boost.Beast 做一个静态文件服务器（支持 keep-alive、简单日志）
预计时长：2–4 周

推荐资源

- RFC 文档、nghttp2、quiche、msquic、gRPC 官方示例

---

## 9. 性能优化（zero-copy、缓冲、池化、批处理）

学习目标

- 了解零拷贝技术（sendfile、mmap、splice、vmsplice）、缓冲策略、对象池、批处理（sendmmsg/recvmmsg）

核心概念

- 内存分配开销、避免内存碎片、减少 syscalls、缓存局部性、批量 IO

示例：使用 writev（scatter/gather）将多个 buffer 聚合写入（POSIX）

writev_demo.cpp

```cpp
#include <sys/uio.h>
#include <unistd.h>
#include <iostream>
#include <string>

int main(){
    std::string a="Hello ", b="world\n";
    iovec iov[2];
    iov[0].iov_base = (void*)a.data(); iov[0].iov_len = a.size();
    iov[1].iov_base = (void*)b.data(); iov[1].iov_len = b.size();
    ssize_t n = writev(STDOUT_FILENO, iov, 2);
    std::cout<<"writev wrote "<<n<<" bytes\n";
}
```

调优实践（建议）

- 用 perf / flamegraph 定位 CPU/memory hotspots
- TCP tuning：调整 send/recv buffer、tcp_tw_reuse、拥塞算法（TCP BBR）
- 使用批处理 syscall（sendmmsg/recvmmsg）减少上下文切换

练习题

- 将文件传输程序改为使用 sendfile（Linux）并测量吞吐量差异
- 实现基本对象池（连接上下文缓冲池）并比较分配开销

小项目（里程碑）

- 高吞吐量文件服务器：支持 sendfile、keep-alive、限速、基于 epoll 的事件循环
预计时长：3–4 周（含测试与调优）

推荐资源

- Brendan Gregg 的性能分析文章、Linux man page（sendfile、writev、splice、perf）

---

## 10. 调试与测试（ASAN/TSAN、profilers、benchmarks、CI）

学习目标

- 学会使用 AddressSanitizer/ThreadSanitizer/UBSan、perf/CPU profiler、heap profiler、benchmark 工具
- 在 CI 中加入测试、静态分析与性能基准

实践命令

- AddressSanitizer:
  - cmake: add_compile_options(-fsanitize=address -fno-omit-frame-pointer) and link flags
- ThreadSanitizer:
  - add_compile_options(-fsanitize=thread)
- gprof / perf: 运行 `perf record -g ./app`，`perf report`
- Google Benchmark：编写 microbenchmark 并加入 CI

示例：用 ASAN 编译

CMakeLists.txt

```cmake
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
```

练习题

- 在聊天室项目中加入单元测试（Catch2）与一个简单的基准（测量消息广播延迟）
- 用 ASAN 找到并修复内存错误

小项目（里程碑）

- 为项目配置 GitHub Actions：build、unit tests、ASAN、benchmark（可选）
预计时长：1 周（CI 基础）+ 测试覆盖持续投入

推荐资源

- Google Benchmark、Catch2、GitHub Actions 文档

---

## 11. 安全实践（证书、TLS、鉴权、常见漏洞）

学习目标

- 理解 TLS 握手、证书验证、密钥管理、常见错误（如未验证证书、TLS 版本弱化）

关键点

- 永远验证服务器证书（不要跳过验证除非在测试）
- 使用现代 TLS 版本（TLS 1.2+，建议 TLS1.3）
- 使用安全的证书颁发链、短生命周期证书、自动化证书更新（ACME/Let's Encrypt）

可运行示例：OpenSSL 简易 TLS 客户端（示意，需安装 OpenSSL）

openssl_client.cpp（简化）

```cpp
// openssl_client.cpp (简化示意；需 -lssl -lcrypto)
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

int main(int argc,char**argv){
    if (argc<3){ std::cerr<<"usage: host port\n"; return 1;}
    const char* host=argv[1]; const char* port=argv[2];
    SSL_library_init(); OpenSSL_add_all_algorithms(); SSL_load_error_strings();
    const SSL_METHOD* method = TLS_client_method();
    SSL_CTX* ctx = SSL_CTX_new(method);
    // 可添加证书验证设置：SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    addrinfo hints{}; hints.ai_socktype=SOCK_STREAM;
    addrinfo* res; getaddrinfo(host,port,&hints,&res);
    int sock=-1; for (auto p=res;p;p=p->ai_next){
        sock=socket(p->ai_family,p->ai_socktype,p->ai_protocol);
        if (sock<0) continue;
        if (connect(sock,p->ai_addr,p->ai_addrlen)==0) break;
        close(sock); sock=-1;
    }
    if (sock<0) { std::cerr<<"connect failed\n"; return 1; }
    SSL* ssl = SSL_new(ctx); SSL_set_fd(ssl,sock);
    if (SSL_connect(ssl) != 1){ ERR_print_errors_fp(stderr); return 1; }
    std::cout<<"Connected with "<<SSL_get_cipher(ssl)<<"\n";
    const char* req="GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
    SSL_write(ssl, req, strlen(req));
    char buf[4096]; int n;
    while ((n=SSL_read(ssl, buf, sizeof(buf)-1))>0){ buf[n]=0; std::cout<<buf; }
    SSL_shutdown(ssl); SSL_free(ssl); close(sock); SSL_CTX_free(ctx);
}
```

CMakeLists.txt

```cmake
find_package(OpenSSL REQUIRED)
add_executable(openssl_client openssl_client.cpp)
target_link_libraries(openssl_client PRIVATE OpenSSL::SSL OpenSSL::Crypto)
```

练习题

- 使用 OpenSSL 验证服务器证书并打印证书链信息
- 在 gRPC 中启用 TLS（服务端/客户端）

小项目（里程碑）

- 为现有 HTTP 服务器集成 TLS（自动化证书更新）
预计时长：1–2 周（含证书管理）

推荐资源

- OpenSSL 文档、Let's Encrypt、OWASP 网络安全指南

---

## 12. 项目清单（从易到难）与 3/6 个月学习计划

实战项目（难度自低到高）

1. TCP Echo（入门） — 本指南第一、二章项目（熟悉 socket）  
2. 命令行聊天室（中） — multi-client、昵称、广播、/nick、/quit  
3. 基于 HTTP 的静态文件服务器（中） — 支持 keep-alive、日志  
4. WebSocket 聊天室（中偏上） — 用 Boost.Beast 或 websocket++  
5. 简易代理（中偏上） — 支持 HTTP 代理与缓存（LRU）  
6. gRPC 微服务（上） — 定义 proto，生成代码，实现 auth & TLS  
7. 高性能消息代理（上） — epoll/io_uring + 零拷贝 + 多线程 + 持久化  
8. HTTP/3 实验（高） — 集成 quiche / msquic，理解 QUIC 流模型

每项目给出里程碑（示例：聊天室）

- M1（第1周）：同步单线程实现 accept+broadcast 基本功能  
- M2（第2周）：加入线程池/异步 IO，提高并发  
- M3（第3周）：加入简单认证、JSON 消息格式、日志  
- M4（第4周）：编写测试、性能基准与部署脚本

3 个月计划（紧凑）

- 周 1-2：网络基础 + DNS/getaddrinfo，简单工具（第2章、练习）  
- 周 3-4：BSD sockets 同步服务器 + 客户端（第2章项目）  
- 周 5-6：线程模型与线程池，改造服务器以支持并发（第4章）  
- 周 7-8：异步 IO 与 Asio 协程示例，完成 async_echo（第5章）  
- 周 9-10：HTTP/Beast 或 raw HTTP 实践 + TLS 入门（第7/8章）  
- 周 11-12：选择一个中级项目（聊天室或静态服务器），完成 M1–M4

6 个月计划（稳健）

- 月 1：网络基础、POSIX sockets（含练习）  
- 月 2：并发模型、线程池、事件驱动（epoll/kqueue/IOCP）  
- 月 3：Asio、coroutines、Boost.Beast（HTTP/WS）  
- 月 4：TLS、OpenSSL、证书管理、gRPC 入门  
- 月 5：性能优化（zero-copy、sendfile、profiling）  
- 月 6：综合项目（选择高阶项目），撰写文档、测试、部署

每周时间建议：3–15 小时（依据目标），复习与实践并重

---

## 13. 常见学习陷阱与面试常问题目

学习陷阱（Avoid）

- 直接复制粘贴网络示例到生产（忽略错误处理与安全）  
- 在 high-concurrency 下使用 thread-per-connection（会 OOM）  
- 在 TLS 里跳过证书验证（危险）  
- 忽视性能基线测试与测量（盲目优化）

面试常问题（样例）

1. TCP 三次握手与四次挥手分别是什么？为什么需要？（简答）  
2. UDP 与 TCP 的区别？什么时候用 UDP？  
3. 描述 select/poll/epoll 的区别与适用场景  
4. 什么是零拷贝？给出 Linux 下实现零拷贝的 syscalls  
5. 描述 TCP 的拥塞控制与慢启动（slow start）  
6. 描述 TLS 握手基本流程与证书验证  
7. C++ 如何避免缓冲拷贝？（move, std::span, zero-copy）  
8. Coroutines 的基础原理是什么？如何用在 IO 上？  
9. 解释 Nagle 算法与 TCP_NODELAY 的作用  
10. 设计一个高并发聊天室，说明关键的数据结构与并发策略

（练习：把上述题目准备成口头回答与代码示例）

---

## 附录：推荐书目与在线资源

- 《TCP/IP 详解 卷1》 - W. Richard Stevens
- 《UNIX Network Programming》 - W. Richard Stevens
- 《C++ Concurrency in Action》 - Anthony Williams
- Asio / Boost.Asio 官方文档与示例
- Boost.Beast 示例仓库
- gRPC 官方文档与 tutorial
- Linux perf / Brendan Gregg 博客

---

结语

- 先做会再读书：每学一章都要有小项目实战。  
- 若需要，把上面任一章拆成详细周计划或把项目做成具体 issue 列表，我可以帮你生成更细化的任务分解、代码骨架与 CI 配置。
