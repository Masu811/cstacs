from test import sort
import numpy as np
import time

a = np.arange(1, 10000, 1, dtype=object)
np.random.shuffle(a)
b = a.copy()
a = list(a)
b = list(b)

start = time.perf_counter()
sorted_a = sort(a)
stop = time.perf_counter()
print(f"C function: {(stop-start)*1000} ms")
sorted_a = np.array(sorted_a)
a_is_sorted = np.all(sorted_a[:-1] <= sorted_a[1:])
print(f"Sorting successful: {a_is_sorted}")

start = time.perf_counter()
for i in range(len(b) - 1):
    j = i
    while b[j+1] < b[j] and j >= 0:
        b[j+1], b[j] = b[j], b[j+1]
        j -= 1

stop = time.perf_counter()
print(f"Python function: {(stop-start)*1000} ms")
sorted_b = np.array(b)
b_is_sorted = np.all(sorted_b[:-1] <= sorted_b[1:])
print(f"Sorting successful: {b_is_sorted}")

