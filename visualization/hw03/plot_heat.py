from pylab import *
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import pandas as pd

resolution = 50

def print_3d(data: pd.DataFrame):

    data["x"] += 2
    data["y"] += 2
    data["z"] += 2

    data = data[(data.x % 10 == 0) & (data.y % 10 == 0) & (data.z % 10 == 0)]

    x = data["x"].to_numpy()
    y = data["y"].to_numpy()
    z = data["z"].to_numpy()
    t = data["t"].to_numpy()

    plt.rcParams.update({
        "text.usetex": True,
        "font.family": "Helvetica",
        "font.size": 15
    })

    fig = plt.figure(figsize=(10, 10))
    ax = fig.add_subplot(111, projection='3d')

    norm = matplotlib.colors.PowerNorm(0.1)
    color_map = cm.ScalarMappable(norm=norm)
    color_map.set_array(t)

    img = ax.scatter(x, y, z, s=150, c=t,
                     norm=norm)
    ax.set_xlabel('$x$')
    ax.set_ylabel('$y$')
    ax.set_zlabel('$z$')
    cbar = plt.colorbar(color_map)
    cbar.ax.set_ylabel('$[$temperature$] = K$')
    plt.show()

if __name__ == '__main__':
    data = pd.read_csv("gathered3d_par100_30000_heatmap_data.csv", sep=",")
    print_3d(data)
