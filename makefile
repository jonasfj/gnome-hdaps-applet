CFLAGS = `pkg-config --cflags --libs libpanelapplet-2.0 gio-2.0`
all: gnome-hdaps-applet
gnome-hdaps-applet: gnome-hdaps-applet.c
	gcc $(CFLAGS) -o $@ $^
	chmod +x gnome-hdaps-applet
install:
	mkdir -p /usr/share/pixmaps/gnome-hdaps-applet/
	cp *.png /usr/share/pixmaps/gnome-hdaps-applet/
	cp GNOME_HDAPS_StatusApplet.server /usr/lib/bonobo/servers/GNOME_HDAPS_StatusApplet.server
	cp gnome-hdaps-applet /usr/bin/gnome-hdaps-applet
remove:
	rm -rf /usr/share/pixmaps/gnome-hdaps-applet/
	rm -f /usr/lib/bonobo/servers/GNOME_HDAPS_StatusApplet.server
	rm -f /usr/bin/gnome-hdaps-applet
clean:
	rm -f gnome-hdaps-applet
