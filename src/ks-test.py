#! /usr/bin/env python3

from scipy import stats
import numpy as np
import sys
import argparse

parser = argparse.ArgumentParser(description="Test distribution of numbers from stdin.")
parser.add_argument("-d", "--dist", choices=["uniform", "norm"], default="uniform",
                    help="Distribution to test against (default: uniform)")
args = parser.parse_args()

data = np.loadtxt(sys.stdin)
# K-S test against the specified distribution
statistic, p_value = stats.kstest(data, args.dist) 
print(f"P-value: {p_value}")
if p_value < 0.05:
    print('FAIL')
else:
    print('PASS')
