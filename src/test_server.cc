#include <stdio.h>

#include "packet.h"
#include "transport.h"

void usage(char *cmd) {
  fprintf(stderr,
          "%s <listen_ip> <protocol> [<udp delay>] [<udp packet size>]\n", cmd);
}

int main(int argc, char *argv[]) {
  // check args
  if (argc < 3) {
    usage(argv[0]);
    return -1;
  }

  auto *t = transputation::Transport::GetTransport(argv[2]);

  if (argc > 3) {
    printf("Setting udpdelay to %s.\n", argv[3]);
    t->SetOpt("udpdelay", argv[3]);
  }
  if (argc > 4) {
    printf("Setting udpmaxsize to %s.\n", argv[4]);
    t->SetOpt("udpmaxsize", argv[4]);
  }


  printf("Listening on %s:1234\n", argv[1]);
  t->SetupServer(argv[1], 1234);
  t->Accept();

  auto p = t->Recv();
  t->Send(p);

  delete p;
  delete t;

  return 0;
}
