#pragma once

namespace transputation {
class UDT;
}

#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "debug.h"
#include "transport.h"
#include "udt/udt.h"

namespace transputation {
class UDT : public Transport {
 public:
  UDT();
  ~UDT();
  void SetupClient(const char *host, uint16_t port);
  void SetupServer(const char *host, uint16_t port);
  void Accept();
  void Connect();
  void Close();
  uint32_t RecvRaw(uint32_t len, uint8_t *data);
  uint32_t SendRaw(uint32_t len, uint8_t *data);

 protected:
  UDTSOCKET fd;
  UDTSOCKET clientfd;
  struct sockaddr_in *local;
  struct sockaddr_in *remote;
};
}  // namespace transputation
