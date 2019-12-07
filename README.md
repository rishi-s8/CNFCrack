# CNFCrack
A C++ based Satisfiability Checker for DIMACS format. Implemented as a part of Course Satisfiability Checking at RWTH Aachen University.

### Usage
1. Compile
```shell
./build.sh
```

2. Run
```shell
./solve.sh <testfile>
```

For example, 
```shell
./build.sh
./solve.sh benchmarks/example-1.cnf
```

### Output
1. If the formula is Satisfiable, print ```sat``` along with a ```satisfying assignment```. Return with an exit code ```10```.
2. If the formula is Unsatisfiable, print ```unsat```. Return with an exit code ```20```.


### Implemented Techniques
1. DIMACS parser.
2. Decision.
3. Assignment Trail.
4. Backtracking.
5. Boolean Constraint Propagation.
6. Two watched literals.
7. Jersolow-Wang Heuristic Variable Ordering.