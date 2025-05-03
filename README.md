# Simple File Transfer Protocol (FTP) in C for Linux

## Overview
This project implements a basic File Transfer Protocol (FTP) in C for Linux. It allows for the transfer of files between a client and server over a local network.

## Features
- **Send and receive files**: Transfer files between a client and server.
- **Simple communication**: Basic text-based commands to initiate file transfers.
- **Linux-based**: Works in a Linux environment using C programming.

## Prerequisites
- Linux-based operating system
- GCC (GNU Compiler Collection) installed


### Server

```bash
cd Server
make
sudo ./myftpd
```

### Client

```bash
cd Client
make
sudo ./myftp
```

### Notes
- This is a simple educational project and not suitable for production use.

- The server must be started before the client attempts to connect.

- All communication happens over a defined TCP port.

- Use within a safe, local environment — there is no encryption or user authentication implemented.
