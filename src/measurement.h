#pragma once

namespace transputation {
class Transport;
}

namespace transputation {
class Measurement {
 public:
  static double GetLatencyClient(const char *host = "127.0.0.1",
                                 const unsigned short port = 1234,
                                 const unsigned short num_packets = 10);
  static double GetLatencyServer(const char *host = "0.0.0.0",
                                 const unsigned short port = 1234,
                                 const unsigned short num_packets = 10);
};
}  // namespace transputation
