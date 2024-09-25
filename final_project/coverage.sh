# error on running the server
./main -w
./main -p -l
./main -l -p

./main &
# kill the server
python3 my_client.py inputs/kill.txt

./main -l &
python3 my_client.py inputs/input.txt &
sleep 1
python3 my_client.py inputs/bad_input.txt &
# wait for the client to finish
sleep 1
# kill the server
python3 my_client.py inputs/kill.txt

./main -p &
python3 my_client.py inputs/input.txt &
sleep 1
python3 my_client.py inputs/bad_input.txt &
# wait for the client to finish
sleep 1
# kill the server
python3 my_client.py inputs/kill.txt
