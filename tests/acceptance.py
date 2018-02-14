#!/usr/bin/env python3

# Author: Kyle Martinez

from filecmp import cmp
from subprocess import PIPE, run
from shutil import copy
import os
import sys

NUM_TESTCASES = 4

print("----------------------------------------------------------------------")
print("COMPILING")
print("----------------------------------------------------------------------")

with open(os.devnull, 'w') as devnull:
    result = run("make", stdout=devnull, stderr=PIPE)
if result.stderr:
    print("✖ Failure! Please manually build to determine the cause.")
    sys.exit(1)
else:
    print("✓ Success!")

print("----------------------------------------------------------------------")
print("RUNNING ACCEPTANCE TESTS")
print("----------------------------------------------------------------------")

passed = 0
os.chdir("./tests")
for i in range(1, NUM_TESTCASES + 1):
    in_filename = "set{i}_process.in".format(i=i)
    out_filename = "set{i}_processes.out".format(i=i)

    print("Running {test}:".format(test=in_filename))
    copy(in_filename, "processes.in")
    with open(os.devnull, 'w') as devnull:
        result = run("../bin/scheduler", stdout=devnull, stderr=devnull)
    if result.returncode != 0:
        print("✖ | Exit failure")
    elif not cmp(out_filename, "processes.out"):
        print("✖ | Output mismatch")
    else:
        print("✓ | Passed")
        passed += 1
try:
    os.remove("processes.in")
    os.remove("processes.out")
except FileNotFoundError:
    pass

print("\nTests Passed = {p} / {t}".format(p=passed, t=NUM_TESTCASES))
print("----------------------------------------------------------------------")

sys.exit(0)
