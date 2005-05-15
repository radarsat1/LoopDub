# Microsoft Developer Studio Project File - Name="loopdub" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=loopdub - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "loopdub.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "loopdub.mak" CFG="loopdub - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "loopdub - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "loopdub - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "loopdub - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "SDL-1.2.8\include\\" /I "portaudio_v18_1\pa_common" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 gui\Release\scrobui.lib portmidi\pm_win\Release\portmidi.lib portmidi\pm_win\pm_win\Release\pm_dll.lib portaudio_v18_1\winvc\Lib\PAStaticDS.lib dsound.lib winmm.lib SDL-1.2.8\lib\SDL.lib SDL-1.2.8\lib\SDLmain.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "loopdub - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "SDL-1.2.8\include\\" /I "portaudio_v18_1\pa_common" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 gui\Debug\scrobui.lib portmidi\pm_win\Release\portmidi.lib portmidi\pm_win\pm_win\Release\pm_dll.lib portaudio_v18_1\winvc\Lib\PAStaticDSd.lib dsound.lib winmm.lib SDL-1.2.8\lib\SDL.lib SDL-1.2.8\lib\SDLmain.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "loopdub - Win32 Release"
# Name "loopdub - Win32 Debug"
# Begin Source File

SOURCE=.\filter.cpp
# End Source File
# Begin Source File

SOURCE=.\filter.h
# End Source File
# Begin Source File

SOURCE=.\ld_logo.h
# End Source File
# Begin Source File

SOURCE=.\ld_midi.cpp
# End Source File
# Begin Source File

SOURCE=.\ld_midi.h
# End Source File
# Begin Source File

SOURCE=.\loopdub.cpp
# End Source File
# Begin Source File

SOURCE=.\loopdub.h
# End Source File
# Begin Source File

SOURCE=.\loopob.cpp
# End Source File
# Begin Source File

SOURCE=.\loopob.h
# End Source File
# Begin Source File

SOURCE=.\platform.h
# End Source File
# Begin Source File

SOURCE=.\player.cpp
# End Source File
# Begin Source File

SOURCE=.\player.h
# End Source File
# Begin Source File

SOURCE=.\sample.cpp
# End Source File
# Begin Source File

SOURCE=.\sample.h
# End Source File
# End Target
# End Project
