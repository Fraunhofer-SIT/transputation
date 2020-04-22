#include <stdio.h>

#include "packet.h"
#include "transport.h"

void usage(char *cmd) {
  fprintf(stderr,
          "%s <listen_ip> <protocol> <# of bytes to receive> "
          "[<udp delay>] [<udp packet size>]\n",
          cmd);
}

int main(int argc, char *argv[]) {
  // check args
  if (argc < 4) {
    usage(argv[0]);
    return -1;
  }

  uint32_t size = atoi(argv[3]);
  uint8_t *data = (uint8_t *)malloc(size);
  auto *t = transputation::Transport::GetTransport(argv[2]);

  if (argc > 4) {
    printf("Setting udpdelay to %s.\n", argv[4]);
    t->SetOpt("udpdelay", argv[3]);
  }
  if (argc > 5) {
    printf("Setting udpmaxsize to %s.\n", argv[5]);
    t->SetOpt("udpmaxsize", argv[4]);
  }

  printf("Listening on %s:1234\n", argv[1]);
  t->SetupServer(argv[1], 1234);
  t->Accept();

  t->RecvRaw(size, data);
  t->SendRaw(size, data);

  free(data);
  delete t;

  return 0;
}
