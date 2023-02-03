



# Install protobuf-c



# Build for debug
    
```bash
mkdir debug
cd debug
cmake -DCMAKE_BUILD_TYPE=Debug  -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
ln -s debug/compile_commands.json .
```

# Testing

```bash
make test
```