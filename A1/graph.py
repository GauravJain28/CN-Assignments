import sys
import matplotlib.pyplot as plt

dmn = sys.argv[1]
ttl = 'RTT vs Hops ('+dmn+')'
with open('out.txt') as f:
    lines = f.readlines()
    x = [float(line.split()[0]) for line in lines]
    y = [float(line.split()[1]) for line in lines]

fig, ax = plt.subplots()
ax.plot(x,y,marker='o')

ax.set(xlabel='No of Hops', ylabel='Round Trip Time (milliseconds)',
       title=ttl)
ax.grid()

fig.savefig("plt.png")
#plt.show()