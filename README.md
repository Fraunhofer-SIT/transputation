# Transputation Framework

Transputation allows secure computation researchers to focus on the protocol details without worrying about the networking aspects of the implementation.

**This code is provided as an experimental implementation. We cannot guarantee security or correctness**

## Getting Started

### Compiling

Compilation of the is straight forward.

```
cd transputation
make
```

### Installing

Simply run (as root)

```
make install
```

### Using the Framework

Transputation is written in C++ and must be used with a C++ compiler.

#### Minimal working example client:

```
#include <stdio.h>
#include <transputation/transputation.h>

using namespace transputation;

int main(int argc, char *argv[]) {
  uint8_t data[256];

  for (uint16_t i = 0; i < 256; i++) {
    data[i] = i;
  }

  Packet *packet = new Packet(256, data);
  Transport *t = Transport::GetTransport("TCP");

  t->SetupClient("127.0.0.1", 1234);
  t->Connect();
  t->Send(packet);
  t->Close();
}
```

Compile with `c++ -o client client.cc -ltransputation -lpthread`

#### Minimal working example server:

```
#include <stdio.h>
#include <transputation/transputation.h>

using namespace transputation;

int main(int argc, char *argv[]) {
  uint8_t data[256];

  for (uint16_t i = 0; i < 256; i++) {
    data[i] = i;
  }

  Packet *expected = new Packet(256, data);
  Transport *t = Transport::GetTransport("TCP");

  t->SetupServer("0.0.0.0", 1234);
  t->Accept();
  Packet *packet = t->Recv();

  if (packet->Equals(expected)) {
    printf("Packets match.\n");
  } else {
    printf("Packets differ.\n");
  }

  t->Close();
```

Compile with `c++ -o server server.cc -ltransputation -lpthread`

## Example uses

We made an experimental fork of libscapi using the transputation framework. It can be found at [https://github.com/Fraunhofer-SIT/transputation_libscapi](https://github.com/Fraunhofer-SIT/transputation_libscapi)

## License

This project is licensed under the GPL2 License - see the [LICENSE.txt](LICENSE.txt) file for details.

UDT is licensed under the BSD License - see the [LICENSE.UDT.txt](LICENSE.UDT.txt) file for details.