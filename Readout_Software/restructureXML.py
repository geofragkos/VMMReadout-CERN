#!/bin/env python

import subprocess
import sys


xml_file = "./configs/config.xml"

to_replace = ["ch.polarity",
                "ch.leakage.current",
                "analog.tristates",
                "double.leakage",
                "peak.time",
                "neighbor.trigger",
                "TAC.slop.adj",
                "disable.at.peak",
                "ART.mode",
                "dual.clock.ART",
                "out.buffer.mo",
                "out.buffer.pdo",
                "out.buffer.tdo",
                "channel.monitoring",
                "monitoring.control",
                "monitor.pdo.out",
                "sub.hyst.discr",
                "direct.time",
                "direct.time.mode",
                "conv.mode.8bit",
                "enable.6bit",
                "ADC.10bit",
                "ADC.8bit",
                "ADC.6bit",
                "dual.clock.data",
                "dual.clock.6bit",
                "threshold.DAC",
                "test.pulse.DAC",
                "leakage.current",
                "test.pulse",
                "hidden.mode",
                "monitor.mode",
                "s10bADC.time.set",
                "s08bADC.time.set",
                "s06bADC.time.set"]


def main() :
    n_replaced = 0
    ofile = open("config_new.xml", 'w')

    lines = open(xml_file).readlines()
    for line in lines :
        if not line : continue
        #line = line.strip()
        for r in to_replace :
            if r in line :
                new_r = r.replace(".", "_")
                line = line.replace(r, new_r)
                n_replaced += 1
        ofile.write(line)
    ofile.close()
    print "replaced %d of expected %d"%(n_replaced, len(to_replace))
                
                


if __name__=="__main__" :
    main()
