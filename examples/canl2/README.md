# canL2 Examples

## config.c

Demonstrates how to configure the bus timing parameters. Set the defines in the beginning of the file to the desired values, and compile and run the example.

Most modules can be configured with a persistent configuration, via the `can-config` parameter. If the persistent configuration is used, the configuration via config.c is not needed.

Example:
```
io4e-canl2-config
```

## send.c

Before calling this example, the `io4e-canl2-config` example should be called to configure the bus timing parameters.

Send a number of CAN test frames. The frames are sent in bursts (called a bucket), and each bucket contains a number of frames. The number of buckets and the number of frames per bucket can be configured via the defines in the beginning of the file.

For example, with the default configuration, the frames are generated as follows:
```
ID=0x100  DATA=
ID=0x100  DATA=01
ID=0x100  DATA=02 02
ID=0x100  DATA=03 03 03
ID=0x100  DATA=04 04 04 04
ID=0x101  DATA=
ID=0x101  DATA=01
ID=0x101  DATA=02 02
ID=0x101  DATA=03 03 03
ID=0x101  DATA=04 04 04 04
ID=0x102  DATA=
...
```

Example:
```
io4e-canl2-send
```

## dumpstream.c

Before calling this example, the `io4e-canl2-config` example should be called to configure the bus timing parameters.

Dumps the received CAN frames to console. By default it will dump all frames, but it can be configured to dump only frames that match a filter. The filter is configured via the defines in the beginning of the file.

Example:
```
io4e-canl2-dumpstream
I (141758374) transport: Successfully connected to 192.168.24.102:10002
Received 0 telegrams, seq=1
Received 0 telegrams, seq=2
...
Received 1 telegrams, seq=27
10382557373 us: ID: 100 DATA: 00 00 00 00 00 00 00 00 ERROR: CAN_NO_ERRORSTATE: CAN_OK
Received 9 telegrams, seq=28
10382557468 us: ID: 100 DATA: 01 01 01 01 01 01 01 01 ERROR: CAN_NO_ERRORSTATE: CAN_OK
10382557648 us: ID: 100 DATA: 02 02 02 02 02 02 02 02 ERROR: CAN_NO_ERRORSTATE: CAN_OK
10382557738 us: ID: 100 DATA: 03 03 03 03 03 03 03 03 ERROR: CAN_NO_ERRORSTATE: CAN_OK
10382557851 us: ID: 100 DATA: 04 04 04 04 04 04 04 04 ERROR: CAN_NO_ERRORSTATE: CAN_OK
10382557975 us: ID: 100 DATA: 05 05 05 05 05 05 05 05 ERROR: CAN_NO_ERRORSTATE: CAN_OK
10382558098 us: ID: 100 DATA: 06 06 06 06 06 06 06 06 ERROR: CAN_NO_ERRORSTATE: CAN_OK
10382558223 us: ID: 100 DATA: 07 07 07 07 07 07 07 07 ERROR: CAN_NO_ERRORSTATE: CAN_OK
10382558349 us: ID: 100 DATA: 08 08 08 08 08 08 08 08 ERROR: CAN_NO_ERRORSTATE: CAN_OK
10382558469 us: ID: 100 DATA: 09 09 09 09 09 09 09 09 ERROR: CAN_NO_ERRORSTATE: CAN_OK
...
```