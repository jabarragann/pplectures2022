import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns


def main():
    # Experiment 1: Evaluate the effect of different filter lenghts

    exp_df = pd.read_csv("./results/experiment1_2.csv")
    exp_df = exp_df.loc[exp_df["exec_type"] == "serial"]

    sns.lineplot(data=exp_df, x="filter_size", y="time", hue="type")
    plt.show()


if __name__ == "__main__":
    main()
