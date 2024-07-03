# Build riscv-openocd

```
sudo dnf install libftdi-devel

mkdir -p ~/tools/riscv-openocd
git clone https://github.com/riscv-collab/riscv-openocd
# git clone https://github.com/RISCV-Tools/riscv-openocd
cd riscv-openocd
./bootstrap
./configure --prefix=$HOME/tools/riscv-openocd
make CFLAGS=-Wno-calloc-transposed-args -j4 
make install
```

# Install riscv toolchain

```
sudo dnf install binutils-riscv64-linux-gnu gcc-c++-riscv64-linux-gnu gcc-riscv64-linux-gnu
```

# Usage

Set up user and udev rules
```
sudo usermod -aG dialout dev
cat 'ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6010", MODE="0660", GROUP="dialout", ENV{ID_MM_DEVICE_IGNORE}="1", ENV{ID_MM_PORT_IGNORE}="1"' | sudo tee /etc/udev/rules.d/99-sipeed-jtag.rules
sudo udevadm control --reload-rules && sudo udevadm trigger
```

```
export OPENOCD_PATH=$HOME/tools/riscv-openocd
export SCRIPT_PATH=$HOME/git/fromzero/samples
git clone https://github.com/Dhole/fromzero
cd fromzero
git submodule update --init --recursive
```


Example 1


Only if riscv64-unknown-elf toolchain is not available:
```
alias make="make CROSS_COMPILE=$CROSS_COMPILE"
export CROSS_COMPILE=riscv64-linux-gnu-
```

```
cd samples/blink
make
../flash.sh build/code.bin
```

View the disassembly
```
riscv64-linux-gnu-objdump -d build/code.elf
```

Example 2

```
cd assembler
make main
./main test-led.asm test-led
../samples/flash.sh test-led
```
