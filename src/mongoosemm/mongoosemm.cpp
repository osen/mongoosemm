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

struct HttpMessageImpl
{
  http_message *message;
};

ConnectionHandler::ConnectionHandler()
{
  self.reset(this, dummyDeleter);
}

struct ConnectionImpl
{
  Manager mgr;
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
    Connection *cc = c;

    if(!c || c->wImpl.lock()->nc != nc)
    {
      c = new Connection();
      c->sImpl = std::make_shared<ConnectionImpl>();
      c->wImpl = c->sImpl;
      c->sImpl->nc = nc;
      c->sImpl->handler = cc->wImpl.lock()->handler;
      c->sImpl->mgr = cc->wImpl.lock()->mgr;
      nc->user_data = c;
      printf("Making\n");
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
      httpMessage.sImpl = std::make_shared<HttpMessageImpl>();
      httpMessage.wImpl = httpMessage.sImpl;
      httpMessage.sImpl->message = (http_message *)ev_data;
      HttpMessage m = httpMessage;
      m.sImpl.reset();
      c->wImpl.lock()->handler.lock()->onHttpRequest(con, m);
    }
    else if(ev == MG_EV_CLOSE)
    {
      c->wImpl.lock()->nc->user_data = NULL;
      delete c;
    }
    else
    {
      printf("Unhandled event %i\n", ev);
    }
  }
};

void Manager::init()
{
  sImpl = std::make_shared<ManagerImpl>();
  mg_mgr_init(&sImpl->mgr, NULL);
  sImpl->mgr.user_data = new Manager(*this);
  wImpl = sImpl;
}

Connection Manager::bind(int port, ConnectionHandler& handler)
{
  Connection rtn;

  rtn.sImpl = std::make_shared<ConnectionImpl>();
  rtn.wImpl = rtn.sImpl;

  rtn.sImpl->handler = handler.self;
  rtn.sImpl->mgr = *this;
  rtn.sImpl->mgr.sImpl.reset();

  std::stringstream ss;
  ss << port;
  rtn.sImpl->nc = mg_bind(&wImpl.lock()->mgr, ss.str().c_str(), ManagerImpl::ev_handler);

  wImpl.lock()->connections.push_back(rtn);

  rtn.sImpl.reset();

  rtn.wImpl.lock()->nc->user_data = new Connection(rtn);

  return rtn;
}

void Manager::poll(int milliseconds)
{
  mg_mgr_poll(&wImpl.lock()->mgr, milliseconds);
}

void HttpMessage::serveHttp(Connection& conn)
{
  mg_serve_http_opts opts = {0};
  opts.document_root = ".";
  opts.enable_directory_listing = "yes";
  mg_serve_http(conn.wImpl.lock()->nc, wImpl.lock()->message, opts);
}

}
