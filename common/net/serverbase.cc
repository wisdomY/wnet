#include "evpp/logging.h"
#include "evpp/tcp_conn.h"

#include "serverbase.h"
#include "serverconn.h"

ServerBase::ServerBase(evpp::EventLoop* loop,
                       IPacketStreamer* streamer,
                       IServerAdapter* adapter,
                       const std::string& listen_addr/*ip:port*/,
                       const std::string& name,
                       uint32_t thread_num)
    : loop_(loop)
    , streamer_(streamer)
    , adapter_(adapter)
    , tcp_server_(loop, listen_addr, name, thread_num)
    , stopped_(false)
{
    tcp_server_.SetConnectionCallback(
        std::bind(&ServerBase::OnConnection, this, std::placeholders::_1));
    tcp_server_.SetMessageCallback(
	std::bind(&ServerBase::OnMessage, this, std::placeholders::_1, std::placeholders::_2));
}

ServerBase::~ServerBase() {
}

void ServerBase::Start() {
    tcp_server_.Init();
    tcp_server_.Start();
    loop_->Run();
}

void ServerBase::Stop() {
    tcp_server_.Stop([this]() { stopped_ = true; stopCond_.notify_one(); });
    std::unique_lock<std::mutex> _lk(stopMutex_);
    stopCond_.wait(_lk, [this] { return stopped_; });
}

void ServerBase::OnConnection(const evpp::TCPConnPtr& conn) {
    if (conn->IsConnected()) {
        DLOG_TRACE << "New Server Connection fd:" << conn->fd() << " id:" << conn->id();
        conn->SetTCPNoDelay(true);
        ServerConnectionPtr sc(new ServerConnection(conn.get(), streamer_, adapter_));
        conn->set_context(evpp::Any(sc));
    } else {
        DLOG_TRACE << "Server Connection lost id:" << conn->id();
        conn->set_context(evpp::Any());
    }
}

void ServerBase::OnMessage(const evpp::TCPConnPtr& conn, evpp::Buffer* input) {
    const evpp::Any& ctx = conn->context();
    if (ctx.IsEmpty()) {
        LOG_ERROR << "The evpp::TCPConn is not assoiated with a Server Connection";
        return;
    }

    ServerConnectionPtr sc = ctx.Get<ServerConnectionPtr>();
    sc->OnMessage(conn, input);
}
