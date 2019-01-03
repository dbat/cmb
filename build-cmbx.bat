::@echo OFF
set "COPTS=/nologo /Zi"
set "ASOPT=/nologo /c /Zi /Fl"

set "COPTS=/nologo"
set "ASOPT=/nologo /c /Fl"

set "asm=cmpa"
set "csrc=cmbx"

ml %ASOPT% %asm%.asm

set "LNKOP=/link /out:%csrc%.exe"

cl %COPTS% %asm%.obj %csrc%.c %LNKOP%

