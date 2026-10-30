import matplotlib.pyplot as plt



X = [0, 0, 10.25, 10.25, 10.25, 10.25, 0, 0, 0, 5.096, 4.87, 4.87, 4.87, 4.87, 4.87, 4.87, 10.25, 12.25, 14.25, 27.788, -1.8, -3.6]
Y = [0, 0.042, 0.019, 0.046, 3.748, 3.7435, 3.749, 3.7445, 1.91, 3.152, 1.119, 7.3935, -0.406, 1.909, 3.794, 5.2935, 1.909, 1.909, 1.909, 1.909, 1.909, 1.909]
P = [1, 1.2, 2, 2.2, 3, 3.2, 4, 4.2, 5, 6, 7, 7.1, 7.2, 7.3, 7.4, 7.5, 8, 8.1, 8.2, 8.3, 10.1, 10.2]

plt.grid()



points_to_exclude = [1,2,3,4]
point_to_change_name = [1.2,2.2,3.2,4.2]
for x,y,p in zip(X, Y,P):
    if p not in points_to_exclude:
        if p in point_to_change_name:
            p = int(p)
        plt.annotate(f"{p}:({x}, {y})", xy=(x,y), xytext=(x, y))
        plt.plot(x,y, 'ro') 
    


plt.show()

