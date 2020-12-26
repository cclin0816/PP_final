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
fittime = 0.2
boardsize = 13

if not sys.argv[1] in benchtarget:
  raise TargetNotExist

def run_timebench(target, round, size):
  os.system(f"./Timebench {target} {size} {round}")

def fit_to_time(target, sec, boardsize):
  target_time = open(target + ".time", "r")
  timeref = target_time.read()
  timeref = timeref.split()
  target_time.close()
  
  target_conf = open(target + '.conf', 'r')
  target_simc = target_conf.read()
  target_simc = target_simc.split()
  target_conf.close()

  target_conf = open(target + '.conf', 'w')
  for i in range(boardsize * boardsize):
    if i >= len(timeref):
      tr = int(timeref[-1])
    else:
      tr = int(timeref[i])
    simc = ((1000000 * sec) / tr) * int(target_simc[i])
    target_conf.write(f"{round(simc)}\n")
  target_conf.close()
  

def timebench(target, fittime, boardsize, benchtype):
  t1 = time.time()
  print("TimeBench: ", target)
  if benchtype == 'init':
    target_conf = open(target + '.conf', 'w')
    for i in range(boardsize * boardsize):
      target_conf.write("4096\n")
    target_conf.close()
    for i in range(3):
      run_timebench(target, 3, boardsize)
      fit_to_time(target, fittime, boardsize)
  print("FineTune: ", target)
  run_timebench(target, 10, boardsize)
  fit_to_time(target, fittime, boardsize)
  print("Elapse Time: ", time.time() - t1)

if sys.argv[1] == "ALL":
  for target in benchtarget[1:]:
    timebench(target, fittime, boardsize, "init")
else:
  if not sys.argv[2] in benchtypelist:
    raise(ErrorArgument)
  timebench(sys.argv[1], fittime, boardsize, sys.argv[2])