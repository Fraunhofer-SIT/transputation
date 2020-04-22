#include "udt.h"

namespace transputation {
UDT::UDT() : clientfd(-1), local(NULL), remote(NULL) {
  ::UDT::startup();
  fd = ::UDT::socket(AF_INET, SOCK_STREAM, 0);

  DEBUG("File descriptor: %d\n", fd);

  if (fd < 0) {
    fprintf(stderr, "Error creating socket: %s\n",
            ::UDT::getlasterror().getErrorMessage());
    exit(1);
  }

  int one = 1;
  int bufsize = 1024 * 1024 * 10;
  ::UDT::setsockopt(fd, 0, UDT_REUSEADDR, (void *)&one, sizeof(one));
  ::UDT::setsockopt(fd, 0, UDT_LINGER, (void *)&one, sizeof(one));
  ::UDT::setsockopt(fd, 0, UDT_MSS, new int(65000), sizeof(int));
  ::UDT::setsockopt(fd, 0, UDP_RCVBUF, (void *)&bufsize, sizeof(bufsize));
//  ::UDT::setsockopt(fd, 0, UDT_RCVBUF, (void *)&bufsize, sizeof(bufsize));

//  ::UDT::setsockopt(fd, 0, UDP_SNDBUF, (void *)&bufsize, sizeof(bufsize));
}

UDT::~UDT() {
  this->Close();
  if (local != NULL) {
    free(local);
  }
  if (remote != NULL) {
    free(remote);
  }
  ::UDT::cleanup();
}

void UDT::SetupClient(const char *host, uint16_t port) {
  DEBUG("Setting remote address\n");
  this->SetAddr(&remote, host, port);
}

void UDT::SetupServer(const char *host, uint16_t port) {
  DEBUG("Setting local address\n");
  this->SetAddr(&local, host, port);

  if (::UDT::bind(fd, (struct sockaddr *)local, sizeof(*local)) != 0) {
    fprintf(stderr, "Error binding to socket: %s\n",
            ::UDT::getlasterror().getErrorMessage());
    exit(1);
  }

  if (::UDT::listen(fd, 1) != 0) {
    fprintf(stderr, "Error listening on socket: %s\n",
            ::UDT::getlasterror().getErrorMessage());
    exit(1);
  }
}

void UDT::Accept() {
  int size = sizeof(*remote);
  clientfd = ::UDT::accept(fd, (struct sockaddr *)remote, &size);
  if (clientfd < 0) {
    fprintf(stderr, "Accept failed: %s",
            ::UDT::getlasterror().getErrorMessage());
    exit(1);
  }
}

void UDT::Connect() {
  if (::UDT::connect(fd, (struct sockaddr *)remote, sizeof(*remote)) < 0) {
    fprintf(stderr, "Connection failed: %s\n",
            ::UDT::getlasterror().getErrorMessage());
    exit(1);
  }

  // keep send and recv compatible with the server invocations
  clientfd = fd;
}

void UDT::Close() {
  ::UDT::close(clientfd);
  ::UDT::close(fd);
  clientfd = -1;
}

uint32_t UDT::RecvRaw(uint32_t len, uint8_t *data) {
  uint32_t received = 0;
  uint8_t *buffer = (uint8_t *)malloc(len);

  while (received < len) {
    int n = ::UDT::recv(clientfd, (char *)buffer, len - received, 0);

    if (n < 0) {
      fprintf(stderr, "Error recv: %s\n",
              ::UDT::getlasterror().getErrorMessage());
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

uint32_t UDT::SendRaw(uint32_t len, uint8_t *data) {
  if (clientfd >= 0) {
    DEBUG("Sending %u bytes.\n", len);

    ssize_t n = ::UDT::send(clientfd, (char *)data, len, 0);

    if (n < 0) {
      fprintf(stderr, "Send error: %s",
              ::UDT::getlasterror().getErrorMessage());
      return 0;
    }

    DEBUG("Sent %zd bytes.\n", n);
    return n;
  }
  DEBUG("Remote not set.\n");
  return 0;
}

}  // namespace transputation
