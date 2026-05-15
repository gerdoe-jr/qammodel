import argparse
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


def plot_ber(prefix: str, input_dir: Path, output_dir: Path) -> None:
    csv_path = input_dir / Path(f"{prefix}_ber.csv")

    if not csv_path.exists():
        print(f"Skip BER: {csv_path} does not exist")
        return

    df = pd.read_csv(csv_path)

    plt.figure()
    plt.plot(df["variance"], df["ber"], marker="o")
    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("Noise variance")
    plt.ylabel("Bit error rate")
    plt.title(f"{prefix.upper()}: BER vs noise variance")
    plt.grid(True, which="both")
    plt.tight_layout()

    output_path = output_dir / f"{prefix}_ber.png"
    plt.savefig(output_path, dpi=200)
    plt.close()

    print(f"Saved {output_path}")


def plot_ber_comparison(prefixes: list[str], input_dir: Path, output_dir: Path) -> None:
    plt.figure(figsize=(9, 6))

    has_data = False

    for prefix in prefixes:
        csv_path = input_dir / Path(f"{prefix}_ber.csv")

        if not csv_path.exists():
            print(f"Skip BER comparison: {csv_path} does not exist")
            continue

        df = pd.read_csv(csv_path)

        if df.empty:
            print(f"Skip BER comparison: {csv_path} is empty")
            continue

        df["variance"] = pd.to_numeric(df["variance"])
        df["ber"] = pd.to_numeric(df["ber"])

        df = df.sort_values("variance")

        plt.plot(
            df["variance"], df["ber"], marker="o", linewidth=1.8, label=prefix.upper()
        )

        has_data = True

    if not has_data:
        plt.close()
        print("No BER data for comparison plot")
        return

    plt.xscale("log")
    plt.yscale("log")

    plt.xlabel("Noise variance")
    plt.ylabel("Bit error rate")
    plt.title("BER comparison")

    plt.grid(True, which="both", linestyle="--", alpha=0.6)
    plt.legend()
    plt.tight_layout(pad=2.0)

    output_path = output_dir / "ber_comparison.png"
    plt.savefig(output_path, dpi=220, bbox_inches="tight")
    plt.close()

    print(f"Saved {output_path}")


def plot_constellation(
    prefix: str, kind: str, input_dir: Path, output_dir: Path
) -> None:
    csv_path = input_dir / Path(f"{prefix}_constellation_{kind}.csv")

    if not csv_path.exists():
        print(f"Skip constellation: {csv_path} does not exist")
        return

    df = pd.read_csv(csv_path)

    x = df["real"]
    y = df["imag"]

    plt.figure(figsize=(8, 8))
    plt.scatter(x, y, s=14, alpha=0.65)

    plt.xlabel("In-phase (I)")
    plt.ylabel("Quadrature (Q)")
    plt.title(f"{prefix.upper()}: constellation {kind}")

    max_abs = max(x.abs().max(), y.abs().max())
    padding = max_abs * 0.25 + 0.25
    limit = max_abs + padding

    plt.xlim(-limit, limit)
    plt.ylim(-limit, limit)

    plt.axhline(0.0, linewidth=1.8)
    plt.axvline(0.0, linewidth=1.8)

    plt.grid(True, linestyle="--", alpha=0.6)
    plt.axis("equal")

    plt.xticks([-limit, -limit / 2, 0, limit / 2, limit])
    plt.yticks([-limit, -limit / 2, 0, limit / 2, limit])

    plt.tight_layout(pad=2.5)

    output_path = output_dir / f"{prefix}_constellation_{kind}.png"
    plt.savefig(output_path, dpi=220, bbox_inches="tight")
    plt.close()

    print(f"Saved {output_path}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--in-dir", default="data", help="Directory for input CSV files"
    )
    parser.add_argument(
        "prefixes", nargs="+", help="Simulation prefixes, for example: qam4 qam16 qam64"
    )
    parser.add_argument(
        "--out-dir", default="plots", help="Directory for output PNG files"
    )

    args = parser.parse_args()

    input_dir = Path(args.in_dir)

    output_dir = Path(args.out_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    for prefix in args.prefixes:
        plot_ber(prefix, input_dir, output_dir)

        for kind in ["clean", "noisy_first", "noisy_last"]:
            plot_constellation(prefix, kind, input_dir, output_dir)

    plot_ber_comparison(args.prefixes, input_dir, output_dir)


if __name__ == "__main__":
    main()
