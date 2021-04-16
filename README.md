# smpl_compiler
Writing a compiler for smpl (course project CS241, UCI, professor Franz)

## Build
cd smpl_compiler/
make

## Run
./compiler sourcefile1.smpl sourcefile2.smpl

The output will be in sourcefile1.smpl.IR and sourcefile2.smpl.IR, these files contain dot graph code, so that you can construct graphs of the generated IR.
