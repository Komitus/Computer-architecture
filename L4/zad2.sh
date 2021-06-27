#!/bin/bash

cd /proc

printf "%-30s" "Name"
printf "%-10s" "Pid"
printf "%-10s" "PPid" 
printf "%-15s" "Status"
printf "%-10s \n \n" "Files_Opened"

for file in *
do
  if  [[ $file =~ ^[0-9]+$ ]];
  then
    cd $file
    if [ -e status ]
    then
      name=$(cat status | awk 'FNR == 1 {print$2; exit}')
      pid=$(cat status | awk 'FNR == 6 {print$2; exit}')
      ppid=$(cat status | awk 'FNR == 7 {print$2; exit}')
      status=$(cat status | awk 'FNR == 3 {print $3; exit}')
			
      if [ -r 'fd' ]    #if exists and we have permission to open it
      then
        o_files=$( ls fd | wc -l )
      else
        o_files='0'
      fi
	
      printf "%-30s" $name 
      printf "%-10s" $pid 
      printf "%-10s" $ppid 
      printf "%-15s" $status 
      printf "%-10s \n" $o_files

      cd ..
    fi
  fi
done
