#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np

a = []
while True:
    try:
        s = input()
        sa = s.split(',')
        saa = [float(m) for m in sa]
        a.append(saa)
    except:
        break
a = np.array(a).T
plt.figure()
plt.plot(a[0], a[1])
plt.show()
