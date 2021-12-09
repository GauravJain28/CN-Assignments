#!/bin/sh

../waf --run Third_1
../waf --run Third_2
../waf --run Third_3

python3 plotQ3.py ../plots/part3/config_1_con1.csv ../plots/part3
python3 plotQ3.py ../plots/part3/config_1_con2.csv ../plots/part3
python3 plotQ3.py ../plots/part3/config_1_con3.csv ../plots/part3
python3 plotQ3.py ../plots/part3/config_2_con1.csv ../plots/part3
python3 plotQ3.py ../plots/part3/config_2_con2.csv ../plots/part3
python3 plotQ3.py ../plots/part3/config_2_con3.csv ../plots/part3
python3 plotQ3.py ../plots/part3/config_3_con1.csv ../plots/part3
python3 plotQ3.py ../plots/part3/config_3_con2.csv ../plots/part3
python3 plotQ3.py ../plots/part3/config_3_con3.csv ../plots/part3