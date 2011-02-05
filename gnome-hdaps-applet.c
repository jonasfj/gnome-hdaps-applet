/*
 * gnome-hdaps-applet
 *
 * Copyright (C) 2006 - 2008 Jon Escombe <lists@dresco.co.uk>
 *				 2011 Jonas Finnemann Jensen <jopsen@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <fcntl.h>
#include <string.h>
#include <panel-applet.h>
#include <gtk/gtk.h>
#include <gio/gio.h>

#define SYSFS_PROTECT_FILE_HDA		"/sys/block/hda/device/unload_heads"
#define SYSFS_PROTECT_FILE_SDA		"/sys/block/sda/device/unload_heads"

#define PIXMAP_HDAPS_ERROR			"/usr/share/pixmaps/gnome-hdaps-applet/hdaps-error-small.png"
#define PIXMAP_HDAPS_RUNNING		"/usr/share/pixmaps/gnome-hdaps-applet/hdaps-running-small.png"
#define PIXMAP_HDAPS_PAUSED			"/usr/share/pixmaps/gnome-hdaps-applet/hdaps-paused-small.png"

void monitor_changed (GFileMonitor *monitor,
					  GFile *file,
					  GFile *other_file,
					  GFileMonitorEvent event_type,
					  gpointer gdata)
{
	GtkWidget *image = gdata;
	GtkWidget *applet;
	GtkTooltipsData *tooltips_data;
	static int timerval = 0;
	gchar *text_label;

	applet = image->parent;
	tooltips_data = gtk_tooltips_data_get (applet);

	//Read file
	char* contents = NULL;
	int status = -1;
	if( g_file_load_contents (file, NULL, &contents, NULL, NULL, NULL) ) {
		status = atoi(contents);
		g_free(contents);
	}

	//Update UI acoording to status
	if (status < 0) {
		gtk_image_set_from_file (GTK_IMAGE (image), PIXMAP_HDAPS_ERROR);
		gtk_tooltips_set_tip (tooltips_data->tooltips, applet,
				"Error in Hard Drive Protection System", NULL);
	} else if (status == 0) {
		gtk_image_set_from_file (GTK_IMAGE (image), PIXMAP_HDAPS_RUNNING);
		gtk_tooltips_set_tip (tooltips_data->tooltips, applet,
				"Hard Drive Protection System is protecting your data", NULL);
	} else if (status > 0) {
		gtk_image_set_from_file (GTK_IMAGE (image), PIXMAP_HDAPS_PAUSED);
		gtk_tooltips_set_tip (tooltips_data->tooltips, applet,
				"Hard Drive Protection System has paused the "
				"hard drive to protect your data", NULL);
	}
}

static gboolean gnome_hdaps_applet_fill (PanelApplet *applet, const gchar *iid, gpointer data)
{
	GtkWidget *image;
	GtkTooltips *tooltips;

	if (strcmp (iid, "OAFIID:GNOME_HDAPS_STATUSApplet") != 0)
		return FALSE;

	//Create UI
	image = gtk_image_new_from_file (PIXMAP_HDAPS_ERROR);
	gtk_container_add (GTK_CONTAINER (applet), image);
	tooltips = gtk_tooltips_new ();
	gtk_tooltips_enable (tooltips);
	gtk_tooltips_set_tip (tooltips, GTK_WIDGET (applet), "Starting HDAPS monitor applet.", NULL);
	gtk_widget_show_all (GTK_WIDGET (applet));
	panel_applet_set_background_widget (PANEL_APPLET (applet), GTK_WIDGET (applet));

	//Find file to monitor
	GFile* file = NULL;
	file = g_file_new_for_path(SYSFS_PROTECT_FILE_SDA);
	if(!g_file_query_exists(file, NULL)){
		g_object_unref(file);
		file = g_file_new_for_path(SYSFS_PROTECT_FILE_HDA);
		if(!g_file_query_exists(file, NULL)){
			g_object_unref(file);
			file = NULL;
		}
	}

	//Create file monitor
	GFileMonitor* monitor = NULL;
	if(file)
		monitor = g_file_monitor_file(file, G_FILE_MONITOR_EVENT_CHANGED, NULL, NULL);
	//Configure monitor if there was no error
	if( monitor ) {
		//Limit change events to one every 100 ms
		g_file_monitor_set_rate_limit(monitor, 100);
		//Connect to file changed signal
		g_signal_connect(monitor, "changed", (GCallback)monitor_changed, (gpointer)image);
		//Update the UI now
		monitor_changed(monitor, file, NULL, G_FILE_MONITOR_EVENT_CHANGED, (gpointer)image);
		//Remove reference to file
		g_object_unref(file);
	} else	//Show error tooltip
		gtk_tooltips_set_tip (tooltips, GTK_WIDGET (applet), "Failed to monitor sysfs.", NULL);

	return TRUE;
}

PANEL_APPLET_BONOBO_FACTORY ("OAFIID:GNOME_HDAPS_STATUS_Factory",
                             PANEL_TYPE_APPLET,
                             "HDAPS Status Applet",
                             "0",
                             gnome_hdaps_applet_fill,
                             NULL);
