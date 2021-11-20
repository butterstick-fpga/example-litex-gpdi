# This file is Copyright (c) Greg Davill <greg.davill@gmail.com>
# License: BSD

from litex.build.generic_platform import *
from litex.build.lattice import LatticePlatform

_syzygy_gpdi = [

    ("hdmi_out", 0,
        Subsignal("clk_p",       Pins("SYZYGY0:S29"), IOStandard("LVCMOS33D")),
        #Subsignal("clk_n",       Pins("SYZYGY0:S31"), IOStandard("LVCMOS33D")),
        Subsignal("data0_p",     Pins("SYZYGY0:S1"),  IOStandard("LVCMOS33D")),
        #Subsignal("data0_n",     Pins("SYZYGY0:S3"),  IOStandard("LVCMOS33D")),
        Subsignal("data1_p",     Pins("SYZYGY0:S5"),  IOStandard("LVCMOS33D")),
        #Subsignal("data1_n",     Pins("SYZYGY0:S7"),  IOStandard("LVCMOS33D")),
        Subsignal("data2_p",     Pins("SYZYGY0:S9"),  IOStandard("LVCMOS33D")),
        #Subsignal("data2_n",     Pins("SYZYGY0:S11"), IOStandard("LVCMOS33D")),
        Misc("DRIVE=8"), Misc("SLEWRATE=FAST"),
    ),
]