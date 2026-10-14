#!/bin/bash

echo "  Replacing '$1' with '$2'"
FILES=`ls *.h *.t *.cc`
for i in $FILES ; \
do \
    mv -f $i $i.tmp ; \
    sed 's/'"$1"'/'"$2"'/g' $i.tmp > $i ; \
done
rm -f *.tmp */*.tmp
