# HelloPass-LLVM
Getting started for LLVM pass writing

## Setup 
Reference from https://github.com/ufarooq/HelloDataFlow-LLVM

### Results
 .cpp file is in the directory of Pass/Transforms/ValueNumbering/ValueNumbering.cpp
 
 Move to Pass/build/ directory to run: 
 
 	cmake -DCMAKE_BUILD_TYPE=Release ../Transforms/ValueNumbering
	
	make -j4
Then move to HelloPass-LLVM-master/ directory to run the script and will have the two output files:

	./run.sh
