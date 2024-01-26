# mvbSniffer Examples

## main.c

Dumps the received MVB frames to console. It will dump all frames, except timed out frames (master frames without an answer from the slave).
Program stops after receiving 5 buckets.

If you don't have a real MVB bus, you can use the internal generator of the MVB sniffer to generate some test frames. Enable the generator by setting `TEST_PATTERN`. Comment out the `#define TEST_PATTERN` line to disable the generator and to listen to the real MVB bus.

Adapt `DEVICE_IP` and `DEVICE_PORT` to your setup and run the compiled binary without arguments.