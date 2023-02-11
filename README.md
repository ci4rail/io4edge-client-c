# io4edge-client-c
C/C++ client sdk for io4edge.

[io4edge devices](https://docs.ci4rail.com/edge-solutions/io4edge/) are intelligent I/O devices invented by [Ci4Rail](https://www.ci4rail.com), connected to the host via network.

This library provides support for the following function blocks within io4edge devices:
* Analog In TypeA - IOU01, MIO01
* Binary IO TypeA - IOU01, MIO01

Currently not supported, but will follow:
* CAN Layer2 - IOU03, MIO03, IOU04, MIO04, IOU06
* MVB Sniffer - IOU03, MIO03
* Binary IO TypeC - IOU07
* Binary IO TypeB - IOU06
* Motion Sensor - CPU01UC

Not planned: Support for io4edge management functions, such as firmware update. Please use [io4edge-client-go](http://github.com/ci4rail/io4edge-client-go) for this.

## Restrictions: 

* Service addressing via MDNS is currently not supported. Please use the IP address and port of the io4edge device.
* Currently compiles on Linux only. CMakelists.txt needs to be adapted for other platforms.

## Build and Installation

Please build this library from source. The following dependencies are required:

* [protobuf-c](https://github.com/protobuf-c/protobuf-c): Please install this library first. As an example, see the [github ci pipeline](.github/workflows/buildtest.yml) for Ubuntu.


### Building 

```bash
git clone https://github.com/ci4rail/io4edge-client-c.git --recursive
cd io4edge-client-c

mkdir build && cd build
cmake .. && cmake --build .
```

### Installation

```bash 
cd build
sudo cmake --install .
```
### Usage

See [examples in github repo](https://github.com/ci4rail/io4edge-client-c/examples) for usage examples.

### Using the library outside of the project

```bash
gcc -o example example.c -lpthread -lrt -lio4edge -lprotobuf-c
```

## Development

When using vscode, open this repo in a [devcontainer](./devcontainer). This will install all required dependencies.

### Build for debug
    
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
```
### Running tests

```bash
cd build
make test
```

### Creating a new functionblock client

Copy the closest existing functionblock client, and run [this script](./scripts/rename.sh), e.g.

```bash
scripts/rename.sh src/include/io4edge_analogintypea.h binaryIoTypeA analogInTypeA
scripts/rename.sh src/analogintypea.c binaryIoTypeA analogInTypeA  
```


## Copyright

Copyright © 2023 Ci4Rail GmbH <engineering@ci4rail.com>

io4edge-client-c is released under Apache 2.0 License, see [LICENSE](LICENSE) for details.
