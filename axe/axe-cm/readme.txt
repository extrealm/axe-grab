
Overview
--------
axe-cm.dll is a shell extension component for AXE. 
Original nppcm.dll is came from notepad++, WSciTEcm.
Below readme.txt describes replacing nppcm.dll with axe-cm.dll

This component is modified from the MIT open source project WSciTEcm
(Context Menu Extension for SciTE), made by Andre Burgaud <andre@burgaud.com>,
to meet the need of AXE.
Installing this Context Menu Handler (axe-cm.dll) creates a new command
"Edit with AXE" in the context menu of Windows Explorer.
You can quickly  open one or several selected files in Windows Explorer: 
right click on the selection and click on the command "Edit with AXE".

The manual installation is describe in the following sections.


Installation
------------
1) Copy axe-cm.dll in AXE directory.
2) In AXE directory installation, type the command "regsvr32 axe-cm.dll".
This will register the dll.

If everything goes well, you should have "Edit with AXE" 
when you right click on selected file(s) in Windows Explorer.

Uninstallation
--------------
In AXE directory installation, type the command "regsvr32 /u axe-cm.dll".


Unload the dll
--------------
If you try to delete or override the dll file and you get the error "Access is
denied.", the library is already loaded.
There are several options to workaround this issue:

Solution 1:
-  Close all the Windows Explorer instances open on your desktop and copy
axe-cm.dll using the command line (Example : "C:/>cp axe-cm.dll <axe_directory>").

Solution 2:
- Reboot the computer and delete or override axe-cm.dll (with the command line)
before starting Windows Explorer and using the context menu (right-click).

Solution 3:
- Open a command line window
- Type CTRL+ALT+DEL to display the Windows Task Manager, display the  Process tab 
and "kill" the explorer.exe process.
- If your exlorer did not restart automatically, start it manually from the command line window 
(c:/>explorer)
- Delete or override axe-cm.dll before using the context menu (Example: "C:/>cp axe-cm.dll <axe_directory>").

Build
-----
axe-cm.dll is built with Visual C++ 6.0. A Makefile is provided with the
sources: in the source directory, type "nmake". Ensure that all the
environment variables and paths are set correctly. To do so, use the command
file "VCVARS32.BAT" available in the bin directory of Visual C++ installation.

Otherwise, you may Visual Studio 2005 .sln file. 

License
-------
Copyright 2010 by Austin Shin <extrealm@naver.com>
Copyright 2002-2003 by Andre Burgaud <andre@burgaud.com>
See license.txt