#pragma once

namespace transputation {
class UDP;
}

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "debug.h"
#include "transport.h"

namespace transputation {
class UDP : public Transport {
 public:
  UDP();
  ~UDP();
  void SetupClient(const char *host, uint16_t port);
  void SetupServer(const char *host, uint16_t port);
  void Accept();
  void Connect();
  uint32_t RecvRaw(uint32_t len, uint8_t *data);
  uint32_t SendRaw(uint32_t len, uint8_t *data);
  void SetOpt(const char *option, const char *value);

 private:
  unsigned int udpdelay = 0;
//  unsigned int udpdelay = 100;
//  unsigned int udpmaxsize = 1468;
  unsigned int udpmaxsize = 65500;
  int fd;
  uint8_t buffer[0xffff];
  uint32_t buffersize = 0;
  uint32_t bufferoffset = 0;
  struct sockaddr_in *local;
  struct sockaddr_in *remote;
};

}  // namespace transputation
