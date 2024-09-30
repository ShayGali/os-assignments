echo "error on running the server"

./main -w
./main -p -l
./main -l -p

echo "run the server with no flags"
./main &
# kill the server
python3 my_client.py inputs/kill.txt

echo "run the server with -l flag"
./main -l &
echo "run the a good client"
python3 my_client.py inputs/input.txt &
sleep 1
echo "run the a bad client"
python3 my_client.py inputs/bad_input.txt &
# wait for the client to finish
sleep 1
# kill the server
echo "kill the LF server"
python3 my_client.py inputs/kill.txt

echo "run the server with -p flag"
./main -p &
echo "run the a good client"
python3 my_client.py inputs/input.txt &
sleep 1
# echo "run the a bad client"
# python3 my_client.py inputs/bad_input.txt &
# wait for the client to finish
sleep 1
# kill the server
echo "kill the pipe server"
python3 my_client.py inputs/kill.txt
