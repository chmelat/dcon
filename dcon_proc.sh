#!/bin/sh

# vylistuj vsechny procesy (BSD syntax) a vyber ...
ps aux | grep '[U]SER'
ps aux | grep '[d]con'  
ps aux | grep '[d]con_start'

