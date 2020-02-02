import matplotlib.pyplot as plt
import numpy as np

fastq = ('100.fastq', '10K.fastq', '100K.fastq')


def table(csv: str, title: str, ax: plt.Axes):
    cols = ('Serial', 'OpenMP', 'MPI')
    rows = ('Mean', 'Std Dev', 'Median', 'Min', 'Max')
    values = np.round(np.genfromtxt(
        'benchmarks/' + csv, usecols=(1, 2, 3, -2, -1),
        delimiter=',', unpack=True, skip_header=1
    ) * 1e3, 3)
    ax.table(
        rowLoc='center', cellLoc='center', loc='center',
        cellText=values, colLabels=cols, rowLabels=rows
    )
    ax.axis('off')
    ax.axis('tight')
    ax.set_title(title)


fig, axes = plt.subplots(len(fastq))

for i, fq in enumerate(fastq):
    table(fq.replace('fastq', 'csv'), fq, axes[i])

fig.patch.set_visible(False)
fig.tight_layout()
plt.show()
