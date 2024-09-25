# tcp client that send a text file line by line to a server

import socket
import time
import sys

def main(file_name:str):
    # create a socket object
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # get local machine name
    host = "localhost"
    port = 9034

    # connection to hostname on the port.
    s.connect((host, port))

    # open the file
    with open(file_name, "r") as f:
        # read the file line by line
        for line in f:
            # send the line to the server
            s.sendall(line.encode())
            # wait a little bit
            time.sleep(0.01)

    # close the connection
    s.close()

if __name__ == "__main__":
    # get the file name from the arguments
    if len(sys.argv) > 1:
        file_name = sys.argv[1]
    else:
        file_name = "inputs.txt"
    main(file_name)
            