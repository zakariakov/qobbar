#!/bin/bash 
# Zakaria Barkouk ( Zakaria.gatter@gmail.com)
export LANG=en_US
export LANGUAGE=en_US

# Show CPU Info - 1 (Mpstat)
_CPU_ () {

# [ -z "$1" ] && icon="" || icon="$1"
#cpu_usage=$(mpstat | awk '{print $3}' );
# cpu_usage=$(mpstat -P ALL | awk '/all/{print $4}' | cut -d. -f1 );
cpu_usage=$(top -b -n2 -p 1 \
| fgrep "Cpu(s)" \
| tail -1 \
| awk -F'id,' -v prefix="$prefix" '{ split($1, vs, ","); \
v=vs[length(vs)]; sub("%", "", v); \
printf "%s%.1f%%\n", prefix, 100 - v }')

#echo "$icon $cpu_usage%"
echo "$cpu_usage"
}

# Show Multi Cpu Info - 2 (Mpstat)
_CPUS_ () {

[ -z "$1" ] && icon="" || icon="$1"

cpus_usage=$(mpstat -P ALL \
| cut -d " " -f3- \
| column -t \
| awk '/^[0-9]/{print $2"%"}' \
| tr "\n" " ");
echo "$icon $cpus_usage"
}

# Show Memory Usage - 3 (Free)
_MEM_ () {

[ -z "$1" ] && icon="" || icon="$1"

mem_usage=$(free -h | awk '/^Mem:/{print $3}');
#echo "$icon $mem_usage"
echo "$mem_usage"
}

# SHOW SWAP USAGE - 4 (FREE)
_SWAP_ () {

[ -z "$1" ] && icon=" " || icon="$1"

swap_usage=$(free -h | awk '/^Swap:/{print $3}');
echo "$icon $swap_usage"
}



# Show Date and Time - 5 (Date)
_DATE_ () {

[ -z "$1" ] && icon="" || icon="$1"

date_time=$(date +%D\ %R);
echo "$icon $date_time"
}

# Show Battery Status - 6 (acpi)
_BAT_ () {
Bat=$(acpi | awk '{print $4}'| tr -d "%,");
adapt=$(acpi -a | awk '{print $3}');

#if [ "$adapt" = "on-line" ];then
#icon0=""
#icon1=""
#icon2=""
#icon3=""
#icon4=""
#else
#icon0=""
#icon1=""
#icon2=""
#icon3=""
#icon4=""
#fi



#bat="$icon4 $Bat"
#if  [[ $Bat -ge 100 ]] ; then
#bat="$icon4  Full"
#elif [[ $Bat -ge 80 ]] && [[ $Bat -lt 100 ]]; then
#bat="$icon4 $Bat%"
#elif [[ $Bat -ge 60 ]] && [[ $Bat -lt 80 ]]; then
#bat="$icon3 $Bat%"
#elif [[ $Bat -ge 40 ]] && [[ $Bat -lt 60 ]]; then
#bat="$icon2$Bat%"
#elif [[ $Bat -ge 20 ]] && [[ $Bat -lt 40 ]]; then
#bat="$icon1$Bat%"
#else
#bat="$icon0$Bat%"
#fi

if [ "$adapt" = "on-line" ];then
icon=""
else
icon=""
fi
echo "$icon$Bat%"

}

# Show Disk Usage - 7 (df)
_DISK_ () {

[ -z "$2" ] && icon="" || icon="$2"

disk_usage=$(df -h "$1" | awk '/\/dev\//{print $3-G"/"$2}');
# echo "$icon $disk_usage"
echo "$disk_usage"
}

# Show Keyboard Layout - 8
_KEY_ () {

[ -z "$1" ] && icon="" || icon="$1"

key_layout=$(cat /etc/default/keyboard \
| awk -F= '/XKBLAYOUT/{print $2}'\
| tr -d '"');
echo "$icon $key_layout"
}

#Show System language - 9
_LANG_ () {

[ -z "$1" ] && icon="" || icon="$1"

echo "$icon $LANGUAGE"
}

#Show Machine Uptime - 10 (uptime)
_UPTIME_ () {

[ -z "$1" ] && icon="" || icon="$1"

uptime=$(uptime -p \
| sed -E 's/ hour/H/g; s/ hours/H/g; s/ minutes/M/g; s/ minute/M/g; s/ day/D/g; s/ days/D/; s/up //g')

echo "$icon $uptime"
}

#Show Trash Size - 11 (du)
_TRASH_ () {

[ -z "$1" ] && icon="" || icon="$1"

TRASH_FOLDER="$HOME/.local/share/Trash/files"

SIZE=$(du -h $TRASH_FOLDER | awk '{print $1}');

echo  "$icon $SIZE"

}

#Show Window Usage - 12 (xdotool)
_WINDOW_ () {

[ -z "$1" ] && icon="" || icon="$1"
[ -z "$2" ] && CAR="150" || CAR="$2"

focus=$(xdotool getactivewindow getwindowname)
focus_Number=$(xdotool getactivewindow getwindowname | wc -c)
Focus_N=$(xdotool getactivewindow getwindowname | head -c $CAR )

#if [ -z "$focus" ];then
#echo "$icon Welcome"
#else
#[ "$focus_Number" -gt "$CAR" ] && echo "$icon $Focus_N ..." || echo "$icon $focus"
#fi

if [ -z "$focus" ];then
echo "Welcome"
else
[ "$focus_Number" -gt " $CAR" ] && echo " $Focus_N ..." || echo " $focus"
fi
}

#Show Mocp playing song and status - 13 (moc)
_MOCP_ () {

[ -z "$1" ] && icon="" || icon="$1"

Title=$(mocp -Q %title)
F_Title=$(mocp -Q %file)
Status=$(mocp -Q %state)


if [ "$Status" != "PLAY" ];then 
echo "$icon Pause"
elif [ -z "$Title" ];then 
echo "$icon $(basename $F_Title)"
else
echo "$icon $Title"
fi
}

#Show mpris Playing song and status - 14 (mpris; mpris-ctl)
_MPRIS_ () {


[ -z "$1" ] && icon="" || icon="$1"

icon1=
icon2=
# NCMP=$(mpc status | awk '/^\[playing\]/{print $1}')
# _NCMP=$(mpc | head -1 )

# if [ "$NCMP" = "[playing]" ];then 
#     echo "$icon2"
# else
#     echo "$icon1"
# fi

NCMP=$(playerctl -p goldfinch status)
#echo $NCMP
if [ "$NCMP" = "Playing" ];then 
echo "$icon2 "
elif [ "$NCMP" = "يشتغل" ];then	
echo "$icon2 "
else
echo "$icon1"
fi

}

#Show Cpu Temp - 15 (xsenser)
_TEMP_ () {

[ -z "$1" ] && icon="" || icon="$1"

temp=$(sensors | awk '/^temp1/{print $2}' | tr -d "+°C"  | cut -d. -f1 )

echo " $temp °C"
if [ "$temp" -gt "75" ];then
echo "<span style=' color:#FF0000;'> $temp °C</span> "
elif [ "$temp" -gt "65" ];then
echo " <span style=' color:#FF4D00;'> $temp °C</span>"
elif [ "$temp" -gt "30" ];then
echo " $temp °C"
fi

}

#Show Gpu Temp - 16 (xsenser)
_GPU_TEMP_ () {

[ -z "$1" ] && icon="" || icon="$1"

gpu_temp=$(sensors | awk '/^GPU/{print $2}' | tr -d "+" );

echo "$icon $gpu_temp"
}

#Show Touchpad status - 17 (sysclient)
_TOUCHPAD_ () {

[ -z "$1" ] && icon="" || icon="$1"

Status=$(synclient -l | awk '/Touchpad/{print $3}')

[ "$Status" = "0" ] && echo "$icon Enable" || echo "$icon Disable"

}

#Show Volume Status - 18 (alsa)
_VOLUME_ () {
Vol=$(amixer -c 0 get Master | awk '/Mono:/{print $4}' | tr -d "[ %]")
Mute=$(amixer -c 0 get Master | awk '/Mono:/{print $6}' | tr -d "[-]")

if [ "$Mute" = "off" ];then 
echo -e " Mute"
else
#if [ "$Vol" -gt "80" ];then
#echo -e " $Vol%"
#elif [ "$Vol" -gt "60" ];then
#echo -e " $Vol%"
#elif [ "$Vol" -gt "40" ];then
#echo -e " $Vol%"
#elif [ "$Vol" -gt "20" ];then
#echo -e " $Vol%"
#elif [ "$Vol" -eq "0" ];then
#echo -e " $Vol%"
#fi
echo -e "$Vol%"
fi
}

#Show Wifi info ; ip ; connected name - 19 (ifconfig; nmcli)
_WIFI_ () {

#[ -z "$1" ] && icon="" || icon="$1"


#(nmcli d | awk '/wifi/{print $1}') | while read W ; do 
#W_C=$(nmcli d | awk '/'"$W"'/{print $3}');
#W_N=$(nmcli d | awk '/'"$W"'/{print $4}');
#W_IP=$(ifconfig "$W" | awk '/inet addr/{print $2}' | cut -d: -f2);

#[ "$W_C" == "connected" ] && echo -n "$icon $W_IP ($W_N) " || echo -n "$icon --- "

#done
#QUALITY=$(iw dev $1 link | grep SSID | cut -d " " -f 2-)
#if  [  -z $QUALITY ]
#then
#echo  ""
#else
#echo   $QUALITY  # short text
#fi

if [[ -n $BLOCK_INSTANCE ]]; then
INTERFACE=$BLOCK_INSTANCE
else
INTERFACE=$(ip route | awk '/^default/ { print $5 ; exit }')
fi

# Issue #36 compliant.
if ! [ -e "/sys/class/net/${INTERFACE}/operstate" ] || ! [ "`cat /sys/class/net/${INTERFACE}/operstate`" = "up" ]
then
# echo "$INTERFACE "
echo "<span style=' color:#686869;'> $INTERFACE </span>"
exit 0
fi

# path to store the old results in
path="/dev/shm/$(basename $0)-${INTERFACE}"

# grabbing data for each adapter.
read rx < "/sys/class/net/${INTERFACE}/statistics/rx_bytes"
read tx < "/sys/class/net/${INTERFACE}/statistics/tx_bytes"

# get time
time=$(date +%s)

# write current data if file does not exist. Do not exit, this will cause
# problems if this file is sourced instead of executed as another process.
if ! [[ -f "${path}" ]]; then
echo "${time} ${rx} ${tx}" > "${path}"
chmod 0666 "${path}"
fi

# read previous state and update data storage
read old < "${path}"
echo "${time} ${rx} ${tx}" > "${path}"

# parse old data and calc time passed
old=(${old//;/ })
time_diff=$(( $time - ${old[0]} ))

# sanity check: has a positive amount of time passed
[[ "${time_diff}" -gt 0 ]] || exit

# calc bytes transferred, and their rate in byte/s
rx_diff=$(( $rx - ${old[1]} ))
tx_diff=$(( $tx - ${old[2]} ))
rx_rate=$(( $rx_diff / $time_diff ))
tx_rate=$(( $tx_diff / $time_diff ))

# shift by 10 bytes to get KiB/s. If the value is larger than
# 1024^2 = 1048576, then display MiB/s instead
#echo -n " "
# incoming
echo -n "⬇"
rx_kib=$(( $rx_rate >> 10 ))
if [[ "$rx_rate" -gt 1048576 ]]; then
printf '%sM' "`echo "scale=1; $rx_kib / 1024" | bc`"
else
echo -n "${rx_kib}K"
fi

echo -n " "

# outgoing
echo -n "⬆"
tx_kib=$(( $tx_rate >> 10 ))
if [[ "$tx_rate" -gt 1048576 ]]; then
printf '%sM' "`echo "scale=1; $tx_kib / 1024" | bc`"
else
echo -n "${tx_kib}K"
fi

}

#Show Ethernet info ; ip ; connected name - 20 (ifconfig; nmcli)
_ETHERNET_ () {

[ -z "$1" ] && icon="" || icon="$1"


(nmcli d | awk '/ethernet/{print $1}') | while read E ; do 
E_C=$(nmcli d | awk '/'"$E"'/{print $3}');
E_N=$(nmcli d | awk '/'"$E"'/{print $4}');
E_IP=$(ifconfig $E | awk '/inet addr/{print $2}' | cut -d: -f2);

[ "$E_C" == "connected" ] && echo -n "$icon $E_IP ($E_N) " || echo -n "$icon --- "

done
}

#Show prosses use by this User - 21 (ps)
_PS_ () {

[ -z "$1" ] && icon="" || icon="$1"

ps_usage=$(ps -U $USER | wc -l);

echo "$icon $ps_usage"
}

#Show System Kerenel - 22 (uname)
_KERNEL_ () {

[ -z "$1" ] && icon="" || icon="$1"

kernel_name=$(uname -r);

echo "$icon $kernel_name"
}

#Show Usb Plug in ur Computer - 23 (lsblk)
_USB_ () {

[ -z "$1" ] && icon="" || icon="$1"

USBS=$(lsblk -l -o 'TRAN' | grep "usb" | wc -l);

echo "$icon $USBS"
}

#Show Total pkgs install in your system - 24 (dpkg)
_PKGS_ () {

[ -z "$1" ] && icon="" || icon="$1"

pkgs=$(dpkg -l | awk '/^ii/{print }' | wc -l);

echo "$icon $pkgs"
}

#Check if there is any Update in ur system - 25 (apt-get)
_UPDATE_ () {

[ -z "$1" ] && icon="" || icon="$1"

U_PKGS=$(apt-get -s -o APT::Get::Show-User-Simulation-Note=0 dist-upgrade | grep "^\s\s" | wc -w);

echo "$icon $U_PKGS"
}

#Show ufw Status - 26 (ufw)
_UFW_ () {

[ -z "$1" ] && icon="" || icon="$1"

S_UFW=$(systemctl status ufw | awk '/Active:/{print $2}');

[ "$S_UFW" == "active" ] && echo "$icon On" || echo "$icon Off"
}

#Show number lock status - 27 
_NUM_LOCK_ () {

[ -z "$1" ] && icon="" || icon="$1"

NUM_LOCK=$(xset -q | awk '/00:/{print $8}');

[ "$NUM_LOCK" == "on" ] && echo "$icon On" || echo "$icon Off"
}

#Show Bluetooth Status - 28 
_BLUETOOTH_ () {

[ -z "$1" ] && icon="" || icon="$1"


S_BLU=$(systemctl status bluetooth.target | awk '/Active:/{print $2}');

[ "$S_BLU" == "active" ] && echo "$icon On" || echo "$icon Off"

}

#Show Caps lock status - 29 
_CAPS_LOCK_ () {

[ -z "$1" ] && icon="" || icon="$1"

CAPS_LOCK=$(xset -q | awk '/00:/{print $4}');

[ "$CAPS_LOCK" == "on" ] && echo "$icon On" || echo "$icon Off"
}

#Display X screensaver state - 30
_X_SCREENSAVER_ () {

[ -z "$1" ] && icon="" || icon="$1"

X_SCREEN=$(xssstate -s);

[ "$X_SCREEN" = "off" ] && echo "$icon Off" || echo "$icon On"
}

#Calcule XBACKLIGHT Points on System - 31
_XBACKLIGHT_ () {

# [ -z "$1" ] && icon="" || icon="$1"

#  MP=$(cat /proc/mounts | grep "^/dev/s*" | awk '{print $1}' | wc -l)

#echo "$icon $MP"

XBACKLIGHT=$(xbacklight -get |  cut -d "." -f 1) 
#if [[ "$XBACKLIGHT" -gt "70" ]] && [[ $Bat -lt 100 ]];then
#echo " $XBACKLIGHT %"
#elif [[ "$XBACKLIGHT" -gt "60" ]] && [[ $Bat -lt 70 ]];then
#echo  " $XBACKLIGHT %"
#elif  [[ "$XBACKLIGHT" -gt "30" ]] && [[ $Bat -lt 60 ]];then
#echo  " $XBACKLIGHT %"
#else
#echo  " $XBACKLIGHT%"
#fi
printf "%.f%%" $XBACKLIGHT

}

#Usage Dialog 
USAGE () {
echo -e "
$0 : Simple script Collaction for i3blocks 

Utils :
(1)  :
-  Show CPU Info * Deps : (Mpstat)
usage : $0 1 \"\$ICON\"
(2)  :
-  Show Multi Cpu Info * Deps : (Mpstat)
usage : $0 2 \"\$ICON\"
(3)  :
-  Show Memory Usage * Deps : (Free)
usage : $0 3 \"\$ICON\"
(4)  :
-  Show Swap Usage * Deps : (Free)
usage : $0 4 \"\$ICON\"
(5)  :
-  Show Date and Time * Deps : (Date)
usage : $0 5 \"\$ICON\"
(6)  :
-  Show Battery Status * Deps : (acpi)
usage : $0 6
(7)  :
-  Show Disk Usage * Deps : (df)
usage : $0 7 \"\$PARTITION\" \"\$ICON\"
example : $0 7 \"/home\" \"\"
(8)  :
-  Show Keyboard Layout
usage : $0 8 \"\$ICON\"
(9)  :
-  Show System language
usage : $0 9 \"\$ICON\"
(10) :
-  Show Machine Uptime * Deps : (uptime)
usage : $0 10 \"\$ICON\"
(11) :
-  Show Trash Size * Deps : (du)
usage : $0 11 \"\$ICON\"
(12) :
-  Show Window Usage * Deps : (xdotool)
usage : $0 12 \"\$ICON\"
(13) :
-  Show Mocp playing song and status * Deps : (moc)
usage : $0 13 \"\$ICON\"
(14) :
-  Show mpris Playing song and status * Deps : (mpris; mpris-ctl)
usage : $0 14 \"\$ICON\"
(15) :
-  Show Cpu Temp * Deps : (xsenser)
usage : $0 15 \"\$ICON\"
(16) :
-  Show Gpu Temp * Deps : (xsenser)
usage : $0 16 \"\$ICON\"
(17) :
-  Show Touchpad status * Deps : (sysclient)
usage : $0 17 \"\$ICON\"
(18) :
-  Show Volume Status * Deps : (alsa)
usage : $0 18
(19) :
-  Show Wifi info ; ip ; connected name * Deps : (ifconfig; nmcli)
usage : $0 19 \"\$ICON\"
(20) :
-  Show Ethernet info ; ip ; connected name *Deps : (ifconfig; nmcli)
usage : $0 20 \"\$ICON\"
(21) :
-  Show prosses use by this User * Deps : (ps)
usage : $0 21 \"\$ICON\"
(22) :
-  Show System Kerenel * Deps : (uname)
usage : $0 22 \"\$ICON\"
(23) :
-  Show Usb Plug in ur Computer * Deps : (lsblk)
usage : $0 23 \"\$ICON\"
(24) :
-  Show Total pkgs install in your system * Deps : (dpkg)
usage : $0 24 \"\$ICON\"
(25) :
-  Check if there is any Update in ur system * Deps : (apt-get)
usage : $0 25 \"\$ICON\"
(26) :
-  Show ufw Status * Deps : (ufw)
usage : $0 26 \"\$ICON\"
(27) :
-  Show number lock status
usage : $0 27 \"\$ICON\"
(28) :
-  Show Bluetooth Status
usage : $0 28 \"\$ICON\"
(29) :
-  Show Caps Lock Status
usage : $0 29 \"\$ICON\"
(30) :
-  Show X screensaver status
usage : $0 30 \"\$ICON\"
(31) :
-  Calcule Mount Points on System - 31
usage : $0 31 \"\$ICON\"
(-h):
- Show this help message
" |less
}


case $1 in 
1 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_CPU_ "$2"
;;
2 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_CPUS_ "$2"
;;
3 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_MEM_ "$2"
;;
4 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_SWAP_ "$2"
;;
5 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_DATE_ "$2"
;;
6 )
[ "$#" -gt "1" ] && {echo " $0 : To many Argument " && return 0 }
_BAT_
;;
7 )
[ "$#" -gt "3" ] && {echo " $0 : To many Argument " && return 0 }
_DISK_ "$2" "$3"
;;
8 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_KEY_ "$2"
;;
9 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_LANG_ "$2"
;;
10 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_UPTIME_ "$2"
;;
11 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_TRASH_ "$2"
;;
12 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_WINDOW_ "$2" "$3"
;;
13 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_MOCP_ "$2"
;;
14 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_MPRIS_ "$2"
;;
15 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_TEMP_ "$2"
;;
16 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_GPU_TEMP_ "$2"
;;
17 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_TOUCHPAD_ "$2"
;;
18 )
[ "$#" -gt "1" ] && {echo " $0 : To many Argument " && return 0 }
_VOLUME_
;;
19 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_WIFI_ "$2"
;;
20 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_ETHERNET_ "$2"
;;
21 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_PS_ "$2"
;;
22 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_KERNEL_ "$2"
;;
23 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_USB_ "$2"
;;
24 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_PKGS_ "$2"
;;
25 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_UPDATE_ "$2"
;;
26 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_UFW_ "$2"
;;
27 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_NUM_LOCK_ "$2"
;;
28 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_BLUETOOTH_ "$2"
;;
29 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_CAPS_LOCK_  "$2"
;;
30 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_X_SCREENSAVER_ "$2"
;;
31 )
[ "$#" -gt "2" ] && {echo " $0 : To many Argument " && return 0 }
_XBACKLIGHT_ "$2"
;;
-h | --help )
[ "$#" -gt "1" ] && {echo " $0 : To many Argument " && return 0 }
USAGE
;;
* )
echo -e " $0 \n\t-h | --help : to Show Help Dialog"
;;
esac
