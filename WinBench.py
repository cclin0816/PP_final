import os
import sys
import time

class ErrorArgument(Exception):
  pass

class TargetNotExist(Exception):
  pass

benchlist_conf = open('BenchList.conf', 'r')
benchtarget = benchlist_conf.read()
benchtarget = benchtarget.split()
benchlist_conf.close()
benchtypelist = ['init', 'run']
# ----------------------PARAMETER------------------------
boardsize = 13
# -------------------------------------------------------

if not sys.argv[1] in benchtarget:
  raise TargetNotExist

def run_winbench(target, round, size):
  if os.system(f"./Winbench {target} {size} {round}") != 0:
    exit(1)


def winbench(target, boardsize, benchtype):
  t1 = time.time()
  print("WinBench: ", target)
  if benchtype == 'init':
    target_win = open(target + '.win', 'w')
    target_win.write("0 0\n")
    target_win.close()
  run_winbench(target, 5, boardsize)
  print("Elapse Time: ", time.time() - t1)

if not sys.argv[2] in benchtypelist:
  raise(ErrorArgument)

if sys.argv[1] == "ALL":
  for target in benchtarget[1:]:
      winbench(target, boardsize, sys.argv[2])
  for i in range(19):
    for target in benchtarget[1:]:
      winbench(target, boardsize, "run")
else:
  winbench(sys.argv[1], boardsize, sys.argv[2])