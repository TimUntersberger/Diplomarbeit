#!/usr/bin/bash
echo "clang" >> .ccls
echo "-std=c99" >> .ccls
echo "-isystem./Mesh/main/include"
ls $IDF_PATH/components/*/include -d | sed "s/\(.*\)/-isystem\1/g" >> .ccls
