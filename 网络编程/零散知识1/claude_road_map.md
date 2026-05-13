# C++20 网络编程学习路线

> 目标：掌握基于 C++20 协程的现代异步网络编程，能够以 Asio 为核心构建高性能服务端/客户端。
> 前提：已具备 C++基础语法和面向对象知识。

---

## 第一阶段：网络基础（独立于语言）

**目标**：理解数据如何在网络中流动，建立"包、连接、状态"的直觉。

| 主题 | 关键概念 | 检验标准 |
|------|---------|---------|
| TCP/IP 分层 | 链路层 → 网络层 → 传输层 → 应用层 | 能说出每一层的作用和代表协议 |
| TCP 核心机制 | 三次握手、四次挥手、滑动窗口、拥塞控制、Nagle 算法 | 能画出状态转换图 |
| Socket 编程模型 | `socket` / `bind` / `listen` / `accept` / `connect` / `send` / `recv` | 能用 C 语言写出阻塞式 Echo Server |
| IO 模型 | 阻塞 IO、非阻塞 IO、IO 多路复用 (`select`/`poll`/`epoll`/`io_uring`) | 理解为什么需要 Reactor/Proactor |
| 协议基础 | 自定义二进制协议 vs 文本协议、粘包/拆包、Length-Prefixed | 能设计一个简单的应用层协议 |

**推荐实践**：
- 用纯 C 写一个阻塞式 Echo Server（单线程 + 多线程版本）。
- 用 `epoll` 写一个简单的 Reactor 模式 Server。

**现代 C++ 关联**：这一阶段让你理解 **Asio 底层在做什么**。当你写 `co_await socket.async_read_some()` 时，要知道它最终调用的还是操作系统提供的异步 IO 接口。

---

## 第二阶段：C++20 现代特性速成（网络编程视角）

**目标**：掌握 Asio 协程编程所需的 C++20 核心武器。

### 1. 协程 (Coroutines)
这是现代 C++ 网络编程的**灵魂**。

```cpp
#include <coroutine>
#include <iostream>

// 简化版：理解 co_await / co_return 的语义
std::task<int> async_add(int a, int b) {
    co_return a + b;  // 挂起 → 返回 → 恢复
}
```

**必须掌握**：
- `co_await`：等待一个 Awaitable 对象完成，交出执行权。
- `co_return`：协程返回值。
- `co_yield`：生成器模式（网络编程中较少用，但需认识）。
- 协程的生命周期：何时构造、何时挂起、何时销毁（避免悬空引用！）。

### 2. Concept（概念）
Asio 大量使用模板约束。

```cpp
template<typename T>
concept Awaitable = requires(T t) {
    { t.await_ready() } -> std::convertible_to<bool>;
    { t.await_suspend() };
    { t.await_resume() };
};
```

### 3. Module（模块）
替代 `#include`，加速编译。现代项目推荐逐步迁移。

```cpp
// math.cppm
export module math;
export int add(int a, int b) { return a + b; }
```

### 4. Ranges（范围库）
处理缓冲区、数据包解析时非常有用。

```cpp
std::vector<int> data{1, 2, 3, 4, 5};
auto result = data | std::views::filter([](int x){ return x > 2; });
```

**推荐练习**：
- 手写一个简化版的 `std::task<T>` 协程类型。
- 理解 `std::coroutine_traits` 和 `promise_type` 的作用。

---

## 第三阶段：Asio 核心机制

**目标**：掌握 Asio 的设计哲学和核心组件。

### 1. 执行上下文 (Execution Context)
```cpp
asio::io_context ctx;  // 事件循环核心
```
- `io_context::run()`：阻塞当前线程，处理事件队列。
- `io_context::poll()`：非阻塞地处理已就绪事件。
- `io_context::stop()` / `restart()`：控制生命周期。

### 2. 执行器 (Executor)
```cpp
auto exec = ctx.get_executor();
```
- 决定回调/协程在哪个上下文执行。
- 多线程下通常配合 `asio::strand` 保证顺序执行。

### 3. IO 对象
| 对象 | 用途 |
|------|------|
| `asio::ip::tcp::socket` | TCP 套接字 |
| `asio::ip::tcp::acceptor` | 监听连接 |
| `asio::ip::udp::socket` | UDP 套接字 |
| `asio::steady_timer` | 定时器 |
| `asio::ip::tcp::resolver` | 域名解析 |

### 4. 异步操作模型
Asio 的异步操作有三种风格：

**A. 回调风格（传统，不推荐新代码使用）**
```cpp
socket.async_read_some(buffer, [](error_code ec, size_t n) {
    // 处理结果... 容易陷入回调地狱
});
```

**B. Future 风格（较少用）**
```cpp
auto fut = socket.async_read_some(buffer, asio::use_future);
```

**C. 协程风格（C++20 推荐）**
```cpp
auto [ec, n] = co_await socket.async_read_some(buffer, asio::as_tuple(asio::use_awaitable));
```

**重点掌握**：`asio::use_awaitable`、`asio::as_tuple`、`asio::deferred`。

### 5. 缓冲区 (Buffer)
```cpp
std::vector<char> data(1024);
asio::mutable_buffer buf = asio::buffer(data);
```
- `asio::buffer()` 不拥有内存，仅封装指针+长度。
- 需要确保异步操作期间底层内存有效。

**推荐实践**：
- 用协程风格重写第二阶段的 Echo Server。
- 实现一个带超时机制的 `async_read_with_timeout`。

---

## 第四阶段：协程网络编程实战

**目标**：用 C++20 协程写出清晰、可维护的异步网络代码。

### 1. 基础 Echo Server（协程版）
```cpp
#include <asio.hpp>
#include <iostream>

namespace this_coro = asio::this_coro;
using asio::ip::tcp;
using asio::co_spawn;
using asio::awaitable;

awaitable<void> session(tcp::socket socket) {
    try {
        char data[1024];
        while (true) {
            std::size_t n = co_await socket.async_read_some(
                asio::buffer(data), this_coro::token());
            co_await asio::async_write(socket, asio::buffer(data, n), this_coro::token());
        }
    } catch (std::exception& e) {
        std::cerr << "session error: " << e.what() << "\n";
    }
}

awaitable<void> listener(unsigned short port) {
    auto executor = co_await this_coro::executor;
    tcp::acceptor acceptor(executor, {tcp::v4(), port});

    while (true) {
        tcp::socket socket = co_await acceptor.async_accept(this_coro::token());
        co_spawn(executor, session(std::move(socket)), asio::detached);
    }
}

int main() {
    try {
        asio::io_context io_context(1);
        co_spawn(io_context, listener(55555), asio::detached);
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}
```

### 2. 错误处理策略
- **异常模式**：简单，但异常路径开销大。
- `asio::as_tuple` 模式：显式处理 `error_code`，服务端代码首选。

```cpp
auto [ec, bytes_written] = co_await asio::async_write(
    socket, asio::buffer(data), asio::as_tuple(asio::use_awaitable));

if (ec) {
    // 优雅处理：关闭连接、记录日志
    co_return;
}
```

### 3. 并发模型选择
| 模型 | 适用场景 | 代码特征 |
|------|---------|---------|
| 单线程 `io_context` | 逻辑简单、无锁需求 | `io_context.run()` |
| 线程池 `io_context` | CPU 密集型处理 | `vector<jthread>` 都跑 `run()` |
| 每连接一线程 | 遗留系统兼容 | 较少用 |
| `io_uring` (Linux) | 超高性能 | Asio 已支持 `io_uring` 后端 |

### 4. 定时器与超时
```cpp
awaitable<void> timeout_operation(tcp::socket& socket) {
    asio::steady_timer timer(co_await this_coro::executor);
    timer.expires_after(std::chrono::seconds(5));

    auto [order_ec, timer_ec, n] = co_await asio::experimental::make_parallel_group(
        socket.async_read_some(asio::buffer(data), asio::deferred),
        timer.async_wait(asio::deferred)
    ).async_wait(asio::experimental::wait_for_one(), asio::use_awaitable);

    if (timer_ec == asio::error::operation_aborted) {
        // IO 先完成
    } else {
        // 超时了，需要 cancel socket
        socket.cancel();
    }
}
```

**推荐实践**：
- 实现一个支持并发连接的 Chat Server（广播消息）。
- 实现一个带心跳保活和重连机制的 TCP Client。

---

## 第五阶段：协议与序列化

**目标**：让网络程序能传输结构化数据。

### 1. 自定义二进制协议
```
[4 bytes: magic] [4 bytes: length] [N bytes: payload] [4 bytes: crc32]
```
- 使用 `asio::async_read` 做固定长度头读取（`asio::transfer_exactly`）。
- 使用 `std::endian`（C++20）处理字节序。

```cpp
#include <bit>

uint32_t host_to_net(uint32_t val) {
    if constexpr (std::endian::native == std::endian::little)
        return std::byteswap(val);  // C++23, C++20 可用自定义实现
    return val;
}
```

### 2. 文本协议
- **JSON**：`nlohmann/json` 库，适合配置、Web 交互。
- **MessagePack**：二进制 JSON，高效紧凑。

### 3. Schema 化协议（生产环境推荐）
- **Protobuf**：跨语言、版本兼容、性能优秀。
- **FlatBuffers**：零拷贝反序列化，游戏/实时系统常用。

**推荐实践**：
- 用 Protobuf 重写 Chat Server 的通信协议。
- 实现一个 Length-Prefixed 的编解码器（解决粘包问题）。

---

## 第六阶段：HTTP / WebSocket

**目标**：具备与 Web 生态交互的能力。

### 1. Beast（Asio 的官方 HTTP 扩展）
```cpp
#include <boost/beast.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

awaitable<void> http_server(tcp::socket socket) {
    beast::flat_buffer buffer;
    http::request<http::string_body> req;
    co_await http::async_read(socket, buffer, req, this_coro::token());

    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set(http::field::content_type, "text/plain");
    res.body() = "Hello, C++20!";
    res.prepare_payload();

    co_await http::async_write(socket, res, this_coro::token());
}
```

### 2. WebSocket
```cpp
#include <boost/beast/websocket.hpp>

namespace websocket = beast::websocket;

awaitable<void> websocket_session(tcp::socket socket) {
    websocket::stream<tcp::socket> ws(std::move(socket));
    co_await ws.async_accept(this_coro::token());

    while (ws.is_open()) {
        beast::flat_buffer buffer;
        co_await ws.async_read(buffer, this_coro::token());
        ws.text(ws.got_text());
        co_await ws.async_write(buffer.data(), this_coro::token());
    }
}
```

### 3. REST API Client
使用 Beast 构造 HTTP 请求，配合 `nlohmann/json` 处理响应。

**推荐实践**：
- 实现一个简易 HTTP 静态文件服务器（支持 Range 请求）。
- 实现一个 WebSocket 聊天室前端 + C++ 后端。

---

## 第七阶段：高级主题与生产化

### 1. SSL/TLS 加密通信
```cpp
#include <asio/ssl.hpp>

asio::ssl::context ctx(asio::ssl::context::tls_server);
ctx.use_certificate_chain_file("server.crt");
ctx.use_private_key_file("server.key", asio::ssl::context::pem);

asio::ssl::stream<tcp::socket> stream(std::move(socket), ctx);
co_await stream.async_handshake(asio::ssl::stream_base::server, this_coro::token());
```

### 2. 信号处理与优雅退出
```cpp
asio::signal_set signals(io_context, SIGINT, SIGTERM);
co_await signals.async_wait(this_coro::token());
io_context.stop();  // 触发所有 pending 操作的 cancellation
```

### 3. 性能调优
- **零拷贝**：`sendfile`（Linux）、内存池（`asio::streambuf` 自定义分配器）。
- **CPU 亲和性**：绑定 `io_context` 线程到特定核心。
- **连接数优化**：调整 `ulimit -n`、使用 `SO_REUSEPORT`。

### 4. 可观测性
- 集成 `spdlog` 做结构化日志。
- 暴露 Prometheus 指标（请求 QPS、延迟 P99、连接数）。

---

## 推荐学习资源

| 资源 | 说明 |
|------|------|
| [Asio 官方文档](https://think-async.com/Asio/) | 权威参考，重点看 C++20 Coroutines 部分 |
| [Boost.Beast 文档](https://www.boost.org/doc/libs/release/libs/beast/doc/html/index.html) | HTTP/WebSocket 必看 |
| *C++ Network Programming with Asio* | 书籍（如有中文译本） |
| [Asio C++20 Examples](https://github.com/chriskohlhoff/asio/tree/master/src/examples/cpp20) | 官方协程示例代码 |

---

## 学习路径总结图

```
网络基础 (TCP/IP, Socket C API)
        ↓
C++20 核心特性 (协程, Concept, Module)
        ↓
Asio 核心机制 (io_context, Executor, Buffer)
        ↓
协程网络编程 (awaitable, 错误处理, 并发模型)
        ↓
协议设计 (二进制协议, Protobuf, 粘包处理)
        ↓
应用层协议 (HTTP/WebSocket via Beast)
        ↓
生产化 (SSL, 性能调优, 可观测性)
```

**核心原则**：每一层都建立在前一层之上，最终形成**协议设计 → 协程编排 → Asio 调度 → OS 异步 IO** 的完整链路理解。
