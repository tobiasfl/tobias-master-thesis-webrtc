#!/bin/sh

#extracts timestamp and rate from lines on the form:
#<random chromium stuff> TOBIAS <timestamp> ms <rate> kbps
awk /TOBIAS/ log.txt | awk {'print $3"\t"$5'} > log_tobias_extracted.txt
