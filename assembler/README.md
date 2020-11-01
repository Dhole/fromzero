# assembler

Reference book: https://www.davidsalomon.name/assem.advertis/AssemAd.html

## ARMv6-M

ARMv6-M Architecture Reference Manual: https://static.docs.arm.com/ddi0419/d/DDI0419D_armv6m_arm.pdf

- ARM Cortex-M1 implements the ARMv6-M ISA

## ARMv7-M

ARMv7-M Architecutre Reference Manual: https://static.docs.arm.com/ddi0403/e/DDI0403E_c_armv7m_arm.pdf

- ARM Cortex-M3 implements the ARMv7-M ISA
- ARMv7-M is a superset of ARMv6-M

## RISC-V development notes

Assembly test:
```
make main_valgrind SRC=test-rv32i.a.asm OUT=test-rv32i.a.bin
```

Disassembly test:
```
echo 'pd 20' | r2 -a riscv -b 32 -m 0 test-rv32i.a.bin
```

Memory profiling:

- Recording
```
valgrind --tool=massif --ignore-fn=_IO_file_doallocate --ignore-fn=__fopen_internal  ./main test-led.asm test-led.bin
```

- View results
```
ms_print massif.out.1240626
```
or
```
massif-visualizer massif.out.1240737
```
