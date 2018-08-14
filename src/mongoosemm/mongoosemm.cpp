#include "mongoosemm.h"

#include "mongoose.h"

#include <sstream>

namespace mongoosemm
{

void ConnectionHandler::onWebsocketHandshakeDone(Connection& conn) { }
void ConnectionHandler::onWebsocketFrame(Connection& conn, WebsocketFrame& frame) { }
void ConnectionHandler::onHttpRequest(Connection& conn, HttpMessage& message) { }
void ConnectionHandler::onClose(Connection& conn) { }
ConnectionHandler::~ConnectionHandler() { }

void dummyDeleter(ConnectionHandler *ptr) { }

ConnectionHandler::ConnectionHandler()
{
  self.reset(this, dummyDeleter);
}

struct ConnectionImpl
{
  std::weak_ptr<ConnectionHandler> handler;
  mg_connection *nc;

  ConnectionImpl()
  {
    nc = NULL;
  }

  ~ConnectionImpl()
  {
    Connection *c = (Connection *)nc->user_data;
    if(c) delete c;
  }
};

void Connection::setProtocolHttpWebsocket()
{
  mg_set_protocol_http_websocket(wImpl.lock()->nc);
}

struct ManagerImpl
{
  mg_mgr mgr;
  std::vector<Connection> connections;

  ManagerImpl()
  {
    mgr = {0};
  }

  ~ManagerImpl()
  {
    Manager *m = (Manager *)mgr.user_data;
    if(m) delete m;

    mg_mgr_free(&mgr);
  }

  static void ev_handler(mg_connection *nc, int ev, void *ev_data)
  {
    Connection *c = (Connection *)nc->user_data;

    if(!c)
    {
      c = new Connection();
      c->sImpl = std::make_shared<ConnectionImpl>();
      c->wImpl = c->sImpl;
      c->sImpl->nc = nc;
      nc->user_data = c;
    }

    Connection con = *c;
    con.sImpl.reset();

    if(c->wImpl.lock()->handler.expired())
    {
      return;
    }

    if(ev == MG_EV_HTTP_REQUEST)
    {
      HttpMessage httpMessage;
      c->wImpl.lock()->handler.lock()->onHttpRequest(con, httpMessage);
    }
    else
    {
      printf("Unhandled event %i\n", ev);
    }
  }
};

void Manager::init()
{
  impl = std::make_shared<ManagerImpl>();
  mg_mgr_init(&impl->mgr, NULL);
  impl->mgr.user_data = new Manager(*this);
}

Connection Manager::bind(int port, ConnectionHandler& handler)
{
  Connection rtn;

  rtn.sImpl = std::make_shared<ConnectionImpl>();
  rtn.wImpl = rtn.sImpl;

  rtn.sImpl->handler = handler.self;

  std::stringstream ss;
  ss << port;
  rtn.sImpl->nc = mg_bind(&impl->mgr, ss.str().c_str(), ManagerImpl::ev_handler);

  impl->connections.push_back(rtn);

  rtn.sImpl.reset();

  rtn.wImpl.lock()->nc->user_data = new Connection(rtn);

  return rtn;
}

void Manager::poll(int milliseconds)
{
  mg_mgr_poll(&impl->mgr, milliseconds);
}

}
