import matplotlib.pyplot as plt
import csv
import sys
import numpy as np

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
l = lab.split('_')
cdr = float(l[1])
adr = float(l[3])

lb = 'ChaRate: {} AppRate: {}'.format(cdr,adr)

fig,ax = plt.subplots()
ax.plot(x,y,'-',label=lb,linewidth=1.5)
ax.set_xlabel('Time (in seconds)')

ax.set_ylabel('Congestion Window Size')
ax.set_title('Variation of Congestion Window Size with Time\nChannel Data Rate: {} Application Data Rate: {}'.format(cdr,adr))
#ax.legend()
ax.grid()
fig.savefig('{}/{}.png'.format(pth,lab), bbox_inches='tight')

print("Plot saved: {}/{}.png".format(pth,lab))
