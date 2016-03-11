#!/usr/bin/python

import sys
from os import listdir
from os.path import isfile, join

def studyfile(filename):
   avgBlack = [0.0, 0.0]

   f = open(filename, 'r')
   for i in range(0, 13):	
      f.readline()  

   i = 0.0
   for line in f:
      s = line.rstrip('\n').split('\t')
      values = s[1][6:-1].split(',')
      avgBlack[0] += float(values[0])
      avgBlack[1] += float(values[1])
      i += 1.0

   avgBlack[0] /= i
   avgBlack[1] /= i
   
   if avgBlack[0] > 98 and avgBlack[1] > 0 and avgBlack[1] < 80:
   	print filename, '\t', "{:6.3f}".format(avgBlack[0]),'%\t', "{:6.3f}".format(avgBlack[1]), '%'  
 
   f.close()

def studydir(dirpath):
   onlyfiles = [f for f in listdir(dirpath) if isfile(join(dirpath, f))]
   for filename in onlyfiles:
	studyfile(dirname+filename)
      
if len(sys.argv) != 2:
   exit
   
dirname = sys.argv[1]
print sys.argv[1]
studydir(dirname)
