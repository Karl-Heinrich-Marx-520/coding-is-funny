#include <boost/asio.hpp>
#include <iostream>
#include <string>

using namespace boost::asio;
using namespace boost::asio::ip;

// 优化版：创建并初始化acceptor，带错误处理
int create_acceptor(io_context& ioc, tcp::acceptor& acceptor, unsigned short port) {
    boost::system::error_code ec;

    // 1. 打开监听套接字
    acceptor.open(tcp::v4(), ec);
    if (ec) {
        std::cout << "打开acceptor失败: " << ec.message() << std::endl;
        return ec.value();
    }

    // 2. 绑定到指定端口（监听本机所有IPv4地址）
    tcp::endpoint ep(tcp::v4(), port);
    acceptor.bind(ep, ec);
    if (ec) {
        std::cout << "绑定端口失败（端口可能被占用）: " << ec.message() << std::endl;
        return ec.value();
    }

    // 3. 开始监听，设置队列长度为10
    acceptor.listen(10, ec);
    if (ec) {
        std::cout << "开始监听失败: " << ec.message() << std::endl;
        return ec.value();
    }

    std::cout << "服务器已启动，监听端口: " << port << std::endl;
    return 0;
}

int main() {
    // 1. 创建io_context（生命周期最长）
    io_context ioc;

    // 2. 创建acceptor对象
    tcp::acceptor acceptor(ioc);
    if (create_acceptor(ioc, acceptor, 33333) != 0) {
        return 1;
    }

    // 3. 循环接受客户端连接
    while (true) {
        boost::system::error_code ec;
        // 4. 接受客户端连接，生成新的socket
        tcp::socket client_sock(ioc);
        acceptor.accept(client_sock, ec);

        if (ec) {
            std::cout << "接受客户端连接失败: " << ec.message() << std::endl;
            continue;
        }

        // 获取客户端地址，打印连接信息
        tcp::endpoint client_ep = client_sock.remote_endpoint();
        std::cout << "客户端连接成功: " << client_ep.address() << ":" << client_ep.port() << std::endl;

        // 5. 用新的socket和客户端通信（示例：接收客户端消息并回复）
        char buf[1024] = {0};
        client_sock.receive(buffer(buf), ec);
        if (!ec) {
            std::cout << "收到客户端消息: " << buf << std::endl;
            std::string reply = "Hello from server!";
            client_sock.send(buffer(reply), ec);
        }

        // 6. 关闭客户端socket
        client_sock.close();
        std::cout << "客户端断开连接" << std::endl;
    }

    return 0;
}
