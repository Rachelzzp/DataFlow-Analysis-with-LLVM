# !/bin/bash
opt -load ../Pass/build/libLLVMValueNumberingPass.so  -ValueNumbering 1.ll 2> 1.out
opt -load ../Pass/build/libLLVMValueNumberingPass.so  -ValueNumbering 2.ll 2> 2.out
echo SUCCESSFULLY COMPILE
