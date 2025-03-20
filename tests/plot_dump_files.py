import numpy as np
import matplotlib.pyplot as plt
import os

files = [f for f in os.listdir() if f.endswith(".csv")]

for file in files:
    data = np.genfromtxt(file, delimiter=",").T

    plt.figure()

    for line in data[1:]:
        plt.plot(data[0], line)

    plt.title(file)
    plt.show()