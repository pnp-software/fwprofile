__author__ = 'pnp'

#===============================================================================
# This program: 
# 1. Identifies situations where there are three consecutive lines like this:
# *
# *
# *
#    and it removes two of the three lines
#
# The program takes one argument:
# - The name of the file to be updated
#
#===============================================================================

import sys
import os
import argparse
import shutil
import csv
import pdb

# Temporary file where the updated input is placed
tempFileName = 'temp.txt'

#===============================================================================
# Remove the old copyright notice and insert the new copyright text
def removeTripleBlankLine(inpFileName, outFileName):

    inFile = open(inpFileName,'r')
    outFile = open(outFileName,'w')
    skipLine = 0
    line1 = ""
    line2 = ""
    line3 = ""
    for line in inFile:
        line1 = line2
        line2 = line3
        line3 = line
        
        if ((line1 == " *\n") and (line2 == " *\n") and (line3 == " *\n")):
            outFile.write(line1)
            skipLine = 3
        else:   
            if (skipLine == 0):
                outFile.write(line1)   

        if (skipLine > 0):
            skipLine = skipLine - 1
            
    outFile.write(line2)
    outFile.write(line3)           
    outFile.close()
    inFile.close()



#===============================================================================
def main(argv):

    # Remove a sequence of three blank comment lines
    removeTripleBlankLine(sys.argv[1], tempFileName)
    
    # Replace the input file with the output file
    cmd = 'mv ' + tempFileName + ' ' + sys.argv[1]
    os.system(cmd)
  
if __name__ == "__main__":
    main(sys.argv[1:])
