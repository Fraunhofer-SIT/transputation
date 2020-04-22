#include "udp.h"

namespace transputation {
UDP::UDP() : local(NULL), remote(NULL) {
  fd = socket(PF_INET, SOCK_DGRAM, 0);

  DEBUG("File descriptor: %d\n", fd);

  if (fd < 0) {
    perror("Error creating socket");
    exit(1);
  }

  int opt = 1;
  int bufsize = 100 *1024 * 1024;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(int));
  setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (void *)&bufsize, sizeof(bufsize));
  setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void *)&bufsize, sizeof(bufsize));
}

UDP::~UDP() {
  this->Close();
  if (local != NULL) {
    free(local);
  }
  if (remote != NULL) {
    free(remote);
  }
}

void UDP::SetupClient(const char *host, uint16_t port) {
  DEBUG("Setting remote address\n");
  this->SetAddr(&remote, host, port);
}

void UDP::SetupServer(const char *host, uint16_t port) {
  DEBUG("Setting local address\n");
  this->SetAddr(&local, host, port);

  int b = ::bind(fd, (struct sockaddr *)local, sizeof(*local));

  DEBUG("Bind result: %d\n", b);

  if (b < 0) {
    perror("Error binding to socket");
    exit(1);
  }

  remote = (struct sockaddr_in *)calloc(1, sizeof(*remote));
}

void UDP::Accept() {
  char *data = (char *)malloc(6);
  char *handshake = (char *)"hello";
  data[6] = 0;

  do {
    this->RecvRaw(6, (uint8_t *)data);
  } while (strncmp(data, handshake, 6));

  this->SendRaw(6, (uint8_t *)handshake);
}

void UDP::Connect() {
  char *data = (char *)malloc(6);
  char *handshake = (char *)"hello";

  do {
    this->SendRaw(6, (uint8_t *)handshake);
    this->RecvRaw(6, (uint8_t *)data);
  } while (strncmp(data, handshake, 6));
}

uint32_t UDP::RecvRaw(uint32_t len, uint8_t *data) {
  uint32_t received = 0;
  socklen_t remotesize = sizeof(remote);

  while (received < len) {
    if (buffersize > 0) {
      uint32_t copysize = (len < buffersize) ? len : buffersize;
      memcpy(data + received, buffer + bufferoffset, copysize);
      buffersize -= copysize;
      bufferoffset += copysize;
      received += copysize;
      continue;
    }
    ssize_t n = recvfrom(fd, buffer, sizeof(buffer), 0,
                         (struct sockaddr *)remote, &remotesize);

    if (n < 0) {
      perror("Error recv");
      return 0;
    }

    bufferoffset = 0;
    buffersize = n;
  }

  DEBUG("Got chunk with %u bytes.\n", received);
  return received;
}

uint32_t UDP::SendRaw(uint32_t len, uint8_t *data) {
  if (remote != NULL) {
    uint32_t tosent = len;
    uint32_t offset = 0;
    uint32_t packetsize;
    ssize_t n;

    DEBUG("Sending %u bytes.\n", len);

    while (tosent > 0) {
      usleep(udpdelay);
      packetsize = (tosent > udpmaxsize) ? udpmaxsize : tosent;
      n = sendto(fd, data + offset, packetsize, 0, (struct sockaddr *)remote,
                 sizeof(*remote));

      if (n < 0) {
        perror("Error sending");
        exit(1);
      }

      offset += packetsize;
      tosent -= packetsize;
    }

    return offset;
  }

  DEBUG("Remote not set.\n");
  return 0;
}

void UDP::SetOpt(const char *option, const char *value) {
  if (!strcasecmp("udpmaxsize", option)) {
    DEBUG("Setting udpmaxsize to %s.\n", value);
    udpmaxsize = atol(value);
  }
  if (!strcasecmp("udpdelay", option)) {
    DEBUG("Setting udpdelay to %s.\n", value);
    udpdelay = atol(value);
  }
}

}  // namespace transputation
