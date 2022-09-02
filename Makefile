COMP = gcc
GTKFLAGS = `pkg-config --cflags gtk4`
GTKLIBS = `pkg-config --libs gtk4`
SETS = -Wall

# Variables automáticas
# $@ - Nombre del objetivo
# $^ - Nombres de los prerequisitos
# $? - Nombres de los Prerequisitos mas nuevos que el objetivo

GUI: GUI.c matriz.o
	$(COMP) $(SETS) -Wdeprecated-declarations $(GTKFLAGS) $^ -o $@ $(GTKLIBS)

matriz.o: matriz.c
	$(COMP) $(SETS)	-c $^ -o $@

GUItest: gui2.c
	$(COMP) $(SETS) -Wdeprecated-declarations $(GTKFLAGS) $^ -o $@ $(GTKLIBS)