import os
import sys
import time
import numpy as np

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
fittime = 2
boardsize = 19
# -------------------------------------------------------

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

  ts = []
  for i in range(len(timeref)):
    ts.append(((1000000 * sec) / int(timeref[i])) * int(target_simc[i]))

  l = round(len(ts) * 4 / 5)
  ts_x = np.arange(l)
  ts_y = ts[:l]
  z = np.polyfit(ts_x, ts_y, 1)
  p = np.poly1d(z)
  print("Fit Line: ", round(z[1]), round(z[0]))
  target_conf = open(target + '.conf', 'w')
  for i in range(boardsize * boardsize):
    target_conf.write(f"{round(p(i))}\n")
  target_conf.close()
  

def timebench(target, fittime, boardsize, benchtype):
  t1 = time.time()
  print("TimeBench: ", target)
  if benchtype == 'init':
    target_conf = open(target + '.conf', 'w')
    for i in range(boardsize * boardsize):
      target_conf.write("40960\n")
    target_conf.close()
    for i in range(2):
      run_timebench(target, 1, boardsize)
      fit_to_time(target, fittime, boardsize)
  # print("FineTune: ", target)
  # run_timebench(target, 3, boardsize)
  # fit_to_time(target, fittime, boardsize)
  print("Elapse Time: ", time.time() - t1)

if not sys.argv[2] in benchtypelist:
  raise(ErrorArgument)

if sys.argv[1] == "ALL":
  for target in benchtarget[1:]:
    timebench(target, fittime, boardsize, sys.argv[2])
else:
  timebench(sys.argv[1], fittime, boardsize, sys.argv[2])