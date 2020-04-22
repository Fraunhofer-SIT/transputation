#pragma once

namespace transputation {
class Transport;
}

#include <netinet/in.h>
#include <strings.h>

#include "packet.h"

namespace transputation {
class Transport {
 public:
  virtual ~Transport() {}
  virtual void SetupClient(const char *host, uint16_t port) = 0;
  virtual void SetupServer(const char *host, uint16_t port) = 0;
  virtual void Accept() {}
  virtual void Connect() {}
  virtual void Close() {}
  virtual uint32_t RecvRaw(uint32_t len, uint8_t *data) = 0;
  virtual uint32_t SendRaw(uint32_t len, uint8_t *data) = 0;
  virtual Packet *Recv();
  virtual uint32_t Send(uint32_t len, uint8_t *data);
  virtual uint32_t Send(Packet *p);
  virtual void SetOpt(const char */*option*/, const char */*value*/) {}
  static void SetAddr(struct sockaddr_in **addr, const char *host,
                      uint16_t port);
  static Transport *GetTransport(const char *transport);
};

}  // namespace transputation
