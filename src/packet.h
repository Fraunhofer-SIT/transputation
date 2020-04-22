#pragma once

namespace transputation {
class Packet;
}

#include <arpa/inet.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

namespace transputation {
class Packet {
 public:
  Packet();
  explicit Packet(Packet *packet);
  Packet(uint32_t len, uint8_t *data);
  void Set(Packet *p);
  void Set(uint32_t len, uint8_t *data);
  void Append(Packet *p);
  void Append(uint32_t len, uint8_t *data);
  bool Equals(Packet *packet);
  ~Packet();
  uint32_t len;
  uint32_t rawlen;
  uint8_t *raw;
  uint8_t *content;
};

}  // namespace transputation
