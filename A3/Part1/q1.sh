#!/bin/sh

../waf --run First_1
../waf --run First_2
../waf --run First_3
../waf --run First_4

python3 plotQ1.py ../plots/part1/tcpNewReno.csv ../plots/part1
python3 plotQ1.py ../plots/part1/tcpHighSpeed.csv ../plots/part1
python3 plotQ1.py ../plots/part1/tcpVeno.csv ../plots/part1
python3 plotQ1.py ../plots/part1/tcpVegas.csv ../plots/part1