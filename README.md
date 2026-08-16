# chat-server  C Multi-Client TCP Chat Server

A lightweight, non-blocking TCP chat server written in C using `select()` for I/O multiplexing. It supports concurrent client connections, username assignment, and message broadcasting.

## Features

- **I/O Multiplexing:** Handles multiple client connections on a single thread using `select()`.
- **Broadcast Messaging:** Forwards messages from any client to all other connected peers.
- **Client Management:** Tracks connected sockets and dynamically handles disconnects/quit commands.


