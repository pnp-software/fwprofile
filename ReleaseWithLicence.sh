#!/bin/bash
# This script takes a delivery file created for the LGPL licence and creates
# a delivery file for a commercial licence.
# The output delivery file is created in the directory where the script is run.
#
# Arguments:
# - The name of the entity for which the delivery is prepared
# - The path to the input delivery file  
# - The file with the licence text
# - The path to the directory where the new delivery file is prepared 
# - The version number (e.g. 1.3.1)
# 
# This script performs the following actions:
# 1. Creates a 'Release' sub-directory in the target directory   
# 2. Unzip the delivery file in the Release directory
# 3. Remove files with references to LGPL
# 4. Update the copyright notice in all source files
# 5. Create zip file with the updated files
#
#====================================================================================

NAME=$1
INPUT_DEL_FILE=$2
LICENCE_FILE=$3
TARGET_DIR=$4
VERSION=$5

# Set Paths
FW_SRC_SM="./src"
FW_SRC_SM_TEST="./tests"
FW_SRC_SM_EXAMPLES_APP="./examples/src/app"

CUR_DIR=$PWD
cd ${TARGET_DIR}
mkdir Release
cd Release

echo "================================================"
echo "Unzip the Input Delivery File"
unzip ${INPUT_DEL_FILE} -d .

echo "================================================"
echo "Remove Files with References to LGPL"
rm COPYING
rm README

echo "================================================"
echo "Update the Copyright Notice"

# FwProfile source code, test cases, demo application, sm example tutorials, pr example tutorials
for i in ${FW_SRC_SM}/*.c ${FW_SRC_SM}/*.h \
         ${FW_SRC_SM_TEST}/*.c ${FW_SRC_SM_TEST}/*.h  
do
  python $CUR_DIR/UpdateCopyrightNotice.py $i ${LICENCE_FILE}
done

echo "================================================"
echo "Create the new delivery file"
zip -r FWProfile_C1_Impl_${VERSION}_${NAME}.zip . 



