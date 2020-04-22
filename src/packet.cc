#include "debug.h"
#include "packet.h"

namespace transputation {
Packet::Packet() : Packet::Packet(0, NULL) {}

Packet::Packet(Packet *packet) { this->Set(packet->len, packet->content); }

Packet::Packet(uint32_t len, uint8_t *data) { this->Set(len, data); }

Packet::~Packet() { free(this->raw); }

void Packet::Set(Packet *packet) { this->Set(packet->len, packet->content); }

void Packet::Set(uint32_t len, uint8_t *data) {
  this->len = len;
  this->rawlen = len + 4;
  this->raw = (uint8_t *)malloc(rawlen + 1);
  this->content = this->raw + sizeof(uint32_t);
  uint32_t *size = (uint32_t *)this->raw;
  *size = htonl(len);

  if (len > 0) {
    memcpy(this->content, data, len);
  }

  this->content[len] = 0;  // terminate for usage with string functions
}

void Packet::Append(Packet *packet) {
  this->Append(packet->len, packet->content);
}

void Packet::Append(uint32_t len, uint8_t *data) {
  if (this->len == 0) {
    this->Set(len, data);
    return;
  }
  uint32_t oldlen = this->len;

  this->len += len;
  this->rawlen = this->len + 4;
  this->raw = (uint8_t *)realloc(this->raw, this->rawlen + 1);
  this->content = this->raw + sizeof(uint32_t);
  uint32_t *size = (uint32_t *)this->raw;
  *size = htonl(this->len);

  if (this->len > 0) {
    memcpy(this->content + oldlen, data, len);
  }
  this->content[this->len] = 0;  // terminate for usage with string functions
}

bool Packet::Equals(Packet *packet) {
  if (this->rawlen != packet->rawlen) {
    return false;
  }

  bool equal = true;

  for (uint32_t i = 0; i < this->rawlen; i++) {
    if (this->raw[i] != packet->raw[i]) {
      equal = false;
      break;
    }
  }

  return equal;
}

}  // namespace transputation
