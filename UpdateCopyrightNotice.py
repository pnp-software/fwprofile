__author__ = 'pnp'

#===============================================================================
# This program: 
# 1. Identifies the line in the input file where the copyright notice starts
# 2. Replaces the copyright notice with a new text.
#
# The program takes two arguments:
# - The name of the file to be updated
# - The name of the file with the new copyright notice
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
def removeCopyrightNotice(inpFileName, outFileName, licenceFileName):

    licenceFile = open(licenceFileName,'r')
    inFile = open(inpFileName,'r')
    outFile = open(outFileName,'w')
    skipLine = 0
    for line in inFile:
        if (skipLine == 0):
            outFile.write(line)
        if ('@copyright' in line):
            skipLine = 1
            # Insert content of licence file
            for licLine in licenceFile:
                outFile.write(licLine)
        if ((skipLine == 1) and ('*/' in line)):
            outFile.write(line)
            skipLine = 0 
            
    outFile.close()
    inFile.close()



#===============================================================================
def main(argv):

    # Remove the old copyright notice
    removeCopyrightNotice(sys.argv[1], tempFileName, sys.argv[2])
    
    # Replace the file with the old copyright notice with the file with the new 
    # copyright notice
    cmd = 'mv ' + tempFileName + ' ' + sys.argv[1]
    os.system(cmd)
  
if __name__ == "__main__":
    main(sys.argv[1:])
