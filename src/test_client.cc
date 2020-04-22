#include <stdio.h>

#include "packet.h"
#include "transport.h"

void usage(char *cmd) {
  fprintf(stderr,
          "%s <server_ip> <protocol> <# of bytes to send> "
          "[<udp delay>] [<udp packet size>]\n",
          cmd);
}

int main(int argc, char *argv[]) {
  // check args
  if (argc < 4) {
    usage(argv[0]);
    return -1;
  }

  // make data
  uint32_t size = atoi(argv[3]);
  uint8_t *data = (uint8_t *)malloc(size);

  for (uint32_t i = 0; i < size; i++) {
    data[i] = i % 256;
  }

  auto *p1 = new transputation::Packet(size, data);

  // open connection
  auto *t = transputation::Transport::GetTransport(argv[2]);

  if (argc > 4) {
    printf("Setting udpdelay to %s.\n", argv[4]);
    t->SetOpt("udpdelay", argv[4]);
  }
  if (argc > 5) {
    printf("Setting udpmaxsize to %s.\n", argv[5]);
    t->SetOpt("udpmaxsize", argv[5]);
  }

  t->SetupClient(argv[1], 1234);
  printf("Connecting to %s:1234\n", argv[1]);
  t->Connect();
  printf("Sending %u bytes...\n", size);
  t->Send(p1);
  auto *p2 = t->Recv();

  // output result
  int ret_code;
  printf("Received %u bytes.\n", p2->len);
  if (p1->Equals(p2)) {
    printf("Packets match.\n");
    ret_code = 0;
  } else {
    printf("Packets differ.\n");
    ret_code = -1;
  }

  // clean up
  t->Close();

  delete p1;
  delete p2;
  delete t;

  return ret_code;
}
