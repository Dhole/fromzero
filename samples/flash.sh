#!/bin/sh

set -e

FILE=$1
if [ "$FILE" = "" ]; then
    echo "Usage: $0 FILE"
    exit 1
fi

# $OPENOCD_PATH/bin/openocd -f $SCRIPT_PATH/sipeed-jtag.cfg -f $SCRIPT_PATH/openocd.cfg -c "program $PWD/$FILE reset exit 0x8000000"

# DBG_KEY=0xe004200c
# DBG_KEY_UNLOCK=0x4b5a6978
# DBG_CMD=0xe0042008
# DBG_CMD_REST=0x01
# Reset procedure:
#       [DBG_KEY] = DBG_KEY_UNLOCK
#       [DBG_CMD] = DBG_CMD_RESET

# $OPENOCD_PATH/bin/openocd -f $SCRIPT_PATH/openocd-gdlink.cfg -c init -c "flash protect 0 0 last off; program  $PWD/$FILE verify 0x8000000; mww 0xe004200c 0x4b5a6978; mww 0xe0042008 0x01; resume; exit;"

$OPENOCD_PATH/bin/openocd -f $SCRIPT_PATH/openocd-gdlink.cfg -c init -c "flash protect 0 0 last off; program  $PWD/$FILE verify reset 0x8000000; mww 0xe004200c 0x4b5a6978; mww 0xe0042008 0x01; resume; exit;"
