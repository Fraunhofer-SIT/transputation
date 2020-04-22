#include <arpa/inet.h>
#include <time.h>
#include "measurement.h"
#include "transport.h"

namespace transputation {
double Measurement::GetLatencyClient(const char *host,
                                     const unsigned short port,
                                     const unsigned short num_packets) {
  double latency = 0;
  struct timespec tstart = {0, 0}, tend = {0, 0};
  transputation::Transport *t = transputation::Transport::GetTransport("udp");
  t->SetupClient(host, port);
  t->Connect();

  for (unsigned short i = 0; i < num_packets; ++i) {
    unsigned short x = htons(i);
    transputation::Packet *p1 = new transputation::Packet(sizeof(i), (uint8_t *)&x);

    clock_gettime(CLOCK_MONOTONIC, &tstart);
    t->Send(p1);

    auto *p2 = t->Recv();
    clock_gettime(CLOCK_MONOTONIC, &tend);
    t->Send(p2);

    bool match = p1->Equals(p2);
    delete p1;
    delete p2;

    latency += (((double)tend.tv_sec + 1.0e-9 * tend.tv_nsec) -
                ((double)tstart.tv_sec + 1.0e-9 * tstart.tv_nsec)) *
               1000.0;

    if (!match) {
      delete t;
      return -1;
    }
  }

  delete t;
  return latency / num_packets;
}

double Measurement::GetLatencyServer(const char *host,
                                     const unsigned short port,
                                     const unsigned short num_packets) {
  double latency = 0;
  struct timespec tstart = {0, 0}, tend = {0, 0};
  transputation::Transport *t = transputation::Transport::GetTransport("udp");
  t->SetupServer(host, port);
  t->Accept();

  for (unsigned short i = 0; i < num_packets; ++i) {
    transputation::Packet *p1 = t->Recv();

    clock_gettime(CLOCK_MONOTONIC, &tstart);
    t->Send(p1);

    auto *p2 = t->Recv();
    clock_gettime(CLOCK_MONOTONIC, &tend);

    bool match = p1->Equals(p2);
    delete p1;
    delete p2;

    latency += (((double)tend.tv_sec + 1.0e-9 * tend.tv_nsec) -
                ((double)tstart.tv_sec + 1.0e-9 * tstart.tv_nsec)) *
               1000.0;

    if (!match) {
      delete t;
      return -1;
    }
  }

  delete t;
  return latency / num_packets;
}

}  // namespace transputation
