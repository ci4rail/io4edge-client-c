



# Install protobuf-c



# Build for debug
    
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug  -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
ln -s debug/compile_commands.json .
```

# Testing

```bash
make test
```

# Build example outside of the project

```bash
gcc -o example1 example1.c -lpthread -lrt -lio4edge -lprotobuf-c
```