export CROSS_COMPILE=riscv64-linux-gnu-
export OPENOCD_PATH=/home/dev/tools/riscv-openocd
export SCRIPT_PATH=/home/dev/git/fromzero/samples

alias make="make CROSS_COMPILE=$CROSS_COMPILE"
alias openocd="$OPENOCD_PATH/bin/openocd"
alias flash="$SCRIPT_PATH/flash.sh"
