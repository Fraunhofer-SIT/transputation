#include "transport.h"
#include "tcp.h"
#include "udp.h"
#include "udt.h"

namespace transputation {
uint32_t Transport::Send(uint32_t len, uint8_t *data) {
  uint32_t n;
  Packet *p = new Packet(len, data);
  n = this->Send(p);
  free(p);
  return n;
}

Packet *Transport::Recv() {
  uint32_t buffersize = sizeof(uint32_t);
  uint8_t *buffer = (uint8_t *)malloc(buffersize);
  size_t len;
  uint32_t total = 0;
  uint32_t torecv = 0;
  uint32_t received = 0;
  Packet *p = new Packet();

  DEBUG("Recv...\n");
  len = this->RecvRaw(4, buffer);

  if (len < 4) {
    return p;
  }

  uint32_t *size = (uint32_t *)buffer;
  torecv = total = ntohl(*size);
  buffer = (uint8_t *)realloc(buffer, total);
  DEBUG("Expected total length = %u\n", total);

  while (len > 0 && received != total) {
    uint32_t recvsize = (torecv < buffersize) ? torecv : buffersize;
    len = this->RecvRaw(recvsize, buffer);

    DEBUG("Got chunk with %zu bytes.\n", len);

    p->Append(len, buffer);
    received += len;
    torecv -= len;
  }

  DEBUG("Got %u bytes.\n", received);

  return p;
}

uint32_t Transport::Send(Packet *p) {
  return this->SendRaw(p->rawlen, p->raw);
}

void Transport::SetAddr(struct sockaddr_in **addr, const char *host,
                        uint16_t port) {
  if (*addr == NULL) {
    *addr = (struct sockaddr_in *)calloc(1, sizeof(**addr));
  }

  if (host == NULL) {
    DEBUG("Using host 0.0.0.0 (NULL)\n");
    (*addr)->sin_addr.s_addr = htonl(INADDR_ANY);
  } else {
    DEBUG("Using host %s\n", host);
    struct hostent *h = gethostbyname(host);

    if (!h) {
      fprintf(stderr, "Invalid or unknown host '%s'\n", host);
      exit(1);
    }

    memcpy(&((*addr)->sin_addr.s_addr), h->h_addr, h->h_length);
  }

  (*addr)->sin_family = AF_INET;
  (*addr)->sin_port = htons(port);

  DEBUG("Using port %d\n", port);
}

Transport *Transport::GetTransport(const char *transport) {
  if (!strcasecmp("udp", transport)) {
    return new transputation::UDP();
  }
  if (!strcasecmp("tcp", transport)) {
    return new transputation::TCP();
  }
  if (!strcasecmp("udt", transport)) {
    return new transputation::UDT();
  }

  fprintf(stderr, "Invalid transport option '%s'.\n", transport);
  exit(1);
}

}  // namespace transputation
