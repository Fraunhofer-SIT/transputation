#pragma once

namespace transputation {
class TCP;
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

namespace transputation {
class TCP : public transputation::Transport {
 public:
  TCP();
  ~TCP();
  void SetupClient(const char *host, uint16_t port);
  void SetupServer(const char *host, uint16_t port);
  void Accept();
  void Connect();
  void Close();
  uint32_t RecvRaw(uint32_t len, uint8_t *data);
  uint32_t SendRaw(uint32_t len, uint8_t *data);

 private:
  int fd;
  int clientfd;
  struct sockaddr_in *local;
  struct sockaddr_in *remote;
};

}  // namespace transputation
