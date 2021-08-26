#!/bin/sh

#extracts timestamp and rate from lines on the form:
#<random chromium stuff> TOBIAS <timestamp in ms> <rate in kbps>

awk /TOBIAS/ log.txt | awk {'print $3"\t"$4'} > log_tobias_extracted.txt
