#include <mongoosemm/mongoosemm.h>

#include <iostream>

class ExampleHandler : public mongoosemm::ConnectionHandler
{
  void onWebsocketHandshakeDone(mongoosemm::Connection& conn)
  {
    std::cout << "Handshake Done" << std::endl;
  }

  void onWebsocketFrame(mongoosemm::Connection& conn, mongoosemm::WebsocketFrame& frame)
  {
    std::cout << "Websocket Frame" << std::endl;
  }

  void onHttpRequest(mongoosemm::Connection& conn, mongoosemm::HttpMessage& message)
  {
    std::cout << "Http Request" << std::endl;
  }

  void onClose(mongoosemm::Connection& conn)
  {
    std::cout << "Close" << std::endl;
  }
};

int main()
{
  mongoosemm::Manager mgr;
  ExampleHandler handler;

  mgr.init();
  mongoosemm::Connection nc = mgr.bind(8080, handler);

  nc.setProtocolHttpWebsocket();

  for(int i = 0; i < 3; i++)
  {
    mgr.poll(1000);
  }

  return 0;
}
