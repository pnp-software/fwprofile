#!/bin/bash
# This script creates a release package for the C1 Implementation of the FW Profile.
#====================================================================================

CC=gcc

OUT="./zip"
OUT_DOCS="${OUT}/docs/"
OUT_LOG="${OUT}/log/"
OUT_SRC="${OUT}/src/"

make clean
rm -fr $OUT
mkdir -p $OUT
mkdir -p $OUT/examples
mkdir -p $OUT/examples/src
mkdir -p $OUT/tests
mkdir -p $OUT_DOCS
mkdir -p $OUT_LOG
mkdir -p $OUT_SRC

if [ "$1" == "" ] || [ "$2" == "" ]
then
    echo "Release.sh: missing file operand"
    echo "Try 'Release.sh' for more information."
    exit 1
fi

if [ "$1" == "--help" ] || [ "$1" == "-h" ]
then
    echo "Usage: Release.sh VERSION EXAMPLE_PATH [FWPROFILE_PATH]"
    echo ""
    echo "Performs release activities with examples taken from"
    echo "EXAMPLE_PATH. FWPROFILE_PATH defaults to current directory."
    exit 0
fi

# Path to FwProfile examples. Must be given.
VERSION=$1
EXAMPLE_PATH=$2

# FwProfile Path
if [ "$3" == "" ]
then
    FW_PATH="."
else
    FW_PATH=$3
fi

FW_SRC_DOC="${FW_PATH}/doc"
FW_SRC="${FW_PATH}/src"
DOXYFILE="C1Impl_TestSuite_Demo.doxyfile"

# ====================================================================================   
echo "================================================"
echo "PDF Generation - FW Profile Definition"
(cd ${FW_SRC_DOC}/fwprofile &&
 pdflatex -shell-escape -interaction=nonstopmode -halt-on-error FWProfile.tex > latex_fwprofile.log &&
 pdflatex -shell-escape -interaction=nonstopmode -halt-on-error FWProfile.tex > latex_fwprofile.log)
cp ${FW_SRC_DOC}/fwprofile/FWProfile.pdf ${OUT_DOCS}
cp ${FW_SRC_DOC}/fwprofile/latex_fwprofile.log ${OUT_LOG}

# ====================================================================================
echo "PDF Generation - FW Profile User Manual"
(cd ${FW_SRC_DOC}/um &&
 pdflatex -shell-escape -interaction=nonstopmode -halt-on-error UserManual.tex > latex_user_manual.log &&
 pdflatex -shell-escape -interaction=nonstopmode -halt-on-error UserManual.tex > latex_user_manual.log)

cp ${FW_SRC_DOC}/um/UserManual.pdf ${OUT_DOCS}
cp ${FW_SRC_DOC}/um/latex_user_manual.log ${OUT_LOG}

# ====================================================================================
echo "PDF Generation - FW Profile FAQ"
(cd ${FW_SRC_DOC}/faq &&
 pdflatex -shell-escape -interaction=nonstopmode -halt-on-error FAQ.tex > latex_faq.log &&
 pdflatex -shell-escape -interaction=nonstopmode -halt-on-error FAQ.tex > latex_faq.log)

cp ${FW_SRC_DOC}/faq/FAQ.pdf ${OUT_DOCS}
cp ${FW_SRC_DOC}/faq/latex_faq.log ${OUT_LOG}	

# ====================================================================================
echo "PDF Generation - FW Profile User Requirements"
(cd ${FW_SRC_DOC}/req &&
 pdflatex -shell-escape -interaction=nonstopmode -halt-on-error UserRequirements.tex > latex_user_requirements.log &&
 pdflatex -shell-escape -interaction=nonstopmode -halt-on-error UserRequirements.tex > latex_user_requirements.log)

cp ${FW_SRC_DOC}/req/UserRequirements.pdf ${OUT_DOCS}
cp ${FW_SRC_DOC}/req/latex_user_requirements.log ${OUT_LOG}

# ====================================================================================
echo "Create Doxygen Documentation"
(cd ${FW_SRC_DOC}/doxygen &&
	doxygen $DOXYFILE > doxygen_generation.log)
cp -ar ${FW_SRC_DOC}/doxygen/html ${OUT_DOCS}/doxygen

# ====================================================================================
echo "Building the application and testsuite"
(make clean;
 make coverage && make test && make run-test && make coverage-info)

echo "Running valgrind..."
valgrind --leak-check=yes ./bin/testsuite > ${OUT_LOG}/TestSuite_Valgrind_Report.txt 2>&1

#echo "===================================================================================="
echo "Extract the non-covered parts of the C1 Implementation code"
#echo "===================================================================================="
egrep -B 6 "####| 0%" *.c.gcov >> ${OUT_LOG}/CodeCoverage_Report.txt 2>&1

#echo "===================================================================================="
echo "Compile & build the Demo Application and tutorials"
#echo "===================================================================================="
( cd ${EXAMPLE_PATH};
  make)

#echo "===================================================================================="
echo "Run the Demo Application and tutorials with Valgrind"
#echo "===================================================================================="
valgrind --leak-check=yes ${EXAMPLE_PATH}/bin/sm_demo > ${OUT_LOG}/SmApp_Valgrind_Report.txt 2>&1
valgrind --leak-check=yes ${EXAMPLE_PATH}/bin/PrExample1 > ${OUT_LOG}/PR_Example1_Valgrind_Report.txt 2>&1
valgrind --leak-check=yes ${EXAMPLE_PATH}/bin/RtExample1 > ${OUT_LOG}/RT_Example1_Valgrind_Report.txt 2>&1
valgrind --leak-check=yes ${EXAMPLE_PATH}/bin/SmExample1 > ${OUT_LOG}/SM_Example1_Valgrind_Report.txt 2>&1
valgrind --leak-check=yes ${EXAMPLE_PATH}/bin/SmExample2 > ${OUT_LOG}/SM_Example2_Valgrind_Report.txt 2>&1
valgrind --leak-check=yes ${EXAMPLE_PATH}/bin/SmExample3 > ${OUT_LOG}/SM_Example3_Valgrind_Report.txt 2>&1

#echo "===================================================================================="
echo "Add Header to Generated Test Reports"
#echo "===================================================================================="
H_L1="===================================================================================\n"
H_CC1="This is the Code Coverage Report of the FW Profile\n"
H_ML1="This is the Memory Leak Report generated by Valgrind running the Test Suite\n"
H_ML2="This is the Memory Leak Report generated by Valgrind running the Demo Application\n"
H_ML3="This is the Memory Leak Report generated by Valgrind running the SM Example 1\n"
H_ML4="This is the Memory Leak Report generated by Valgrind running the SM Example 2\n"
H_ML5="This is the Memory Leak Report generated by Valgrind running the SM Example 3\n"
H_ML6="This is the Memory Leak Report generated by Valgrind running the PR Example 1\n"
H_DATE="Generated: `date`\n"
H_CP="Copyright: P&P Software GmbH, 2013, All Rights Reserved\n"
H_VER="FW Profile Version: todo\n"

sed -i -e "1i${H_L1}${H_ML1}${H_DATE}${H_CP}${H_VER}${H_L1}" ${OUT_LOG}/TestSuite_Valgrind_Report.txt
sed -i -e "1i${H_L1}${H_CC1}${H_DATE}${H_CP}${H_VER}${H_L1}" ${OUT_LOG}/CodeCoverage_Report.txt
sed -i -e "1i${H_L1}${H_ML2}${H_DATE}${H_CP}${H_VER}${H_L1}" ${OUT_LOG}/SmApp_Valgrind_Report.txt
sed -i -e "1i${H_L1}${H_ML6}${H_DATE}${H_CP}${H_VER}${H_L1}" ${OUT_LOG}/PR_Example1_Valgrind_Report.txt
sed -i -e "1i${H_L1}${H_ML6}${H_DATE}${H_CP}${H_VER}${H_L1}" ${OUT_LOG}/RT_Example1_Valgrind_Report.txt
sed -i -e "1i${H_L1}${H_ML3}${H_DATE}${H_CP}${H_VER}${H_L1}" ${OUT_LOG}/SM_Example1_Valgrind_Report.txt
sed -i -e "1i${H_L1}${H_ML4}${H_DATE}${H_CP}${H_VER}${H_L1}" ${OUT_LOG}/SM_Example2_Valgrind_Report.txt
sed -i -e "1i${H_L1}${H_ML5}${H_DATE}${H_CP}${H_VER}${H_L1}" ${OUT_LOG}/SM_Example3_Valgrind_Report.txt

# ====================================================================================
echo "================================================"
echo "Create Release Package"
echo "================================================"
echo ""

cp -a ${FW_SRC_DOC}/commercial/README ${OUT}
cp -a ${FW_SRC_DOC}/commercial/COPYING ${OUT}
cp -a ${FW_PATH}/Makefile ${OUT}
cp -ar ${FW_PATH}/src ${OUT}
cp -ar ${FW_PATH}/tests ${OUT}
cp -ar ${EXAMPLE_PATH}/src/app ${OUT}/examples/src/app
cp -ar ${EXAMPLE_PATH}/src/pr_tutorials ${OUT}/examples/src/pr_tutorials
cp -ar ${EXAMPLE_PATH}/src/rt_tutorials ${OUT}/examples/src/rt_tutorials
cp -ar ${EXAMPLE_PATH}/src/sm_tutorials ${OUT}/examples/src/sm_tutorials
sed "2s/.*/C1_DIR = ..\/src/" ${EXAMPLE_PATH}/Makefile > ${OUT}/examples/Makefile

( cd ${OUT};
  zip -r ./FWProfile_C1_Impl_${VERSION}_LGPLv3.zip .)

#END

