#!/bin/bash
# This script changes the licence statement in all the source files.
# The script should be run twice: once on the *.c files and once on the *.h files.
#====================================================================================

FILES=./*/*.h   # Must also be run on the *.c files

for f in $FILES
do
    echo "Processing $f file..."
    
    sed -i 's/This file is part of FW Profile/This file is part of the FW Profile/g' $f
    sed -i 's/FW Profile is free software: you can redistribute it and\/or modify/This Source Code Form is subject to the terms of the Mozilla Public/g' $f
    sed -i 's/it under the terms of the GNU Lesser General Public License as published by/License, v. 2.0. If a copy of the MPL was not distributed with this/g' $f
    sed -i 's/the Free Software Foundation, either version 3 of the License, or/file, You can obtain one at http:\/\/mozilla.org\/MPL\/2.0\/./g' $f

    # Remove lines which are no longer needed
    sed -i '/(at your option) any later version./d' $f 
    sed -i '/FW Profile is distributed in the hope that it will be useful,/d' $f 
    sed -i '/but WITHOUT ANY WARRANTY; without even the implied warranty of/d' $f 
    sed -i '/MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the/d' $f 
    sed -i '/GNU Lesser General Public License for more details./d' $f 
    sed -i '/You should have received a copy of the GNU Lesser General Public License/d' $f 
    sed -i '/along with FW Profile.  If not, see <http:\/\/www.gnu.org\/licenses\/>./d' $f 
    
    # The previous steps create situation where there are three consecutive blank
    # comment lines. The next command will remove them.
    python RemoveTripleBlankCommentLines.py $f

done

