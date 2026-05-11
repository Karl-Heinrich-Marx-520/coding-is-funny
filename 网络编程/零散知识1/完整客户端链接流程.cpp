#include <boost/asio.hpp>
#include <iostream>

using namespace boost::asio;
using namespace boost::asio::ip;

// 你的endpoint函数：构造目标地址
tcp::endpoint create_endpoint(const std::string& ip, unsigned short port) {
    boost::system::error_code ec;
    address addr = make_address(ip, ec);
    if (ec) {
        std::cout << "IP解析失败: " << ec.message() << std::endl;
        return tcp::endpoint();
    }
    return tcp::endpoint(addr, port);
}

// 修改后的socket函数：创建并打开socket
int create_tcp_socket(tcp::socket& sock) {
    boost::system::error_code ec;
    sock.open(tcp::v4(), ec);
    if (ec) {
        std::cout << "Socket打开失败: " << ec.message() << std::endl;
        return ec.value();
    }
    return 0;
}

int main() {
    // 1. 创建io_context（生命周期必须覆盖所有socket操作）
    io_context ioc;

    // 2. 构造目标地址（endpoint）
    auto ep = create_endpoint("127.0.0.1", 3333);

    // 3. 创建并打开socket
    tcp::socket sock(ioc);
    if (create_tcp_socket(sock) != 0) {
        return 1;
    }

    // 4. 关键步骤：socket拿着endpoint去连接服务器
    boost::system::error_code ec;
    sock.connect(ep, ec);
    if (ec) {
        std::cout << "连接服务器失败: " << ec.message() << std::endl;
        return 1;
    }

    std::cout << "连接成功！" << std::endl;

    // 后续可以用sock.send()/sock.receive()收发数据...

    sock.close();
    return 0;
}
