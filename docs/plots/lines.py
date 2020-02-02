from functools import partial

import matplotlib.pyplot as plt
import numpy as np

N = np.arange(3)

fastq = ('100.fastq', '10K.fastq', '100K.fastq')

impls = ('Serial', 'OpenMP', 'MPI')

readcsv = partial(
    np.genfromtxt, usecols=1,
    delimiter=',', skip_header=1
)

means = np.vstack([readcsv(
    'benchmarks/' + fq.replace('fastq', 'csv')
) for fq in fastq]) * 1e3

plt.figure()

plt.plot(N, means[:, 0], 'bd--', label=impls[0])
plt.plot(N, means[:, 1], 'gd--', label=impls[1])
# MPI is too slooooow
# plt.plot(N, means[:, 2], 'rd--', label=impls[2])

plt.xticks(N, fastq)
plt.xlabel('Input file')

plt.ylabel('Execution time (ms)')

plt.legend()
plt.show()
