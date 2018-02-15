#!/usr/bin/env python3

# Author: Kyle Martinez

from filecmp import cmp
from subprocess import Popen, call, PIPE, DEVNULL
from shutil import copy
import os
import sys

NUM_TESTCASES = 4

print("======================================================================")
print("COMPILING")
print("======================================================================")

pipes = Popen("make", stderr=PIPE, stdout=DEVNULL)
pipes.wait()
stderr = pipes.stderr.read()
if pipes.returncode != 0:
    print("✖ Error! Please manually build to determine the cause.")
    sys.exit(1)
elif len(stderr):
    print("✖ Warning! Please manually build to determine the cause.")
    sys.exit(1)
else:
    print("✓ Success!")

print("======================================================================")
print("TESTING")
print("======================================================================")

early_exit = False
passed = 0
os.chdir("./tests")
for i in range(1, NUM_TESTCASES + 1):
    in_filename = "set{i}_process.in".format(i=i)
    out_filename = "set{i}_processes.out".format(i=i)

    print("Running {test}:".format(test=in_filename))
    copy(in_filename, "processes.in")
    if call("../bin/scheduler", stdout=DEVNULL, stderr=DEVNULL) != 0:
        print("✖ | Exit failure")
        early_exit = True
    elif not cmp(out_filename, "processes.out"):
        print("✖ | Output mismatch")
    else:
        print("✓ | Passed")
        passed += 1

cleanup = ["processes.in", "processes.out"]
for filename in cleanup:
    try:
        os.remove(filename)
    except FileNotFoundError:
        pass

print("======================================================================")
print("RESULTS")
print("======================================================================")
print("Tests Passed = {p} / {t}".format(p=passed, t=NUM_TESTCASES))
if early_exit:
    print("Advice: run `make debug` and use gdb or lldb to diagnose failures")
print("======================================================================")

sys.exit(0)
