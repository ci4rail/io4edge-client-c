#!/bin/bash
#
# 
#
apt-get update && export DEBIAN_FRONTEND=noninteractive \
  && apt-get -y install --no-install-recommends gcc g++ git automake libtool \
  make cmake pkg-config libprotobuf-dev libprotoc-dev protobuf-compiler \
  ca-certificates

# Build protobuf-c
git clone https://github.com/protobuf-c/protobuf-c.git && \
  cd protobuf-c && ./autogen.sh && ./configure && make && make install

# Build project
cd /repo && mkdir -p build && cd build && cmake .. && cmake --build . && cmake --install .

