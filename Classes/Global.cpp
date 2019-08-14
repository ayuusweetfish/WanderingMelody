#include "Global.h"

// JSON.stringify(a.map((s) => s.replace(/_/g, ' ').split(' ').map((w) => w.charAt(0) + w.substr(1).toLowerCase()).join(' ')))
const char *keyboardNames[] = {"=~=","Pause","Scroll Lock","Print","SysReq","Break","Escape","Backspace","Tab","Back Tab","Return","Caps Lock","Left Shift","Right Shift","Left Ctrl","Right Ctrl","Left Alt","Right Alt","Menu",
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
"Windows",
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
"Command",
#else
"Super",
#endif
"Insert","Home","Pg Up","Delete","End","Pg Down","Left","Right","Up","Down","Num Lock","KP +","KP -","KP *","KP /","KP Enter","KP Home","KP Up","KP Pg Up","KP Left","KP Five","KP Right","KP End","KP Down","KP Pg Down","KP Insert","KP Delete","F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12","Space","!","\"","Number","$","%","^","&","'","(",")","*","+",",","-",".","/","0","1","2","3","4","5","6","7","8","9",":",";","<","=",">","?","@","Capital A","Capital B","Capital C","Capital D","Capital E","Capital F","Capital G","Capital H","Capital I","Capital J","Capital K","Capital L","Capital M","Capital N","Capital O","Capital P","Capital Q","Capital R","Capital S","Capital T","Capital U","Capital V","Capital W","Capital X","Capital Y","Capital Z","[","\\","]","_","`","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","{","|","}","~","Euro","Pound","Yen","Middle Dot","Search","D-Pad Left","D-Pad Right","D-Pad Up","D-Pad Down","D-Pad Center","Enter","Play"};
const char *joystickNames[] = {"Left X","Left Y","Right X","Right Y","A","B","C","X","Y","Z","D-Pad Up","D-Pad Down","D-Pad Left","D-Pad Right","D-Pad Center","Left Shoulder","Right Shoulder","Axis Left Trigger","Axis Right Trigger","Left Thumbstick","Right Thumbstick","Start","Select","Pause"};
