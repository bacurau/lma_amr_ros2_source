import matplotlib.pyplot as plt




X = [0, 10.2499, 10.2499, 0, 0, 5.096, 4.87, 4.87, 4.87, 4.87, 4.87, 4.87, 10.2499, 12.2499, 14.2499, 27.7879, -1.8, -3.6]
Y = [0, 0.04, 3.811, 3.815, 1.953, 3.196, 1.163, 7.4375, -0.362, 1.953, 3.838, 5.3375, 1.953, 1.953, 1.953, 1.953, 1.953, 1.953]
P = [1, 2, 3, 4, 5, 6, 7, 7.1, 7.2, 7.3, 7.4, 7.5, 8, 8.1, 8.2, 8.3, 10.1, 10.2]



# ax.plot(A,B)
# for xy in zip(A, B):                                       # <--
#     ax.annotate('(%s, %s)' % xy, xy=xy, textcoords='data') # <--

plt.grid()

plt.plot(X,Y, 'ro')
for x,y,p in zip(X, Y,P):                                       
    plt.annotate(f"{p}:({x}, {y})", xy=(x,y), xytext=(x-0.6, y)) 


#plt.annotate('local max', xy=(2, 1), xytext=(3, 1.5))

plt.show()