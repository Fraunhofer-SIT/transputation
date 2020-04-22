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
  uint8_t *data1 = (uint8_t *)malloc(size);
  uint8_t *data2 = (uint8_t *)malloc(size);

  for (uint32_t i = 0; i < size; i++) {
    data1[i] = i % 256;
  }


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
  t->SendRaw(size, data1);
  t->RecvRaw(size, data2);

  int ret_code = 0;
  for (uint32_t i = 0; i < size; i++) {
    if (data1[i] != data2[i]) {
      ret_code = -1;
      break;
    }
  }

  // output result
  if (ret_code == 0) {
    printf("Packets match.\n");
  } else {
    printf("Packets differ.\n");
  }

  // clean up
  t->Close();

  free(data1);
  free(data2);
  delete t;

  return ret_code;
}
