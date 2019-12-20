#!/usr/bin/python3

from socket import socket, AF_INET, SOCK_STREAM, SOMAXCONN, SHUT_RDWR
import sys

def process_connection(conn):
    buf_size = 4096 * 2

    data = conn.recv(buf_size)
    print("Got:\n{}\n".format(data))
    response = data.decode()
    response += "\nSincerely yours.\n"
    conn.send(response.encode())

    conn.shutdown(SHUT_RDWR)
    conn.close()


if __name__ == "__main__":
    ip = sys.argv[1]
    port = int(sys.argv[2])
    sock = socket(AF_INET, SOCK_STREAM)
    sock.bind((ip, port))
    sock.listen(SOMAXCONN)
    while True:
        conn, _ = sock.accept()
        process_connection(conn)
