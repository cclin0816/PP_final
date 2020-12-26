# PP_final

## Done

MCTS  
MCTS Leaf Thread  
MCTS Root Thread  
MCTS Tree Global Lock Thread  
MCTS Tree Local Lock Thread

## Undone

MCTS Leaf OpenMP  
MCTS Root OpenMP  
MCTS Tree Local Lock OpenMP  
MCTS Root MPI  
MCTS Leaf Thread Pool  
MCTS Leaf CUDA  
MCTS MIX

## Benchmark

TimeBench  
WinBench

## Usage

modify fittime in Timebench.py  
moidfy boardsize in Timebench.py and include/define.hpp
modify thread_count in include/define.hpp

```shell=
make clean
make
python3 Timebench.py [ALL / [target [init / run]]]
```

Timebench 跑的時候盡量不要用電腦  
Winbench 沒差
