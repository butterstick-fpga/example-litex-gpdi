# ButterStick GPDI LiteX demo

A LiteX project which builds a SoC with DRAM / HDIM output via the GPDI SYZYGY addon.

1. Connect GPDI board to SYZYGY.PORTA
2. Connect HDMI Cable to the right-hand port
3. Build LiteX project and load bitstream, alt=0
4. Load tux image into FLASH, alt=1

```
dfu-util -D build/butterstick_r1d0/gateware/butterstick_r1d0.dfu --alt 0
dfu-util --device 1209:5af1 --alt 1 -D ../firmware/tux.data --reset
```
