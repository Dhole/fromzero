#!/usr/bin/env python3

# from sort import sort

registers = [(f'x{i}', i) for i in range(32)]
registers += [('zero', 0)]
registers += [('ra', 1)]
registers += [('sp', 2)]
registers += [('gp', 3)]
registers += [('tp', 4)]
registers += [(f't{i}', i) for i in range(0, 3)]
registers += [('s0', 8)]
registers += [('fp', 8)]
registers += [('s1', 9)]
registers += [(f'a{i}', i+(10-0)) for i in range(0, 8)]
registers += [(f's{i}', i+(18-2)) for i in range(2, 12)]
registers += [(f't{i}', i+(28-3)) for i in range(3, 7)]

registers = sorted(registers, key=lambda kv: kv[0])
for reg in registers:
    (k, v) = reg
    print(f'{{name: "{k}", value: {v}}},')
print()
print(f'length: {len(registers)}')
# print(registers)
