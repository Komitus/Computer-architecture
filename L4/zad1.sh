#!bin/bash


net_interface=`ip link show | awk '/2:/ {print $2}'`

#echo $net_interface

unit_formatter(){
  
  numb=$1
  if [[  $numb -gt 1000000000 ]]; then
    numb=$(printf %.2f $(echo "$numb/1000000000" | bc -l))
    numb="$numb GB/s"
  elif [[ $numb -gt 1000000 ]]; then
    numb=$(printf %.2f $(echo "$numb/1000000" | bc -l))
    numb="$numb MB/s"
  elif [[ $numb -gt 1000 ]]; then
    numb=$(printf %.2f $(echo "$numb/1000" | bc -l))
    numb="$numb KB/s"
  else
    numb=$(printf %.2f $numb)
    numb="$numb B/s"
  fi

  echo $numb
}

prev_down=`cat /proc/net/dev | grep $net_interface | awk '{print $2}'`
prev_up=`cat /proc/net/dev | grep $net_interface | awk '{print $10}'`

net_Status () {

  download=`cat /proc/net/dev | grep $net_interface | awk '{print $2}'`  
  upload=`cat /proc/net/dev | grep $net_interface | awk '{print $10}'`
  
  #Counting difference between new and old values
  down_Growth=$(( download - prev_down ))
  up_Growth=$(( upload - prev_up ))
  
  if [[ "$max_down" -lt "$down_Growth" ]]; then
	max_down=$down_Growth 
	fi
	
	if [[ "$max_up" -lt "$up_Growth" ]]; then
	max_up=$up_Growth 
	fi
	
  prev_down=$download
  prev_up=$upload

  avg_down=$(( (avg_down*tests_numb + down_Growth)/(tests_numb+1) ))
  avg_up=$(( (avg_up*tests_numb + up_Growth)/(tests_numb+1) ))
  tests_numb=$(( tests_numb+1 ))
  
 
  

  #echo Receive : $(unit_formatter $down_Growth)
  #echo Transmitt : $(unit_formatter $up_Growth)
  #echo Avg Receive : $(unit_formatter $avg_down)
  #echo Avg Transmitt : $(unit_formatter $avg_up)
}



up_Time () {

  uptime=`cat /proc/uptime | awk '{print $1}'` 


  
  uptime=$( printf %.0f $uptime )

  day=$(( uptime/(24*3600) ))
  uptime=$(( uptime%(24*3600) ))
  hour=$(( uptime/3600 ))
  uptime=$(( uptime%3600 ))
  minute=$(( uptime/60 ))
  uptime=$(( uptime%60 ))
  second=$uptime

  if [ $day -eq 1 ]
  then
    day="$day day"
  else
    day="$day days"
  fi

  if [ $hour -eq 1 ]
  then
    hour="$hour hour"
  else
    hour="$hour hours"
  fi

  if [ $minute -eq 1 ]
  then
    minute="$minute minute"
  else
    minute="$minute minutes"
  fi

  if [ $second -eq 1 ]
  then
    second="$second second"
  else
    second="$second seconds"
  fi


  echo Uptime : $day, $hour, $minute and $second
}

avg_Load () {

  load=`cat /proc/loadavg | awk '{print $1}'`

  echo "System Avg load: $load"

}

disp_bar(){  # Two arguments: 1) percentage to display 2) color of the bar
max=`expr $1/1000000` 
fgc=$2
bgc=$3
for (( i = 0 ; i < $max ; i++ ))
do
       echo -n "$(tput dim)$(tput bold)$(tput setab $3)$(tput setaf $2)|"  
        
done
}



echo Network on: $net_interface




while(true)
do

net_Status;

echo -n "Download :"
disp_bar $down_Growth 4 7
echo "$(tput sgr 0) $(unit_formatter $down_Growth)"

echo -n "Avg_Down :"
disp_bar $avg_down 2 7
echo "$(tput sgr 0) $(unit_formatter $avg_down)"

echo -n "Max_Down :"
disp_bar $max_down 1 7
echo "$(tput sgr 0) $(unit_formatter $max_down)"

#upload
echo -n "Upload   :"
disp_bar $up_Growth 4 6
echo "$(tput sgr 0) $(unit_formatter $up_Growth)"

echo -n "Avg_Up   :"
disp_bar $avg_up 2 6
echo "$(tput sgr 0) $(unit_formatter $avg_up)"

echo -n "Max_Up   :"
disp_bar $max_up 1 6
echo "$(tput sgr 0) $(unit_formatter $max_up)"

up_Time
avg_Load



sleep 1
clear 
done






























