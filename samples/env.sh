export CROSS_COMPILE=/mnt/data-ssd/dev/crosstool-ng/riscv32-unknown-elf/bin/riscv32-unknown-elf-
export OPENOCD_PATH=/mnt/data-ssd/dev/riscv-openocd
export SCRIPT_PATH=/mnt/data/git/fromzero/samples

alias make="make CROSS_COMPILE=$CROSS_COMPILE"
alias openocd="$OPENOCD_PATH/bin/openocd"
alias flash="$SCRIPT_PATH/flash.sh"
