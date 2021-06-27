#!/bin/bash


USR_DIR=$1

#if [ -z "$USR_DRI" ] || [[ -d "$USR_DRI" ]]
#then
#    USR_DIR="$(pwd)" 
#fi

echo $USR_DIR


for FILE in "$USR_DIR/"*
   do
 
           tmp_name=$(basename "$FILE")
	   #echo $tmp_name
           name="$(echo $tmp_name |  tr  '[:upper:]' '[:lower:]' )"
           END_NAME="${USR_DIR}/${name}"
           mv -n "$FILE" "$END_NAME"
           echo "$tmp_name ----> $name"      
         
   done
        
       


