import sys
import os
import glob
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import re

plt.rcParams.update({
    "font.size": 18,
    "axes.titlesize": 16,
    "axes.labelsize": 16,
    "xtick.labelsize": 16,
    "ytick.labelsize": 16,
    "legend.fontsize": 14
})

COLORS = [
    "#66c2a5",
    "#fc8d62",
    "#8da0cb",
]

OUT = "./plots"


def load_csvs(pattern):
    files = glob.glob(pattern)
    dfs = []

    for f in files:
        df = pd.read_csv(f)

        if "resolution" not in df.columns:
            m = re.search(r"res(\d+)", f)
            if m:
                df["resolution"] = int(m.group(1))
            else:
                raise ValueError(f"Cannot determine resolution from {f}")

        dfs.append(df)

    return pd.concat(dfs, ignore_index=True)


def relative_error_boxplot(df):
    resolutions = sorted(df["resolution"].unique())

    data = [
        df[df["resolution"] == r]["relative_error"].values
        for r in resolutions
    ]

    plt.figure(figsize=(8,6))

    bp = plt.boxplot(data, patch_artist=True)

    for i, box in enumerate(bp["boxes"]):
        box.set_facecolor(COLORS[i % len(COLORS)])
        box.set_edgecolor("black")

    for median in bp["medians"]:
        median.set_color("black")
        median.set_linewidth(2)

    plt.xticks(range(1, len(resolutions)+1), resolutions)

    plt.ylabel("Relative error")
    plt.xlabel("H3 resolution")
    plt.title("Relative error distribution")

    plt.grid(axis="y", linestyle="--", alpha=0.6)

    os.makedirs(OUT, exist_ok=True)
    path = os.path.join(OUT, "relative_error_boxplot.jpg")

    plt.tight_layout()
    plt.savefig(path, bbox_inches="tight")
    plt.close()

    print(f"Saved {path}")


def relative_error_cdf(df):
    plt.figure(figsize=(8,6))

    resolutions = sorted(df["resolution"].unique())

    for i, r in enumerate(resolutions):
        data = df[df["resolution"] == r]["relative_error"].values
        data = sorted(data)

        y = [i/len(data) for i in range(len(data))]

        plt.plot(
            data,
            y,
            label=f"Resolution {r}",
            color=COLORS[i % len(COLORS)],
            linewidth=2
        )

    #plt.xticks(np.arange(0, 21, 2))
    plt.xlabel("Relative error")
    plt.ylabel("CDF")

    plt.title("CDF of relative error")

    plt.grid(axis="both", linestyle="--", alpha=0.6)
    plt.legend()

    os.makedirs(OUT, exist_ok=True)
    path = os.path.join(OUT, "relative_error_cdf.jpg")

    plt.tight_layout()
    plt.savefig(path, bbox_inches="tight")
    plt.close()

    print(f"Saved {path}")


def error_vs_distance_per_resolution(df):
    bins = [0, 2, 5, 10, 20, 40, 80]
    df = df.copy()
    df["distance_bin"] = pd.cut(df["distance_h3"], bins=bins)

    resolutions = sorted(df["resolution"].unique())

    plt.figure(figsize=(10,6))

    for i, r in enumerate(resolutions):
        sub = df[df["resolution"] == r]

        grouped = sub.groupby("distance_bin")["relative_error"]

        medians = grouped.median()
        p25 = grouped.quantile(0.25)
        p75 = grouped.quantile(0.75)

        x = range(len(medians))
        labels = [str(b) for b in medians.index]

        color = COLORS[i % len(COLORS)]

        # Median line
        plt.plot(
            x,
            medians.values,
            label=f"Resolution {r}",
            color=color,
            linewidth=2
        )

        # IQR band
        plt.fill_between(
            x,
            p25.values,
            p75.values,
            color=color,
            alpha=0.2
        )

    plt.xticks(x, labels, rotation=45)

    plt.ylabel("Relative error")
    plt.xlabel("H3 distance (binned)")
    plt.title("Relative error vs H3 distance (per resolution)")

    plt.grid(axis="y", linestyle="--", alpha=0.6)
    plt.legend()

    os.makedirs(OUT, exist_ok=True)
    path = os.path.join(OUT, "relative_error_vs_distance_per_resolution.jpg")

    plt.tight_layout()
    plt.savefig(path, bbox_inches="tight")
    plt.close()

    print(f"Saved {path}")


def main():
    if len(sys.argv) != 2:
        print("Usage: python3 errorPlots.py <csv_pattern>")
        sys.exit(1)

    pattern = sys.argv[1]

    df = load_csvs(pattern)

    df["relative_error"] = df["relative_error"].abs()

    #relative_error_boxplot(df)
    relative_error_cdf(df)
    #error_vs_distance_per_resolution(df)


if __name__ == "__main__":
    main()