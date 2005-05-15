
Instructions for compiling LoopDub on Windows
---------------------------------------------

LoopDub for Windows is compiled using Visual C++.  Please note that as I
have version 6 of Visual C++, instructions may differ slightly if you are
using a later version.  In general however, I think it shouldn't be a
problem for you.  (Fingers crossed.)  If it is, let me know, but I can't
promise anything as I certainly won't be upgrading.

Apologies in advance for these necessary steps, but I didn't think it would
be right to include other people's packages directly in my own download.
If you are serious about helping to extend LoopDub, you'll have to download
these libs.  You only need to do it once anyways, once it's working you
should be all set.

(Can you tell I've gotten used to Debian, where a simple "apt-get" is all
you need to do?)

That said, for minimal frustration, PLEASE FOLLOW ALL THESE STEPS before
giving up.


1. Get the following files and copy them to the LoopDub source directory:

 - http://www.portaudio.com/archives/portaudio_v18_1.zip
 - http://www.portaudio.com/archives/winvc.zip
 - http://www-2.cs.cmu.edu/~music/portmusic/portmidi/portmidi17nov04.zip
 - http://www.libsdl.org/release/SDL-devel-1.2.8-VC6.zip

  (PortMidi should actually already be there.  I've included it because
  it is not readily available as a Linux package the way PortAudio and
  SDL are.  At least with Debian this is the case, maybe it's available
  for other distros, but that's not an issue here.)

  These URLs are valid as of right now (March 30, 2005), but if you can't
  download them a search should be able to find them.

2. Unzip all but winvc.zip.  Right-click and choose "Unzip Here", or
   equivalent.  You should now have three new folders with the following
   names:

 - loopdub/portaudio_v18_1
 - loopdub/portmidi
 - loopdub/SDL-devel-1.2.8

3. Unzip winvc.zip into its own folder.  All files should show up in a
   folder called "winvc".  You should move this folder to within the
   portaudio_v18_1 folder.  The new location of the winvc files should be:

 - loopdub/poraudio_v18_1/winvc


** COMPILE PORTMIDI **

1. Open loopdub/portmidi/portmidi.dsw.

2. Choose "Batch Build.." from the Build menu.  Select ONLY the following
   projects (Release and Debug):

 - pm_dll
 - portmidi

If you wish to compile the portmidi test programs, you may, but I will
warn you that there are some errors in the project files that caused me
a bit of a headache.  I will not explain here, as it would be out of the
scope of this document, but I'd be happy to explain if someone asks.


** COMPILE PORTAUDIO **

1. Open loopdub/portaudio_v18_1/winvc/WinVC.dsw

2. IMPORTANT STEP: There is a line in pa_dsound.c that causes LoopDub to
   crash in PaHost_OpenStream().  I still don't know why this causes a
   problem, but to work around it change line 54 from:

   #define PRINT(x) { printf x; fflush(stdout); }

   to 

   #define PRINT(x) { printf x; /*fflush(stdout);*/ }

   If anyone has any ideas on why calling fflush(stdout) triggers an
   access violation, I'd love to know.  It _shouldn't_ be a problem, but
   it was for me, for some reason or another.  Just change this line and
   it will work.  I'm not really in the position to debug PortAudio at this
   point, and besides, I'm aware that this isn't the latest version.

   (V19 is still unstable, so I prefer to depend on V18.)

3. Use Build/Batch Build.. to compile everything.


** COMPILE LOOPDUB **

SDL, conveniently, is already compiled for us, so we can continue with the
main event now.  

Open loopdub/loopdub.dsw and Build All.  LoopDub should compile and
run.  Remember that you have to give it an argument to tell it where to find
your loop folder, which you can do from the Project/Settings dialog.  In the
future perhaps I'll have it query using a dialog box, but this will do for
this release.
