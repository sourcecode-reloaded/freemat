; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "FreeMat"
!define PRODUCT_VERSION "1.09"
!define PRODUCT_PUBLISHER "Samit Basu"
!define PRODUCT_WEB_SITE "http://freemat.sf.net"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\FreeMat.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

SetCompressor lzma

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "..\..\COPYING"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Start menu page
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "FreeMat"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\FreeMat.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

!include WinMessages.nsh

!ifdef ALL_USERS
  !define WriteEnvStr_RegKey \
     'HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"'
!else
  !define WriteEnvStr_RegKey 'HKCU "Environment"'
!endif

#
# WriteEnvStr - Writes an environment variable
# Note: Win9x systems requires reboot
#
# Example:
#  Push "HOMEDIR"           # name
#  Push "C:\New Home Dir\"  # value
#  Call WriteEnvStr
#
Function WriteEnvStr
  Exch $1 ; $1 has environment variable value
  Exch
  Exch $0 ; $0 has environment variable name
  Push $2

  Call IsNT
  Pop $2
  StrCmp $2 1 WriteEnvStr_NT
    ; Not on NT
    StrCpy $2 $WINDIR 2 ; Copy drive of windows (c:)
    FileOpen $2 "$2\autoexec.bat" a
    FileSeek $2 0 END
    FileWrite $2 "$\r$\nSET $0=$1$\r$\n"
    FileClose $2
    SetRebootFlag true
    Goto WriteEnvStr_done

  WriteEnvStr_NT:
      WriteRegExpandStr ${WriteEnvStr_RegKey} $0 $1
      SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} \
        0 "STR:Environment" /TIMEOUT=5000

  WriteEnvStr_done:
    Pop $2
    Pop $1
    Pop $0
FunctionEnd

#
# un.DeleteEnvStr - Removes an environment variable
# Note: Win9x systems requires reboot
#
# Example:
#  Push "HOMEDIR"           # name
#  Call un.DeleteEnvStr
#
Function un.DeleteEnvStr
  Exch $0 ; $0 now has the name of the variable
  Push $1
  Push $2
  Push $3
  Push $4
  Push $5

  Call un.IsNT
  Pop $1
  StrCmp $1 1 DeleteEnvStr_NT
    ; Not on NT
    StrCpy $1 $WINDIR 2
    FileOpen $1 "$1\autoexec.bat" r
    GetTempFileName $4
    FileOpen $2 $4 w
    StrCpy $0 "SET $0="
    SetRebootFlag true

    DeleteEnvStr_dosLoop:
      FileRead $1 $3
      StrLen $5 $0
      StrCpy $5 $3 $5
      StrCmp $5 $0 DeleteEnvStr_dosLoop
      StrCmp $5 "" DeleteEnvStr_dosLoopEnd
      FileWrite $2 $3
      Goto DeleteEnvStr_dosLoop

    DeleteEnvStr_dosLoopEnd:
      FileClose $2
      FileClose $1
      StrCpy $1 $WINDIR 2
      Delete "$1\autoexec.bat"
      CopyFiles /SILENT $4 "$1\autoexec.bat"
      Delete $4
      Goto DeleteEnvStr_done

  DeleteEnvStr_NT:
    DeleteRegValue ${WriteEnvStr_RegKey} $0
    SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} \
      0 "STR:Environment" /TIMEOUT=5000

  DeleteEnvStr_done:
    Pop $5
    Pop $4
    Pop $3
    Pop $2
    Pop $1
    Pop $0
FunctionEnd

#
# [un.]IsNT - Pushes 1 if running on NT, 0 if not
#
# Example:
#   Call IsNT
#   Pop $0
#   StrCmp $0 1 +3
#     MessageBox MB_OK "Not running on NT!"
#     Goto +2
#     MessageBox MB_OK "Running on NT!"
#
!macro IsNT UN
Function ${UN}IsNT
  Push $0
  ReadRegStr $0 HKLM \
    "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
  StrCmp $0 "" 0 IsNT_yes
  ; we are not NT.
  Pop $0
  Push 0
  Return

  IsNT_yes:
    ; NT!!!
    Pop $0
    Push 1
FunctionEnd
!macroend
!insertmacro IsNT ""
!insertmacro IsNT "un."


Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "Setup.exe"
InstallDir "$PROGRAMFILES\FreeMat"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "Program" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "..\..\Release\FreeMat.exe"
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\FreeMat.lnk" "$INSTDIR\FreeMat.exe"
  CreateShortCut "$DESKTOP\FreeMat.lnk" "$INSTDIR\FreeMat.exe"
  File "..\..\help\freemat.chm"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Help.lnk" "$INSTDIR\freemat.chm"
SectionEnd

Section "M Files" SEC02
  SetOutPath "$INSTDIR\MFiles"
  File "..\..\MFiles\bind.m"
  File "..\..\MFiles\clusterboot.m"
  File "..\..\MFiles\copper.m"
  File "..\..\MFiles\exit.m"
  File "..\..\MFiles\eye.m"
  File "..\..\MFiles\fliplr.m"
  File "..\..\MFiles\flipud.m"
  File "..\..\MFiles\gausfit.m"
  File "..\..\MFiles\gfitfun.m"
  File "..\..\MFiles\gray.m"
  File "..\..\MFiles\imagez.m"
  File "..\..\MFiles\length.m"
  File "..\..\MFiles\linspace.m"
  File "..\..\MFiles\log2.m"
  File "..\..\MFiles\mpirun.m"
  File "..\..\MFiles\mpiserver.m"
  File "..\..\MFiles\mpispawn.m"
  File "..\..\MFiles\mprintimage.m"
  File "..\..\MFiles\mprintplot.m"
  File "..\..\MFiles\popdir.m"
  File "..\..\MFiles\pushdir.m"
  File "..\..\MFiles\rawread.m"
  File "..\..\MFiles\rcall.m"
  File "..\..\MFiles\rescan.m"
  File "..\..\MFiles\reshape.m"
  File "..\..\MFiles\rserver.m"
  File "..\..\MFiles\sqrt.m"
  File "..\..\MFiles\std.m"
SectionEnd

Section "Text Help" SEC03
  SetOutPath "$INSTDIR\MFiles"
  File "..\..\help\mdc\abs.mdc"
  File "..\..\help\mdc\acos.mdc"
  File "..\..\help\mdc\asin.mdc"
  File "..\..\help\mdc\atan2.mdc"
  File "..\..\help\mdc\atan.mdc"
  File "..\..\help\mdc\axis.mdc"
  File "..\..\help\mdc\bin2int.mdc"
  File "..\..\help\mdc\break.mdc"
  File "..\..\help\mdc\cd.mdc"
  File "..\..\help\mdc\ceil.mdc"
  File "..\..\help\mdc\cell.mdc"
  File "..\..\help\mdc\clear.mdc"
  File "..\..\help\mdc\close.mdc"
  File "..\..\help\mdc\colon.mdc"
  File "..\..\help\mdc\colormap.mdc"
  File "..\..\help\mdc\comparisonops.mdc"
  File "..\..\help\mdc\complex.mdc"
  File "..\..\help\mdc\conj.mdc"
  File "..\..\help\mdc\continue.mdc"
  File "..\..\help\mdc\copy.mdc"
  File "..\..\help\mdc\cos.mdc"
  File "..\..\help\mdc\cot.mdc"
  File "..\..\help\mdc\csc.mdc"
  File "..\..\help\mdc\cumsum.mdc"
  File "..\..\help\mdc\dawson.mdc"
  File "..\..\help\mdc\dcomplex.mdc"
  File "..\..\help\mdc\diag.mdc"
  File "..\..\help\mdc\disp.mdc"
  File "..\..\help\mdc\dotleftdivide.mdc"
  File "..\..\help\mdc\dotpower.mdc"
  File "..\..\help\mdc\dotrightdivide.mdc"
  File "..\..\help\mdc\dottimes.mdc"
  File "..\..\help\mdc\double.mdc"
  File "..\..\help\mdc\eig.mdc"
  File "..\..\help\mdc\ei.mdc"
  File "..\..\help\mdc\e.mdc"
  File "..\..\help\mdc\eone.mdc"
  File "..\..\help\mdc\eps.mdc"
  File "..\..\help\mdc\erfc.mdc"
  File "..\..\help\mdc\erfcx.mdc"
  File "..\..\help\mdc\erf.mdc"
  File "..\..\help\mdc\error.mdc"
  File "..\..\help\mdc\eval.mdc"
  File "..\..\help\mdc\exist.mdc"
  File "..\..\help\mdc\expei.mdc"
  File "..\..\help\mdc\exp.mdc"
  File "..\..\help\mdc\fclose.mdc"
  File "..\..\help\mdc\feof.mdc"
  File "..\..\help\mdc\feps.mdc"
  File "..\..\help\mdc\feval.mdc"
  File "..\..\help\mdc\fft.mdc"
  File "..\..\help\mdc\fgetline.mdc"
  File "..\..\help\mdc\figure.mdc"
  File "..\..\help\mdc\find.mdc"
  File "..\..\help\mdc\fitfun.mdc"
  File "..\..\help\mdc\float.mdc"
  File "..\..\help\mdc\floor.mdc"
  File "..\..\help\mdc\fopen.mdc"
  File "..\..\help\mdc\for.mdc"
  File "..\..\help\mdc\fprintf.mdc"
  File "..\..\help\mdc\fread.mdc"
  File "..\..\help\mdc\fscanf.mdc"
  File "..\..\help\mdc\fseek.mdc"
  File "..\..\help\mdc\ftell.mdc"
  File "..\..\help\mdc\function.mdc"
  File "..\..\help\mdc\fwrite.mdc"
  File "..\..\help\mdc\gammaln.mdc"
  File "..\..\help\mdc\gamma.mdc"
  File "..\..\help\mdc\getpath.mdc"
  File "..\..\help\mdc\getprintlimit.mdc"
  File "..\..\help\mdc\global.mdc"
  File "..\..\help\mdc\grid.mdc"
  File "..\..\help\mdc\help.mdc"
  File "..\..\help\mdc\hermitian.mdc"
  File "..\..\help\mdc\hold.mdc"
  File "..\..\help\mdc\if.mdc"
  File "..\..\help\mdc\image.mdc"
  File "..\..\help\mdc\imag.mdc"
  File "..\..\help\mdc\i.mdc"
  File "..\..\help\mdc\import.mdc"
  File "..\..\help\mdc\indexing.mdc"
  File "..\..\help\mdc\inf.mdc"
  File "..\..\help\mdc\int16.mdc"
  File "..\..\help\mdc\int2bin.mdc"
  File "..\..\help\mdc\int32.mdc"
  File "..\..\help\mdc\int8.mdc"
  File "..\..\help\mdc\isempty.mdc"
  File "..\..\help\mdc\isinf.mdc"
  File "..\..\help\mdc\isnan.mdc"
  File "..\..\help\mdc\keyboard.mdc"
  File "..\..\help\mdc\keywords.mdc"
  File "..\..\help\mdc\lasterr.mdc"
  File "..\..\help\mdc\leftdivide.mdc"
  File "..\..\help\mdc\legend.mdc"
  File "..\..\help\mdc\loadlib.mdc"
  File "..\..\help\mdc\load.mdc"
  File "..\..\help\mdc\logicalops.mdc"
  File "..\..\help\mdc\log.mdc"
  File "..\..\help\mdc\ls.mdc"
  File "..\..\help\mdc\matrix.mdc"
  File "..\..\help\mdc\max.mdc"
  File "..\..\help\mdc\mean.mdc"
  File "..\..\help\mdc\min.mdc"
  File "..\..\help\mdc\minus.mdc"
  File "..\..\help\mdc\mpiallreduce.mdc"
  File "..\..\help\mdc\mpibarrier.mdc"
  File "..\..\help\mdc\mpibcast.mdc"
  File "..\..\help\mdc\mpicommgetparent.mdc"
  File "..\..\help\mdc\mpicommrank.mdc"
  File "..\..\help\mdc\mpicommsize.mdc"
  File "..\..\help\mdc\mpicommspawn.mdc"
  File "..\..\help\mdc\mpifinalize.mdc"
  File "..\..\help\mdc\mpiinitialized.mdc"
  File "..\..\help\mdc\mpiinit.mdc"
  File "..\..\help\mdc\mpiintercommmerge.mdc"
  File "..\..\help\mdc\mpirecv.mdc"
  File "..\..\help\mdc\mpireduce.mdc"
  File "..\..\help\mdc\mpisend.mdc"
  File "..\..\help\mdc\nan.mdc"
  File "..\..\help\mdc\nargin.mdc"
  File "..\..\help\mdc\nargout.mdc"
  File "..\..\help\mdc\ones.mdc"
  File "..\..\help\mdc\pcode.mdc"
  File "..\..\help\mdc\persistent.mdc"
  File "..\..\help\mdc\pi.mdc"
  File "..\..\help\mdc\plot.mdc"
  File "..\..\help\mdc\plus.mdc"
  File "..\..\help\mdc\point.mdc"
  File "..\..\help\mdc\power.mdc"
  File "..\..\help\mdc\printf.mdc"
  File "..\..\help\mdc\print.mdc"
  File "..\..\help\mdc\prod.mdc"
  File "..\..\help\mdc\psi.mdc"
  File "..\..\help\mdc\pwd.mdc"
  File "..\..\help\mdc\qr.mdc"
  File "..\..\help\mdc\quit.mdc"
  File "..\..\help\mdc\randbeta.mdc"
  File "..\..\help\mdc\randbin.mdc"
  File "..\..\help\mdc\randchi.mdc"
  File "..\..\help\mdc\randexp.mdc"
  File "..\..\help\mdc\randf.mdc"
  File "..\..\help\mdc\randgamma.mdc"
  File "..\..\help\mdc\randi.mdc"
  File "..\..\help\mdc\rand.mdc"
  File "..\..\help\mdc\randmulti.mdc"
  File "..\..\help\mdc\randnbin.mdc"
  File "..\..\help\mdc\randnchi.mdc"
  File "..\..\help\mdc\randnf.mdc"
  File "..\..\help\mdc\randn.mdc"
  File "..\..\help\mdc\randp.mdc"
  File "..\..\help\mdc\real.mdc"
  File "..\..\help\mdc\repmat.mdc"
  File "..\..\help\mdc\reshape.mdc"
  File "..\..\help\mdc\retall.mdc"
  File "..\..\help\mdc\return.mdc"
  File "..\..\help\mdc\rightdivide.mdc"
  File "..\..\help\mdc\save.mdc"
  File "..\..\help\mdc\script.mdc"
  File "..\..\help\mdc\sec.mdc"
  File "..\..\help\mdc\seed.mdc"
  File "..\..\help\mdc\setpath.mdc"
  File "..\..\help\mdc\setprintlimit.mdc"
  File "..\..\help\mdc\sin.mdc"
  File "..\..\help\mdc\sizefig.mdc"
  File "..\..\help\mdc\size.mdc"
  File "..\..\help\mdc\sleep.mdc"
  File "..\..\help\mdc\source.mdc"
  File "..\..\help\mdc\special.mdc"
  File "..\..\help\mdc\sprintf.mdc"
  File "..\..\help\mdc\str2num.mdc"
  File "..\..\help\mdc\strcmp.mdc"
  File "..\..\help\mdc\struct.mdc"
  File "..\..\help\mdc\sum.mdc"
  File "..\..\help\mdc\svd.mdc"
  File "..\..\help\mdc\switch.mdc"
  File "..\..\help\mdc\system.mdc"
  File "..\..\help\mdc\tan.mdc"
  File "..\..\help\mdc\times.mdc"
  File "..\..\help\mdc\title.mdc"
  File "..\..\help\mdc\transpose.mdc"
  File "..\..\help\mdc\try.mdc"
  File "..\..\help\mdc\typeof.mdc"
  File "..\..\help\mdc\uint16.mdc"
  File "..\..\help\mdc\uint32.mdc"
  File "..\..\help\mdc\uint8.mdc"
  File "..\..\help\mdc\varargin.mdc"
  File "..\..\help\mdc\varargout.mdc"
  File "..\..\help\mdc\var.mdc"
  File "..\..\help\mdc\where.mdc"
  File "..\..\help\mdc\which.mdc"
  File "..\..\help\mdc\while.mdc"
  File "..\..\help\mdc\who.mdc"
  File "..\..\help\mdc\winlev.mdc"
  File "..\..\help\mdc\xlabel.mdc"
  File "..\..\help\mdc\ylabel.mdc"
  File "..\..\help\mdc\zeros.mdc"
  File "..\..\help\mdc\zoom.mdc"
SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -EnvironmentVariables
  Push "FREEMAT_PATH"
  Push "$INSTDIR\MFiles"
  Call WriteEnvStr
  MessageBox MB_OK "Installer will set the FREEMAT_PATH environment variable to point to the installed MFiles only.  You must modify this environment variable to point to your FreeMat .m files."
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\FreeMat.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\FreeMat.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "${PRODUCT_STARTMENU_REGVAL}" "$ICONS_GROUP"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "FreeMat was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove FreeMat and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  ReadRegStr $ICONS_GROUP ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "${PRODUCT_STARTMENU_REGVAL}"
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\MFiles\bind.m"
  Delete "$INSTDIR\MFiles\clusterboot.m"
  Delete "$INSTDIR\MFiles\copper.m"
  Delete "$INSTDIR\MFiles\exit.m"
  Delete "$INSTDIR\MFiles\eye.m"
  Delete "$INSTDIR\MFiles\fliplr.m"
  Delete "$INSTDIR\MFiles\flipud.m"
  Delete "$INSTDIR\MFiles\gausfit.m"
  Delete "$INSTDIR\MFiles\gfitfun.m"
  Delete "$INSTDIR\MFiles\gray.m"
  Delete "$INSTDIR\MFiles\imagez.m"
  Delete "$INSTDIR\MFiles\length.m"
  Delete "$INSTDIR\MFiles\linspace.m"
  Delete "$INSTDIR\MFiles\log2.m"
  Delete "$INSTDIR\MFiles\mpirun.m"
  Delete "$INSTDIR\MFiles\mpiserver.m"
  Delete "$INSTDIR\MFiles\mpispawn.m"
  Delete "$INSTDIR\MFiles\mprintimage.m"
  Delete "$INSTDIR\MFiles\mprintplot.m"
  Delete "$INSTDIR\MFiles\popdir.m"
  Delete "$INSTDIR\MFiles\pushdir.m"
  Delete "$INSTDIR\MFiles\rawread.m"
  Delete "$INSTDIR\MFiles\rcall.m"
  Delete "$INSTDIR\MFiles\rescan.m"
  Delete "$INSTDIR\MFiles\reshape.m"
  Delete "$INSTDIR\MFiles\rserver.m"
  Delete "$INSTDIR\MFiles\sqrt.m"
  Delete "$INSTDIR\MFiles\std.m"
  Delete "$INSTDIR\help\abs.mdc"
  Delete "$INSTDIR\help\acos.mdc"
  Delete "$INSTDIR\help\asin.mdc"
  Delete "$INSTDIR\help\atan2.mdc"
  Delete "$INSTDIR\help\atan.mdc"
  Delete "$INSTDIR\help\axis.mdc"
  Delete "$INSTDIR\help\bin2int.mdc"
  Delete "$INSTDIR\help\break.mdc"
  Delete "$INSTDIR\help\cd.mdc"
  Delete "$INSTDIR\help\ceil.mdc"
  Delete "$INSTDIR\help\cell.mdc"
  Delete "$INSTDIR\help\clear.mdc"
  Delete "$INSTDIR\help\close.mdc"
  Delete "$INSTDIR\help\colon.mdc"
  Delete "$INSTDIR\help\colormap.mdc"
  Delete "$INSTDIR\help\comparisonops.mdc"
  Delete "$INSTDIR\help\complex.mdc"
  Delete "$INSTDIR\help\conj.mdc"
  Delete "$INSTDIR\help\continue.mdc"
  Delete "$INSTDIR\help\copy.mdc"
  Delete "$INSTDIR\help\cos.mdc"
  Delete "$INSTDIR\help\cot.mdc"
  Delete "$INSTDIR\help\csc.mdc"
  Delete "$INSTDIR\help\cumsum.mdc"
  Delete "$INSTDIR\help\dawson.mdc"
  Delete "$INSTDIR\help\dcomplex.mdc"
  Delete "$INSTDIR\help\diag.mdc"
  Delete "$INSTDIR\help\disp.mdc"
  Delete "$INSTDIR\help\dotleftdivide.mdc"
  Delete "$INSTDIR\help\dotpower.mdc"
  Delete "$INSTDIR\help\dotrightdivide.mdc"
  Delete "$INSTDIR\help\dottimes.mdc"
  Delete "$INSTDIR\help\double.mdc"
  Delete "$INSTDIR\help\eig.mdc"
  Delete "$INSTDIR\help\ei.mdc"
  Delete "$INSTDIR\help\e.mdc"
  Delete "$INSTDIR\help\eone.mdc"
  Delete "$INSTDIR\help\eps.mdc"
  Delete "$INSTDIR\help\erfc.mdc"
  Delete "$INSTDIR\help\erfcx.mdc"
  Delete "$INSTDIR\help\erf.mdc"
  Delete "$INSTDIR\help\error.mdc"
  Delete "$INSTDIR\help\eval.mdc"
  Delete "$INSTDIR\help\exist.mdc"
  Delete "$INSTDIR\help\expei.mdc"
  Delete "$INSTDIR\help\exp.mdc"
  Delete "$INSTDIR\help\fclose.mdc"
  Delete "$INSTDIR\help\feof.mdc"
  Delete "$INSTDIR\help\feps.mdc"
  Delete "$INSTDIR\help\feval.mdc"
  Delete "$INSTDIR\help\fft.mdc"
  Delete "$INSTDIR\help\fgetline.mdc"
  Delete "$INSTDIR\help\figure.mdc"
  Delete "$INSTDIR\help\find.mdc"
  Delete "$INSTDIR\help\fitfun.mdc"
  Delete "$INSTDIR\help\float.mdc"
  Delete "$INSTDIR\help\floor.mdc"
  Delete "$INSTDIR\help\fopen.mdc"
  Delete "$INSTDIR\help\for.mdc"
  Delete "$INSTDIR\help\fprintf.mdc"
  Delete "$INSTDIR\help\fread.mdc"
  Delete "$INSTDIR\help\fscanf.mdc"
  Delete "$INSTDIR\help\fseek.mdc"
  Delete "$INSTDIR\help\ftell.mdc"
  Delete "$INSTDIR\help\function.mdc"
  Delete "$INSTDIR\help\fwrite.mdc"
  Delete "$INSTDIR\help\gammaln.mdc"
  Delete "$INSTDIR\help\gamma.mdc"
  Delete "$INSTDIR\help\getpath.mdc"
  Delete "$INSTDIR\help\getprintlimit.mdc"
  Delete "$INSTDIR\help\global.mdc"
  Delete "$INSTDIR\help\grid.mdc"
  Delete "$INSTDIR\help\help.mdc"
  Delete "$INSTDIR\help\hermitian.mdc"
  Delete "$INSTDIR\help\hold.mdc"
  Delete "$INSTDIR\help\if.mdc"
  Delete "$INSTDIR\help\image.mdc"
  Delete "$INSTDIR\help\imag.mdc"
  Delete "$INSTDIR\help\i.mdc"
  Delete "$INSTDIR\help\import.mdc"
  Delete "$INSTDIR\help\indexing.mdc"
  Delete "$INSTDIR\help\inf.mdc"
  Delete "$INSTDIR\help\int16.mdc"
  Delete "$INSTDIR\help\int2bin.mdc"
  Delete "$INSTDIR\help\int32.mdc"
  Delete "$INSTDIR\help\int8.mdc"
  Delete "$INSTDIR\help\isempty.mdc"
  Delete "$INSTDIR\help\isinf.mdc"
  Delete "$INSTDIR\help\isnan.mdc"
  Delete "$INSTDIR\help\keyboard.mdc"
  Delete "$INSTDIR\help\keywords.mdc"
  Delete "$INSTDIR\help\lasterr.mdc"
  Delete "$INSTDIR\help\leftdivide.mdc"
  Delete "$INSTDIR\help\legend.mdc"
  Delete "$INSTDIR\help\loadlib.mdc"
  Delete "$INSTDIR\help\load.mdc"
  Delete "$INSTDIR\help\logicalops.mdc"
  Delete "$INSTDIR\help\log.mdc"
  Delete "$INSTDIR\help\ls.mdc"
  Delete "$INSTDIR\help\matrix.mdc"
  Delete "$INSTDIR\help\max.mdc"
  Delete "$INSTDIR\help\mean.mdc"
  Delete "$INSTDIR\help\min.mdc"
  Delete "$INSTDIR\help\minus.mdc"
  Delete "$INSTDIR\help\mpiallreduce.mdc"
  Delete "$INSTDIR\help\mpibarrier.mdc"
  Delete "$INSTDIR\help\mpibcast.mdc"
  Delete "$INSTDIR\help\mpicommgetparent.mdc"
  Delete "$INSTDIR\help\mpicommrank.mdc"
  Delete "$INSTDIR\help\mpicommsize.mdc"
  Delete "$INSTDIR\help\mpicommspawn.mdc"
  Delete "$INSTDIR\help\mpifinalize.mdc"
  Delete "$INSTDIR\help\mpiinitialized.mdc"
  Delete "$INSTDIR\help\mpiinit.mdc"
  Delete "$INSTDIR\help\mpiintercommmerge.mdc"
  Delete "$INSTDIR\help\mpirecv.mdc"
  Delete "$INSTDIR\help\mpireduce.mdc"
  Delete "$INSTDIR\help\mpisend.mdc"
  Delete "$INSTDIR\help\nan.mdc"
  Delete "$INSTDIR\help\nargin.mdc"
  Delete "$INSTDIR\help\nargout.mdc"
  Delete "$INSTDIR\help\ones.mdc"
  Delete "$INSTDIR\help\pcode.mdc"
  Delete "$INSTDIR\help\persistent.mdc"
  Delete "$INSTDIR\help\pi.mdc"
  Delete "$INSTDIR\help\plot.mdc"
  Delete "$INSTDIR\help\plus.mdc"
  Delete "$INSTDIR\help\point.mdc"
  Delete "$INSTDIR\help\power.mdc"
  Delete "$INSTDIR\help\printf.mdc"
  Delete "$INSTDIR\help\print.mdc"
  Delete "$INSTDIR\help\prod.mdc"
  Delete "$INSTDIR\help\psi.mdc"
  Delete "$INSTDIR\help\pwd.mdc"
  Delete "$INSTDIR\help\qr.mdc"
  Delete "$INSTDIR\help\quit.mdc"
  Delete "$INSTDIR\help\randbeta.mdc"
  Delete "$INSTDIR\help\randbin.mdc"
  Delete "$INSTDIR\help\randchi.mdc"
  Delete "$INSTDIR\help\randexp.mdc"
  Delete "$INSTDIR\help\randf.mdc"
  Delete "$INSTDIR\help\randgamma.mdc"
  Delete "$INSTDIR\help\randi.mdc"
  Delete "$INSTDIR\help\rand.mdc"
  Delete "$INSTDIR\help\randmulti.mdc"
  Delete "$INSTDIR\help\randnbin.mdc"
  Delete "$INSTDIR\help\randnchi.mdc"
  Delete "$INSTDIR\help\randnf.mdc"
  Delete "$INSTDIR\help\randn.mdc"
  Delete "$INSTDIR\help\randp.mdc"
  Delete "$INSTDIR\help\real.mdc"
  Delete "$INSTDIR\help\repmat.mdc"
  Delete "$INSTDIR\help\reshape.mdc"
  Delete "$INSTDIR\help\retall.mdc"
  Delete "$INSTDIR\help\return.mdc"
  Delete "$INSTDIR\help\rightdivide.mdc"
  Delete "$INSTDIR\help\save.mdc"
  Delete "$INSTDIR\help\script.mdc"
  Delete "$INSTDIR\help\sec.mdc"
  Delete "$INSTDIR\help\seed.mdc"
  Delete "$INSTDIR\help\setpath.mdc"
  Delete "$INSTDIR\help\setprintlimit.mdc"
  Delete "$INSTDIR\help\sin.mdc"
  Delete "$INSTDIR\help\sizefig.mdc"
  Delete "$INSTDIR\help\size.mdc"
  Delete "$INSTDIR\help\sleep.mdc"
  Delete "$INSTDIR\help\source.mdc"
  Delete "$INSTDIR\help\special.mdc"
  Delete "$INSTDIR\help\sprintf.mdc"
  Delete "$INSTDIR\help\str2num.mdc"
  Delete "$INSTDIR\help\strcmp.mdc"
  Delete "$INSTDIR\help\struct.mdc"
  Delete "$INSTDIR\help\sum.mdc"
  Delete "$INSTDIR\help\svd.mdc"
  Delete "$INSTDIR\help\switch.mdc"
  Delete "$INSTDIR\help\system.mdc"
  Delete "$INSTDIR\help\tan.mdc"
  Delete "$INSTDIR\help\times.mdc"
  Delete "$INSTDIR\help\title.mdc"
  Delete "$INSTDIR\help\transpose.mdc"
  Delete "$INSTDIR\help\try.mdc"
  Delete "$INSTDIR\help\typeof.mdc"
  Delete "$INSTDIR\help\uint16.mdc"
  Delete "$INSTDIR\help\uint32.mdc"
  Delete "$INSTDIR\help\uint8.mdc"
  Delete "$INSTDIR\help\varargin.mdc"
  Delete "$INSTDIR\help\varargout.mdc"
  Delete "$INSTDIR\help\var.mdc"
  Delete "$INSTDIR\help\where.mdc"
  Delete "$INSTDIR\help\which.mdc"
  Delete "$INSTDIR\help\while.mdc"
  Delete "$INSTDIR\help\who.mdc"
  Delete "$INSTDIR\help\winlev.mdc"
  Delete "$INSTDIR\help\xlabel.mdc"
  Delete "$INSTDIR\help\ylabel.mdc"
  Delete "$INSTDIR\help\zeros.mdc"
  Delete "$INSTDIR\help\zoom.mdc"
  Delete "$INSTDIR\help\abs.mdc"
  Delete "$INSTDIR\help\acos.mdc"
  Delete "$INSTDIR\help\asin.mdc"
  Delete "$INSTDIR\help\atan2.mdc"
  Delete "$INSTDIR\help\atan.mdc"
  Delete "$INSTDIR\help\axis.mdc"
  Delete "$INSTDIR\help\break.mdc"
  Delete "$INSTDIR\help\cd.mdc"
  Delete "$INSTDIR\help\ceil.mdc"
  Delete "$INSTDIR\help\cell.mdc"
  Delete "$INSTDIR\help\clear.mdc"
  Delete "$INSTDIR\help\closeimage.mdc"
  Delete "$INSTDIR\help\closeplot.mdc"
  Delete "$INSTDIR\help\colon.mdc"
  Delete "$INSTDIR\help\colormap.mdc"
  Delete "$INSTDIR\help\comparisonops.mdc"
  Delete "$INSTDIR\help\complex.mdc"
  Delete "$INSTDIR\help\conj.mdc"
  Delete "$INSTDIR\help\continue.mdc"
  Delete "$INSTDIR\help\cos.mdc"
  Delete "$INSTDIR\help\cot.mdc"
  Delete "$INSTDIR\help\csc.mdc"
  Delete "$INSTDIR\help\cumsum.mdc"
  Delete "$INSTDIR\help\dcomplex.mdc"
  Delete "$INSTDIR\help\diag.mdc"
  Delete "$INSTDIR\help\disp.mdc"
  Delete "$INSTDIR\help\dotleftdivide.mdc"
  Delete "$INSTDIR\help\dotpower.mdc"
  Delete "$INSTDIR\help\dotrightdivide.mdc"
  Delete "$INSTDIR\help\dottimes.mdc"
  Delete "$INSTDIR\help\double.mdc"
  Delete "$INSTDIR\help\e.mdc"
  Delete "$INSTDIR\help\eig.mdc"
  Delete "$INSTDIR\help\error.mdc"
  Delete "$INSTDIR\help\eval.mdc"
  Delete "$INSTDIR\help\exist.mdc"
  Delete "$INSTDIR\help\exp.mdc"
  Delete "$INSTDIR\help\fclose.mdc"
  Delete "$INSTDIR\help\feof.mdc"
  Delete "$INSTDIR\help\feval.mdc"
  Delete "$INSTDIR\help\fft.mdc"
  Delete "$INSTDIR\help\fgetline.mdc"
  Delete "$INSTDIR\help\find.mdc"
  Delete "$INSTDIR\help\float.mdc"
  Delete "$INSTDIR\help\floor.mdc"
  Delete "$INSTDIR\help\fopen.mdc"
  Delete "$INSTDIR\help\for.mdc"
  Delete "$INSTDIR\help\fprintf.mdc"
  Delete "$INSTDIR\help\fread.mdc"
  Delete "$INSTDIR\help\fscanf.mdc"
  Delete "$INSTDIR\help\fseek.mdc"
  Delete "$INSTDIR\help\ftell.mdc"
  Delete "$INSTDIR\help\function.mdc"
  Delete "$INSTDIR\help\fwrite.mdc"
  Delete "$INSTDIR\help\getpath.mdc"
  Delete "$INSTDIR\help\getprintlimit.mdc"
  Delete "$INSTDIR\help\global.mdc"
  Delete "$INSTDIR\help\grid.mdc"
  Delete "$INSTDIR\help\help.mdc"
  Delete "$INSTDIR\help\hermitian.mdc"
  Delete "$INSTDIR\help\hold.mdc"
  Delete "$INSTDIR\help\i.mdc"
  Delete "$INSTDIR\help\if.mdc"
  Delete "$INSTDIR\help\imag.mdc"
  Delete "$INSTDIR\help\image.mdc"
  Delete "$INSTDIR\help\import.mdc"
  Delete "$INSTDIR\help\indexing.mdc"
  Delete "$INSTDIR\help\isnan.mdc"
  Delete "$INSTDIR\help\isinf.mdc"
  Delete "$INSTDIR\help\inf.mdc"
  Delete "$INSTDIR\help\int8.mdc"
  Delete "$INSTDIR\help\int16.mdc"
  Delete "$INSTDIR\help\int32.mdc"
  Delete "$INSTDIR\help\isempty.mdc"
  Delete "$INSTDIR\help\keyboard.mdc"
  Delete "$INSTDIR\help\keywords.mdc"
  Delete "$INSTDIR\help\lasterr.mdc"
  Delete "$INSTDIR\help\leftdivide.mdc"
  Delete "$INSTDIR\help\load.mdc"
  Delete "$INSTDIR\help\loadlib.mdc"
  Delete "$INSTDIR\help\log.mdc"
  Delete "$INSTDIR\help\logicalops.mdc"
  Delete "$INSTDIR\help\ls.mdc"
  Delete "$INSTDIR\help\matrix.mdc"
  Delete "$INSTDIR\help\max.mdc"
  Delete "$INSTDIR\help\mean.mdc"
  Delete "$INSTDIR\help\min.mdc"
  Delete "$INSTDIR\help\minus.mdc"
  Delete "$INSTDIR\help\nan.mdc"
  Delete "$INSTDIR\help\nargin.mdc"
  Delete "$INSTDIR\help\nargout.mdc"
  Delete "$INSTDIR\help\newimage.mdc"
  Delete "$INSTDIR\help\newplot.mdc"
  Delete "$INSTDIR\help\ones.mdc"
  Delete "$INSTDIR\help\persistent.mdc"
  Delete "$INSTDIR\help\pi.mdc"
  Delete "$INSTDIR\help\plot.mdc"
  Delete "$INSTDIR\help\plus.mdc"
  Delete "$INSTDIR\help\point.mdc"
  Delete "$INSTDIR\help\power.mdc"
  Delete "$INSTDIR\help\printf.mdc"
  Delete "$INSTDIR\help\printimage.mdc"
  Delete "$INSTDIR\help\printplot.mdc"
  Delete "$INSTDIR\help\prod.mdc"
  Delete "$INSTDIR\help\pwd.mdc"
  Delete "$INSTDIR\help\quit.mdc"
  Delete "$INSTDIR\help\rand.mdc"
  Delete "$INSTDIR\help\randn.mdc"
  Delete "$INSTDIR\help\real.mdc"
  Delete "$INSTDIR\help\reshape.mdc"
  Delete "$INSTDIR\help\retall.mdc"
  Delete "$INSTDIR\help\return.mdc"
  Delete "$INSTDIR\help\rightdivide.mdc"
  Delete "$INSTDIR\help\save.mdc"
  Delete "$INSTDIR\help\script.mdc"
  Delete "$INSTDIR\help\sec.mdc"
  Delete "$INSTDIR\help\seed.mdc"
  Delete "$INSTDIR\help\setpath.mdc"
  Delete "$INSTDIR\help\setprintlimit.mdc"
  Delete "$INSTDIR\help\sin.mdc"
  Delete "$INSTDIR\help\size.mdc"
  Delete "$INSTDIR\help\sizeimage.mdc"
  Delete "$INSTDIR\help\sizeplot.mdc"
  Delete "$INSTDIR\help\sleep.mdc"
  Delete "$INSTDIR\help\source.mdc"
  Delete "$INSTDIR\help\special.mdc"
  Delete "$INSTDIR\help\sprintf.mdc"
  Delete "$INSTDIR\help\strcmp.mdc"
  Delete "$INSTDIR\help\struct.mdc"
  Delete "$INSTDIR\help\sum.mdc"
  Delete "$INSTDIR\help\svd.mdc"
  Delete "$INSTDIR\help\switch.mdc"
  Delete "$INSTDIR\help\system.mdc"
  Delete "$INSTDIR\help\tan.mdc"
  Delete "$INSTDIR\help\times.mdc"
  Delete "$INSTDIR\help\title.mdc"
  Delete "$INSTDIR\help\transpose.mdc"
  Delete "$INSTDIR\help\try.mdc"
  Delete "$INSTDIR\help\typeof.mdc"
  Delete "$INSTDIR\help\uint8.mdc"
  Delete "$INSTDIR\help\uint16.mdc"
  Delete "$INSTDIR\help\uint32.mdc"
  Delete "$INSTDIR\help\useimage.mdc"
  Delete "$INSTDIR\help\useplot.mdc"
  Delete "$INSTDIR\help\var.mdc"
  Delete "$INSTDIR\help\varargin.mdc"
  Delete "$INSTDIR\help\varargout.mdc"
  Delete "$INSTDIR\help\where.mdc"
  Delete "$INSTDIR\help\which.mdc"
  Delete "$INSTDIR\help\while.mdc"
  Delete "$INSTDIR\help\who.mdc"
  Delete "$INSTDIR\help\xlabel.mdc"
  Delete "$INSTDIR\help\ylabel.mdc"
  Delete "$INSTDIR\help\zeros.mdc"
  Delete "$INSTDIR\help\zoom.mdc"
  Delete "$INSTDIR\MFiles\clusterboot.m"
  Delete "$INSTDIR\MFiles\copper.m"
  Delete "$INSTDIR\MFiles\exit.m"
  Delete "$INSTDIR\MFiles\eye.m"
  Delete "$INSTDIR\MFiles\fliplr.m"
  Delete "$INSTDIR\MFiles\flipud.m"
  Delete "$INSTDIR\MFiles\gray.m"
  Delete "$INSTDIR\MFiles\imagez.m"
  Delete "$INSTDIR\MFiles\length.m"
  Delete "$INSTDIR\MFiles\linspace.m"
  Delete "$INSTDIR\MFiles\log2.m"
  Delete "$INSTDIR\MFiles\mprintimage.m"
  Delete "$INSTDIR\MFiles\mprintplot.m"
  Delete "$INSTDIR\MFiles\popdir.m"
  Delete "$INSTDIR\MFiles\pushdir.m"
  Delete "$INSTDIR\MFiles\rawread.m"
  Delete "$INSTDIR\MFiles\rcall.m"
  Delete "$INSTDIR\MFiles\rescan.m"
  Delete "$INSTDIR\MFiles\reshape.m"
  Delete "$INSTDIR\MFiles\rserver.m"
  Delete "$INSTDIR\MFiles\sqrt.m"
  Delete "$INSTDIR\MFiles\std.m"
  Delete "$INSTDIR\freemat.chm"
  Delete "$INSTDIR\FreeMat.exe"

  Delete "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Website.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Help.lnk"
  Delete "$DESKTOP\FreeMat.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\FreeMat.lnk"

  RMDir "$SMPROGRAMS\$ICONS_GROUP"
  RMDir "$INSTDIR\MFiles"
  RMDir "$INSTDIR\help"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  Push "FREEMAT_PATH"
  Call un.DeleteEnvStr
  SetAutoClose true
SectionEnd
