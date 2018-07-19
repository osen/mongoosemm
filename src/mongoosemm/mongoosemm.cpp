#include "mongoosemm.h"

//#include "mongoose.h"

namespace mongoosemm
{

void ConnectionHandler::onWebsocketHandshakeDone(Connection& conn) { }
void ConnectionHandler::onWebsocketFrame(Connection& conn, WebsocketFrame& frame) { }
void ConnectionHandler::onHttpRequest(Connection& conn, HttpMessage& message) { }
void ConnectionHandler::onClose(Connection& conn) { }
ConnectionHandler::~ConnectionHandler() { }
void ConnectionHandler::dummyDeleter(ConnectionHandler *ptr) { }

ConnectionHandler::ConnectionHandler()
{
  self.reset(this, dummyDeleter);
}

struct ConnectionImpl
{
  std::weak_ptr<ConnectionHandler> handler;

};

void Connection::setProtocolHttpWebsocket()
{

}

struct ManagerImpl
{
/*
  static void ev_handler(mg_connection *nc, int ev, void *ev_data)
  {

  }
*/
  std::vector<Connection> connections;
};

void Manager::init()
{
  impl = std::make_shared<ManagerImpl>();
}

Connection Manager::bind(int port, ConnectionHandler& handler)
{
  Connection rtn;

  rtn.sImpl = std::make_shared<ConnectionImpl>();
  rtn.wImpl = rtn.sImpl;

  rtn.sImpl->handler = handler.self;

  // Do actual mg_bind

  impl->connections.push_back(rtn);

  rtn.sImpl.reset();

  return rtn;
}

void Manager::poll(int milliseconds)
{

}

}
