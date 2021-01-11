# PP_final 20

## Content

MCTS (MCTS)  
MCTS Leaf Thread (MCTS_L_THD)  
MCTS Root Thread (MCTS_R_THD)  
MCTS Tree Global Lock Thread (MCTS_T_G_THD)  
MCTS Tree Local Lock Thread (MCTS_T_L_THD)  
MCTS Leaf OpenMP (MCTS_L_OMP)  
MCTS Root OpenMP (MCTS_R_OMP)  
MCTS Tree Local Lock OpenMP (MCTS_T_L_OMP)  

## Benchmark

TimeBench  
WinBench

## Parameter

modify fittime in Timebench.py for different think time  
modify boardsize in Timebench.py and Winbench.py  
modify thread_count in include/define.hpp and make again  
modify EXPLORE_PARAM of UCB value calculation in include/define.hpp  
modify round in Winbench.py every round is 10 times with black white switch  

## Usage

need to run timebench first then winbench  
```shell=
make clean
make
python3 Timebench.py [ALL / target] [init / run]
python3 Winbench.py [ALL / target] [init / run]
```
init is for first time or after changing config  
run is to continue base on previous  
ALL will run every type of MCTS  
