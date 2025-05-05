> This project was developed as part of university coursework and is a collaborative effort between **Terence Lee** and **Hendra Kurniawan**.

# Simple File Transfer Protocol (FTP) in C for Linux

## Authors
- Terence Lee  
- Hendra Kurniawan

## Overview
This project implements a basic File Transfer Protocol (FTP) in C (GNU) for Linux. It enables file transfers between a client and a server over a network connection.

## Features
- **Send and receive files** – Transfer files between client and server.
- **Text-based command interface** – Use simple commands to initiate file transfers.
- **Linux-based** – Designed for GNU/Linux environments using C.
- **Daemon mode** – Server runs in the background as a daemon.
- **Logging** – Server logs all interactions to a log file.

## 📄 **Documentation:** 
[File-Transfer-Protocol-Documentation.pdf](https://github.com/user-attachments/files/20023767/File-Transfer-Protocol-Documentation.pdf)

## Prerequisites
- Linux-based operating system
- GCC (GNU Compiler Collection)

## Getting Started

### Running the Server

```bash
cd Server
make
sudo ./myftpd [initial_current_directory]
```

- The server program is named `myftpd`, where the `d` stands for **daemon**.
- It runs as a background daemon process.
- Optionally, you can provide an initial current directory. If omitted, it inherits the directory from the parent process.
- If supplied, the server sets the directory using the `chdir()` function.
- The server maintains its own current working directory which clients can change using the `cd` command.

### Running the Client

```bash
cd Client
make
sudo ./myftp [hostname | IP_address]
```

- The client program is named `myftp`.
- If a hostname or IP address is not specified, it connects to the **localhost** by default.

## Usage

Once the connection between the client and server is established, the client will display a prompt (`$`), and users can enter the following commands:

| Command                  | Description |
|--------------------------|-------------|
| `pwd`                    | Display the **server's** current working directory. |
| `lpwd`                   | Display the **client's** current working directory. |
| `dir`                    | List files in the **server's** current directory. |
| `ldir`                   | List files in the **client's** current directory. |
| `cd directory_pathname`  | Change the **server's** current directory. Supports `.` and `..`. |
| `lcd directory_pathname` | Change the **client's** current directory. Supports `.` and `..`. |
| `get filename`           | Download a file from the **server** to the **client**. |
| `put filename`           | Upload a file from the **client** to the **server**. |
| `quit`                   | Terminate the FTP session. |

## Notes
- This is an educational project and **not intended for production use**.
- Start the server **before** running the client.
- All communication occurs over a specified TCP port.
- Use in a **safe, local environment** — encryption and user authentication are **not implemented**.
