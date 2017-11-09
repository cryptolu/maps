#!/bin/bash

script_path=$(dirname $0)
if [ '.' != $script_path ] ; then
    cd $script_path
fi

cd ..

find . -name "*~" -type f -delete
find . -name "*.o" -type f -delete

