# binaryiotypea Examples

## main.c

Demonstrates most functions of the binaryIoTypeA client.

Each channel's output is read back on the same channel's input. So you only need to supply all channel groups with supply voltage.

The output 0 is stimulated by a thread to force transitions on the inputs.

Adapt `DEVICE_IP` and `DEVICE_PORT` to your setup and run the compiled binary without arguments.
