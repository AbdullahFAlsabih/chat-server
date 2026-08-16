# C Multi-Client TCP Chat Server

A lightweight, non-blocking TCP chat server written in C using `select()` for I/O multiplexing. It supports concurrent client connections, username assignment, and message broadcasting without requiring any custom client software.

## Features

- **No Dedicated Client Needed:** Connect directly from any terminal using standard networking tools (`nc`) via the server IP and port.



- **I/O Multiplexing:** Handles multiple client connections on a single thread using `select()`.
- **Broadcast Messaging:** Forwards messages from any client to all other connected peers.
- **Client Management:** Tracks connected sockets and dynamically handles disconnects/quit commands.

