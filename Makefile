COMP = gcc
GTKFLAGS = `pkg-config --cflags gtk4`
GTKLIBS = `pkg-config --libs gtk4`
# Variables automáticas
# $@ - Nombre del objetivo
# $^ - Nombres de los prerequisitos
# $? - Nombres de los Prerequisitos mas nuevos que el objetivo

GUI: GUI.c
	$(COMP) -Wall -Wdeprecated-declarations $(GTKFLAGS) $^ -o $@ $(GTKLIBS)