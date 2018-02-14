#!/usr/bin/env python3

# Author: Kyle Martinez

from filecmp import cmp
from subprocess import call
from shutil import copy
import os
import sys

NUM_TESTCASES = 4

print("----------------------------------------------------------------------")
print("COMPILING")
print("----------------------------------------------------------------------")
if call("make") != 0:
    print("✖ Failure! Try manually building to inspect the problem.")
    sys.exit(1)
else:
    print("✓ Success!")

print("----------------------------------------------------------------------")
print("RUNNING ACCEPTANCE TESTS")
print("----------------------------------------------------------------------")
os.chdir("./tests")
passed = 0
for i in range(1, NUM_TESTCASES + 1):
    in_filename = "set{i}_process.in".format(i=i)
    out_filename = "set{i}_processes.out".format(i=i)
    copy(in_filename, "processes.in")
    print("Running {test}:".format(test=in_filename))
    with open(os.devnull, 'w') as devnull:
        if call("../bin/scheduler", stdout=devnull, stderr=devnull) != 0:
            print("✖ Runtime Error")
            continue
    if cmp(out_filename, "processes.out"):
        print("✓ Passed")
        passed += 1
    else:
        print("✖ Output Mismatch")
    os.remove("processes.in")
    os.remove("processes.out")
print("----------------------------------------------------------------------")
print("Tests Passed = {passed}".format(passed=passed))
print("----------------------------------------------------------------------")
sys.exit(0)
