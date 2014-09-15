#!/bin/bash

#
# This is a tiny script to help you creating a CSV file from a face
# database with a similar hierarchie:
#
#  <datapath>
#  |-- name1
#  |   |-- 1.jpg
#  |   |-- ...
#  |   |-- 10.jpg
#  |-- name2
#  |   |-- 1.jpg
#  |   |-- ...
#  |   |-- 10.jpg
#  ...
#  |-- name10
#  |   |-- 1.jpg
#  |   |-- ...
#  |   |-- 10.jpg
#  |   |-- ...
#  ...
#
# Authorship
#  - Author:   David Qiu
#  - Email:    david@davidqiu.com
#  - Email:    dicong.qiu@intel.com
#  - Website:  http://www.davidqiu.com/
#
# Copyright (C) 2014, David Qiu.
#


pcount=0

if [ $# -lt 1 ]; then
  echo -e "\033[36mUsage: \"$0 <datapath>\".\033[0m"
  exit 1
fi

dir_data=${1%/}
for dir_person in "$dir_data"/*
do
  if [ -d "$dir_person" ];  then
    for img in "$dir_person"/*
    do
      if [ -f "$img" ]; then
        echo "$img;$pcount;`basename \"$dir_person\"`"
      fi
    done
    ((pcount=$pcount+1))
  fi
done


