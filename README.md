# qobbar

![Screenshots](https://github.com/zakariakov/qobbar/blob/master/example/qobbar3.jpg)

![Screenshots](https://github.com/zakariakov/qobbar/blob/master/example/qobbar4.png)

![Screenshots](https://github.com/zakariakov/qobbar/blob/master/example/qobbar1.png)


### Dependencies

- libX11
- libXcomposite
- libXdamage
- libQt5X11Extras
- libQt5Widgets
- libQt5Gui
- libQt5Concurrent
- libQt5DBus
- libQt5Core
- libGL
- libpthread


### Building from source


  ~~~ sh
  $ git clone https://github.com/zakariakov/qobbar.git
  $ cd qobbar
  $ qmake
  $ make
  $ sudo make install
  ~~~

### Configuration

 The configuration uses the NativeFormat CONF file format.

 The default SystemScope paths "/etc/xdg/qobbar/qobbar.conf"

 The default UserScope paths "$HOME/.config/qobbar/qobbar.conf" 

 to create user configuration 

  ~~~ sh
  $ mkdir -p "$HOME/.config/qobbar"
  $ cp /etc/xdg/qobbar/qobbar.conf $HOME/.config/qobbar
  ~~~

 or create any configuration file in "$HOME/.config/qobbar" 

 the suffix ".conf" is obligatoire ex;"myconf.conf"

### Running the app

  ~~~ sh
 "Usage: qobbar [OPTION]"
    "qobbar v: 0.1 "
    "OPTION:"
    " -h  --help                      Print this help."
    " -c  --config     <string>       config file name."
    "                                 ex: create file in $HOME/.config/qobbar/top-bar.conf "
    "                                 run \"qobbar -c top-bar\"  ."
    " -d  --debug                     Print debug in termminal."
    " -r  --right                     right-to-left layout direction."
  ~~~

to run the default configuration just run "qobbar".

or cp any configuration in the example folder to $HOME/.config/qobbar.

to run any configuration ex: top-bar.conf run "qobbar -c top-bar".

### Available modules

  ~~~ sh
    Panel         configured using this name 'Panel'.
    Pager         configured using this name 'Pager'.
    Systray       configured using this name 'Systray'.
    Taskbar       configured using this name 'Taskbar'.
    Statu         configured using any name ex: 'Cpu' 'Mem'.

  ~~~ 

### Available token                                       

#### Common 

  ~~~ sh
- Background			<string>  color Hex or xrdb.color<num>
- Foreground			<string>  color Hex or xrdb.color<num>
- Underline			<string>  color Hex or xrdb.color<num>
- Overline			<string>  color Hex or xrdb.color<num>
				to get color from Xresource 
				ex: 'Background=xrdb.background'
				ex: 'Overline=xrdb.color5'

- Border			<int>     default=0
- Alpha				<int>     0-to-255 default=255
- FontName			<string>  default parent fontfamily
- FontSize			<int>     default parent font size
- FontBold			<bool>    default  window fontbold
  ~~~

#### Panel 

  ~~~ sh
- BorderColor			<string>  color Hex or xrdb.color<num>
- BarLeft			<list> Ex:Systray,statu1,statu2
- BarCenter			<list> Ex:Time,Date
- BarRight			<list> Ex:Pager
- BarLeftSpacing		<int>  default=0
- BarRightSpacing		<int>  default=0
- BarCenterSpacing		<int>  default=0

 -----padding has no effect in tilling wm -----
- PaddingBottom                 <int>  default=0
- PaddingLeft                   <int>  default=0
- PaddingRight                  <int>  default=0
- PaddingTop                    <int>  default=0

  ~~~

#### Systray

#### Pager and Taskbar

  ~~~ sh
- ActiveBackground		<string>  default  window highlight
- ActiveAlpha 			<int>     0-to-255 default=255
- ActiveForeground		<string>  default  window highlightText
- ActiveUnderline		<string>  color Hex or xrdb.color
- ActiveOverline		<string>  color Hex or xrdb.color
  ~~~

#### Pager 

  ~~~ sh
- DesktopDesplay		<string>  "name" "index" "icon" default=index
 				icon-[0-9]  ex: home,office,multimedia,
				NOTE: The desktop name needs to match the name configured by the WM
 				You can get a list of the defined desktops using:
				$ xprop -root _NET_DESKTOP_NAMES

- IconsList			<list> list of icon 0 to 9
  ~~~

#### Status 

  ~~~ sh
- Command			<string>  Command to desplay
- Interval			<int>     second default 1
- Label				<string>  
- Suffix			<string>  
- Prefix			<string>  
- ClickLeft			<string>   Command to exec
- ClickRight			<string>   Command to exec
- MouseWheelUp			<string>   Command to exec
- MouseWheelDown		<string>   Command to exec

  ~~~


### Example 
  ~~~ sh
	[Panel]
	BarLeft=Button,Pager
	BarCenter=Taskbar
	BarRight=Cpu,Systray,Time
	Top=true
	Background=#000000
	;Foreground=xrdb.foreground
	Alpha=150

	[Pager]
	#ActiveBackground=#ffffff
	ActiveForeground=#ffffff
	ActiveOverline=#ffff00
	DesktopDesplay="icon"
	IconsList=,,,
	Foreground=#8A8383
	Border=1
	ActiveAlpha=0
	
	[Taskbar]
	ActiveForeground=#FFFFFF
	ActiveOverline=#1E90FF
	DesktopDesplay="icon"
	ActiveBackground=#ffffff
	ActiveAlpha=20
	Border=1

	[Time]
	Interval=12
	Command="date +%H:%M\--%d/%m/%y"
	Label="<span style=' color:#4BB94B;'> </span>"
	FontBold=true
	ClickLeft="zenity --calendar"

	[Cpu]
	;Command=$HOME/.config/qobbar/blocks.sh 1
	Command=$HOME/.config/scripts/cpu_usage
	Interval=2
	Label= "<span style=' color:#FFFFFF ;'></span> "	
	Foreground=#FFFFFF
	Overline=#ED163D
	Border=1
	
	[Button]
	Label=
	ClickLeft= qobmenu
	Underline=#FF3A00
	;Overline=#40BF4D
	Border=1
	Foreground=xrdb.foreground
  ~~~

NOTE : to use this qobmenu [**@zakariakov**](https://github.com/zakariakov/qobmenu)

or jgmenu (https://github.com/johanmalm/jgmenu)


