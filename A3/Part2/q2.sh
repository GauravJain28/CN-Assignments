#!/bin/sh

../waf --run Second_1
../waf --run Second_2
../waf --run Second_3
../waf --run Second_4
../waf --run Second_5
../waf --run Second_6
../waf --run Second_7
../waf --run Second_8
../waf --run Second_9
../waf --run Second_10

python3 plotQ2.py ../plots/part2/ChRate_2.000000_AppRate_2.000000.csv ../plots/part2
python3 plotQ2.py ../plots/part2/ChRate_4.000000_AppRate_2.000000.csv ../plots/part2
python3 plotQ2.py ../plots/part2/ChRate_10.000000_AppRate_2.000000.csv ../plots/part2
python3 plotQ2.py ../plots/part2/ChRate_20.000000_AppRate_2.000000.csv ../plots/part2
python3 plotQ2.py ../plots/part2/ChRate_50.000000_AppRate_2.000000.csv ../plots/part2
python3 plotQ2.py ../plots/part2/ChRate_6.000000_AppRate_0.500000.csv ../plots/part2
python3 plotQ2.py ../plots/part2/ChRate_6.000000_AppRate_1.000000.csv ../plots/part2
python3 plotQ2.py ../plots/part2/ChRate_6.000000_AppRate_2.000000.csv ../plots/part2
python3 plotQ2.py ../plots/part2/ChRate_6.000000_AppRate_4.000000.csv ../plots/part2
python3 plotQ2.py ../plots/part2/ChRate_6.000000_AppRate_10.000000.csv ../plots/part2

