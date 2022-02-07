# Migrate executing threads across machines

## Overview

This repository contains code on how to execute applications which can be stopped during execution and continued on another machine,
without requiring any bootstrapping.

## File Structure

 - `psu_thread.h`: Contains functions which starts and migrates threads
 - `socket.h`: Helper utilities for handling socket based programming.
 - `app4.c`: Extra test case using pointer variables on the stack
 - `app5.c`: Test case using nested function migrations.

## Building

To build the project one can run `make` which will invoke the build via `gcc` from the `Makefile`.

```sh
> make
gcc -std=gnu99 -w -m32 -lpthread -pthread -o app1 app1.c
gcc -std=gnu99 -w -m32 -lpthread -pthread -o app2 app2.c
gcc -std=gnu99 -w -m32 -lpthread -pthread -o app3 app3.c
gcc -std=gnu99 -w -m32 -lpthread -pthread -o app4 app4.c
gcc -std=gnu99 -w -m32 -lpthread -pthread -o app5 app5.c
```

## Execution

After building multiple executables will be generated.

To run them you need to do:

Client:

```sh
./app1 other_host_name_/_ip 0
```

Server:

```sh
./app1 0.0.0.0 1
```

where mode=1 is the server which makes the other host listen, while mode=0 is the client which performs the migration


Sample Execution:

 1. First the server is started on host A:

 ```sh
> ./app3 0.0.0.0 1
  [info] Setting up server
  ...
 ```

 This will then wait for the client to send data

 2. Then client is started on the host B:

 ```sh
> ./app3 e5-cse-135-01.cse.psu.edu 0
 [info] Setting up Client..
Hostname: e5-cse-135-05.cse.psu.edu
Bar: Value of A = 1
Hostname: hostname1
Bar: Value of A = 2
Hostname: hostname1
Bar: Value of A = 3
EBP: 804f478
 [debug] Sent 17184 bytes over the socket
 [debug] Thread exited with value: 0
 ```

 3. Once the client has sent the data to the server, the complete server output will be as follows:

 ```sh
 > ./app3 0.0.0.0 1
  [info] Setting up server
  [debug] Reading 2896 bytes
  [debug] Reading 1448 bytes
  [debug] Reading 4344 bytes
  [debug] Reading 2896 bytes
  [debug] Reading 2896 bytes
  [debug] Reading 2704 bytes
 Hostname: hostname2
 Bar: Value of A = 4
 Hostname: e5-cse-135-01.cse.psu.edu
 Bar: Value of A = 5
 Hostname: hostname2
 Bar: Exit Value of A = 5
 Hostname: hostname2
 Bar: Exit Value of A = 4
 Hostname: hostname2
 Bar: Exit Value of A = 3
 Hostname: hostname2
 Bar: Exit Value of A = 2
 Hostname: hostname2
 Bar: Exit Value of A = 1
  [debug] Thread exited with value: 0
 ```
