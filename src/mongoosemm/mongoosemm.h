#ifndef MONGOOSEMM_MONGOOSEMM_H
#define MONGOOSEMM_MONGOOSEMM_H

#include <memory>
#include <vector>
#include <string>

namespace mongoosemm
{

struct WebsocketFrameImpl;
struct HttpMessageImpl;
struct ConnectionImpl;
struct ManagerImpl;
class ConnectionHandler;
class Manager;
struct Connection;

struct ServeHttpOpts
{
  std::string documentRoot;
  bool enableDirectoryListing;
};

class HttpMessage
{
  friend class ManagerImpl;

  std::shared_ptr<HttpMessageImpl> sImpl;
  std::weak_ptr<HttpMessageImpl> wImpl;

public:
  void serveHttp(Connection& conn);
};

class WebsocketFrame
{
  std::shared_ptr<WebsocketFrameImpl> sImpl;
  std::weak_ptr<WebsocketFrameImpl> wImpl;

public:
  std::vector<unsigned char> getData();
  std::string getText();
};

struct Connection
{
  friend class mongoosemm::Manager;

  std::shared_ptr<ConnectionImpl> sImpl;
  std::weak_ptr<ConnectionImpl> wImpl;

  void setProtocolHttpWebsocket();
  void serveHttp(HttpMessage& message, ServeHttpOpts& opts);
  void sendWebsocketFrame(std::vector<unsigned char>& data);
  void sendWebsocketFrame(std::string& text);
};

class ConnectionHandler
{
  friend class mongoosemm::ManagerImpl;
  friend class mongoosemm::Manager;

  std::shared_ptr<ConnectionHandler> self;

  virtual void onWebsocketHandshakeDone(Connection& conn);
  virtual void onWebsocketFrame(Connection& conn, WebsocketFrame& frame);
  virtual void onHttpRequest(Connection& conn, HttpMessage& message);
  virtual void onClose(Connection& conn);

public:
  ConnectionHandler();
  virtual ~ConnectionHandler();

};

class Manager
{
  std::shared_ptr<ManagerImpl> sImpl;
  std::weak_ptr<ManagerImpl> wImpl;

public:
  void init();
  Connection bind(int port, ConnectionHandler& handler);
  void poll(int milliseconds);
};

}

#endif

