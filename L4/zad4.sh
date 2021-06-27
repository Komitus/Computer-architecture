#!bin/bash
if [ $# -ne 2 ]; then
  echo "Type first argument url, second time"
  exit 1
fi
#^[0-9]+$ will match a non-empty contiguous string of digits

if ! [[ $2 =~ ^[0-9]+$ ]]; then
  echo Second argument shoul be a number >&2
  exit 1
fi

addr=$1
time=$2

dir_name=$(sed "s/\/\|\.//g" <<< $addr)
dir_name=$(sed "s/https://" <<< $dir_name)
echo $dir_name

path=$HOME/Desktop/webTrack/$dir_name
#echo $path
mkdir $path 2>/dev/null
git init -q $path 
rm -f $path/current
touch $path/current

cd $path
while [ true ]; 
do
  if ! [ "$(curl -s -I -f $addr)" ];then    # -I only head -s silently -f return true if fails
    echo Site is not working >&2
    exit 1
  fi
  
  
	links -dump $addr > new
  if [ "$(diff new current)" ]; then
  	
  	# we do commits when changes appear, we can se logs in $PWD/.git/logs/refs/heads/master
  	git add $path/current   >/dev/null 2>&1 
    git commit -m 'new revision'  >/dev/null 2>&1
    diff new current | zenity --text-info --title="$addr got changed!" 
    mv new current
   
    
  fi
	cat < $PWD/.git/logs/refs/heads/master
  sleep $time
  clear
done


