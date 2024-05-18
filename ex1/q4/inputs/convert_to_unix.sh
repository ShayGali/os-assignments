#!/bin/bash

dos2unix good_input*.txt

for file in bad_input*.txt
do
    dos2unix "$file"
done