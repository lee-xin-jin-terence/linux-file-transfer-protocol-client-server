> This project was developed as part of a university coursework. It was a collaborative effort between Terence Lee and Hendra Kurniawan.

# Simple File Transfer Protocol (FTP) in C for Linux

# Authors: 
Terence Lee<br/>
Hendra Kurniawan

## Overview
This project implements a basic File Transfer Protocol (FTP) in C (GNU) for Linux. It allows for the transfer of files between a client and server over a remote network.

## Features
- **Send and receive files**: Transfer files between a client and server.
- **Simple communication**: Basic text-based commands to initiate file transfers.
- **Linux-based**: Works in a Linux environment using GNU C programming.
- **Daemon**: Server runs in the background as daemon
- **Logging**: Server logs all interaction into a log file

**Documentation:** [File-Transfer-Protocol-Documentation.pdf](https://github.com/user-attachments/files/20023767/File-Transfer-Protocol-Documentation.pdf)


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
sudo ./myftp target_server.com:port_number
##e.g. sudo ./myftp ftp_server.com:8080
```

### Notes
- This is a simple educational project and not suitable for production use.

- The server must be started before the client attempts to connect.

- All communication happens over a defined TCP port.

- Use within a safe, local environment — there is no encryption or user authentication implemented.
