#include "tcp.h"

namespace transputation {
TCP::TCP() : clientfd(-1), local(NULL), remote(NULL) {
  fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  DEBUG("File descriptor: %d\n", fd);

  if (fd < 0) {
    perror("Error creating socket");
    exit(1);
  }

  int one = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&one, sizeof(one));
  setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&one, sizeof(one));
}

TCP::~TCP() {
  this->Close();
  if (local != NULL) {
    free(local);
  }
  if (remote != NULL) {
    free(remote);
  }
}

void TCP::SetupClient(const char *host, uint16_t port) {
  DEBUG("Setting remote address\n");
  this->SetAddr(&remote, host, port);
}

void TCP::SetupServer(const char *host, uint16_t port) {
  DEBUG("Setting local address\n");
  this->SetAddr(&local, host, port);

  if (::bind(fd, (struct sockaddr *)local, sizeof(*local)) < 0) {
    perror("Error binding to socket");
    exit(1);
  }

  if (listen(fd, 1) < 0) {
    perror("Error listening on socket");
    exit(1);
  }
}

void TCP::Accept() {
  socklen_t size = sizeof(*remote);
  clientfd = accept(fd, (struct sockaddr *)remote, (socklen_t *)&size);
  if (clientfd < 0) {
    perror("Accept failed");
    exit(1);
  }
}

void TCP::Connect() {
  if (connect(fd, (struct sockaddr *)remote, sizeof(*remote)) < 0) {
    perror("Connection failed");
    exit(1);
  }

  // keep send and recv compatible with the server invocations
  clientfd = fd;
}

void TCP::Close() {
  close(clientfd);
  clientfd = -1;
}

uint32_t TCP::RecvRaw(uint32_t len, uint8_t *data) {
  uint32_t received = 0;
  uint8_t *buffer = (uint8_t *)malloc(len);

  while (received < len) {
    ssize_t n = recv(clientfd, buffer, len - received, 0);

    if (n < 0) {
      perror("Error recv");
      free(buffer);
      return 0;
    }

    memcpy(data + received, buffer, n);
    received += n;
  }

  free(buffer);
  DEBUG("Got chunk with %u bytes.\n", received);
  return received;
}

uint32_t TCP::SendRaw(uint32_t len, uint8_t *data) {
  if (clientfd >= 0) {
    DEBUG("Sending %u bytes.\n", len);
    ssize_t n = write(clientfd, data, len);
    DEBUG("Sent %zd bytes.\n", n);

    if (n < 0) {
      perror("Send error");
      return 0;
    }

    return n;
  }

  DEBUG("Remote not set.\n");
  return 0;
}

}  // namespace transputation
