import matplotlib.pyplot as plt
import pandas as pd

path = "/tmp/stacs_plot_data.csv"

data = pd.read_csv(path)
headers = data.columns

if len(headers) > 1:
    for header in headers[1:]:
        plt.plot(data[headers[0]], data[header], label=header)
    plt.xlabel(headers[0])
    plt.legend()
else:
    plt.plot(data[headers[0]], label=headers[0])

plt.show()
