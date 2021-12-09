import matplotlib.pyplot as plt
import csv
import sys
import numpy as np

# font = {'family' : 'sans-serif',
#         'weight' : 'normal',
#         'size'   : 20}

# plt.rc('font', **font)
# plt.rcParams['figure.figsize'] = (18,18)

#plt.style.use('seaborn-whitegrid')

path = sys.argv[1]
pth = sys.argv[2]

x = []
y = []

with open(path,'r') as file:
	plots = csv.reader(file, delimiter = ',')
	
	for row in plots:
		x.append(float(row[0]))
		y.append(int(row[1]))

lab = path[15:-4]

fig,ax = plt.subplots()
ax.plot(x,y,'-',label=lab,linewidth=1.5)
ax.set_xlabel('Time (in seconds)')
ax.set_ylabel('Congestion Window Size')
ax.set_title('Variation of Congestion Window Size with Time')
ax.legend()
ax.grid()
fig.savefig('{}/{}.png'.format(pth,lab), bbox_inches='tight')

# plt.plot(x,y)
# plt.xlim([0,30]) 
# plt.savefig('{}/p.png'.format(pth))

print("Plot saved: {}/{}.png".format(pth,lab))
