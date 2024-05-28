#!/usr/bin/bash

SERVER_PORT=4269
CLIENT_PORT=6699

# TCP
echo "~~~ b option ~~~"

echo "TCP server"
./mync -e "./ttt 123456789" -b TCPS$SERVER_PORT >/dev/null &
nc localhost $SERVER_PORT <./inputs/user_win.txt >/dev/null &

wait

echo "TCP client"
nc -l -p $CLIENT_PORT <./inputs/user_win.txt >/dev/null &
./mync -e "./ttt 123456789" -b TCPClocalhost,$CLIENT_PORT >/dev/null

wait

echo "TCP chat"
mkfifo my_pipe

./mync TCPS$SERVER_PORT &
nc localhost $SERVER_PORT <./inputs/user_win.txt &
cat ./inputs/user_win.txt >my_pipe &
echo "end" >my_pipe

rm my_pipe
wait

# echo "UDP server"
# ./mync -e "./ttt 123456789" -b UDPS$SERVER_PORT -t 1 &
# mkfifo my_pipe
# nc -u -w 1 localhost $SERVER_PORT <my_pipe &

# echo "Connect message" >my_pipe
# cat ./inputs/user_win.txt >my_pipe

# rm my_pipe
# wait

# echo "UDP client"
# mkfifo my_pipe
# nc -u -w 1 -l -p $CLIENT_PORT <my_pipe >/dev/null &
# ./mync -e "./ttt 123456789" -b UDPClocalhost,$CLIENT_PORT -t 1 >/dev/null &

# cat ./inputs/user_win.txt >my_pipe

# rm my_pipe

# wait
