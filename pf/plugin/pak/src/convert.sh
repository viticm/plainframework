#!/bin/sh

files=`find ./ -name "*.h" -o -name "*.cpp"`

for file in $files
do
  #touch $file-utf8 
  #iconv -f GBK -t UTF-8 $file > $file-utf8
  #mv $file-utf8 $file
  sed -i 's;\t;  ;g' $file
done
