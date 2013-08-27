CC = icc
CCOPTS = /Ss /Rn /C
LINK = link386
LINKOPTS =

float.exe: float.obj float.mak
    $(LINK) float.obj;

.c.obj:
   $(CC) $(CCOPTS) -C .\$*.c
