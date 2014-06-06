/*****************************************************************************

 Copyright (c) 2008-2014  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 *****************************************************************************/

#include "xde-logout.h"

typedef enum {
	LOGOUT_ACTION_CANCEL,
	LOGOUT_ACTION_POWEROFF,
	LOGOUT_ACTION_REBOOT,
	LOGOUT_ACTION_SUSPEND,
	LOGOUT_ACTION_HIBERNATE,
	LOGOUT_ACTION_HYBRIDSLEEP,
	LOGOUT_ACTION_SWITCHUSER,
	LOGOUT_ACTION_SWITCHDESK,
	LOGOUT_ACTION_LOCKSCREEN,
	LOGOUT_ACTION_LOGOUT,
	LOGOUT_ACTION_RESTART,
} LogoutActionResult;

LogoutActionResult logout_result = LOGOUT_ACTION_CANCEL;

struct available_functions {
	gboolean lockscreen;		/* can we lock the screen? */
	gboolean power_off;		/* can we power off the computer? */
	gboolean reboot;		/* can we reboot the computer? */
	gboolean suspend;		/* can we suspend the computer? */
	gboolean hibernate;		/* can we hibernate the computer? */
	gboolean hybrid_sleep;		/* can we hybrid_sleep the computer? */
};

struct available_functions af = {
	.lockscreen = FALSE,
	.power_off = FALSE,
	.reboot = FALSE,
	.suspend = FALSE,
	.hibernate = FALSE,
	.hybrid_sleep = FALSE,
};

/*
 * Determine whether we have been invoked under a session running lxsession(1).
 * When that is the case, we simply execute lxsession-logout(1) with the
 * appropriate parameters for branding.  In that case, this method does not
 * return (executes lxsession-logout directly).  Otherwise the method returns.
 * This method is currently unused and is deprecated.
 */
void
lxsession_check()
{
}

struct prog_cmd {
	char *name;
	char *cmd;
};

/*
 * Test to see whether the caller specified a lock screen program. If not,
 * search through a short list of known screen lockers, searching PATH for an
 * executable of the corresponding name, and when one is found, set the screen
 * locking program to that function.  We could probably easily write our own
 * little screen locker here, but I don't have the time just now...
 *
 * These are hardcoded.  Sorry.  Later we can try to design a reliable search
 * for screen locking programs in the XDG applications directory or create a
 * "sensible-" or "preferred-" screen locker shell program.  That would be
 * useful for menus too.
 */
void
test_lock_screen_program()
{
	static const struct prog_cmd progs[6] = {
		/* *INDENT-OFF* */
		{"xlock",	    "xlock -mode blank &"   },
		{"slock",	    "slock &"		    },
		{"slimlock",	    "slimlock &"	    },
		{"i3lock",	    "i3lock -c 000000 &"    },
		{"xscreensaver",    "xscreensaver -lock &"  },
		{ NULL,		     NULL		    }
		/* *INDENT-ON* */
	};
	struct prog_cmd *prog;

	if (!options.lockscreen) {
		for (prog = progs; prog->name; prog++) {
			char *paths = strdup(getenv("PATH") ? : "");
			char *p = paths - 1;
			char *e = paths + strlen(paths);
			char *b, *path;
			struct stat st;
			int status;

			while ((b = p + 1) < e) {
				*(p = strchrnul(b, ":")) = '\0';
				len = strlen(b) + 1 + strlen(prog->name) + 1;
				path = calloc(len, sizeof(*path));
				strncpy(path, b, len);
				strncat(path, "/", len);
				strncat(path, prog->name, len);
				status = stat(path, &st);
				free(path);
				if (status == 0 && S_ISREG(st.st_mode) && (st.st_mode & S_IXOTH)) {
					options.lockscreen = strdup(prog->cmd);
					break 2;
				}
			}
		}
	}
	if (options.lockscreen)
		af.lockscreen = TRUE;
	return;
}

/*
 * Uses DBUsGProxy and the login1 service to test for available power functions.
 * The results of the test are stored in the corresponding booleans in the
 * available functions structure.
 */
void
test_power_functions()
{
	GError *error = NULL;
	DbusGConnection *bus = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);
	DBusGProxy *proxy = dbus_g_proxy_new_for_name(bus, "org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager");

	dbus_g_proxy_call(proxy, "CanPowerOff", &error, G_TYPE_INVALID, G_TYPE_BOOLEAN, &af.power_off, G_TYPE_INVALID);
	dbus_g_proxy_call(proxy, "CanReboot", &error, G_TYPE_INVALID, G_TYPE_BOOLEAN, &af.reboot, G_TYPE_INVALID);
	dbus_g_proxy_call(proxy, "CanSuspend", &error, G_TYPE_INVALID, G_TYPE_BOOLEAN, &af.suspend, G_TYPE_INVALID);
	dbus_g_proxy_call(proxy, "CanHibernate", &error, G_TYPE_INVALID, G_TYPE_BOOLEAN, &af.hibernate, G_TYPE_INVALID);
	dbus_g_proxy_call(proxy, "CanHybridSleep", &error, G_TYPE_INVALID, G_TYPE_BOOLEAN, &af.hybrid_sleep, G_TYPE_INVALID);
	g_object_unref(G_OBJECT(proxy));
}

/*
 * Transform the a window into a window that has a grab on the pointer on a
 * GtkWindow and restricts pointer movement to the window boundary.
 */
void
grabbed_window(GtkWindow * window)
{
	GdkWindow *win = gtk_widget_get_window(GTK_WIDGET(window));
	GEventMask mask = GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK;

	gdk_window_set_override_redirect(win, TRUE);
	gdk_window_set_focus_on_map(win, TRUE);
	gdk_window_set_accept_focus(win, TRUE);
	gdk_window_set_keep_above(win, TRUE);
	gdk_window_set_modal_hint(win, TRUE);
	gdk_window_stick(win);
	gdk_window_deiconify(win);
	gdk_window_show(win);
	gdk_window_focus(win, GDK_CURRENT_TIME);
	if (gdk_keyboard_grab(win, TRUE, GDK_CURRENT_TIME) != GDK_GRAB_SUCCESS)
		fprintf(stderr, "Could not grab keyboard!\n");
	if (gdk_pointer_grab(win, TRUE, mask, win, NULL, GDK_CURRENT_TIME) != GDK_GRAB_SUCCESS)
		fprintf(stderr, "Could not grab pointer!\n");
}

/*
 * Transform the window back into a regular window, releasing the pointer and
 * keyboard grab and motion restriction.  window is the GtkWindow that
 * previously had the grabbed_window() method called on it.
 */
void
ungrabbed_window(GtkWindow *window)
{
	GdkWindow *win = gtk_widget_get_window(GTK_WIDGET(window));

	gdk_keyboard_ungrab(GDK_CURRENT_TIME);
	gdk_pointer_ungrab(GDK_CURRENT_TIME);
	gdk_window_hide(win);
}

/*
 * Simple dialog prompting the user with a yes or no question; however, the
 * window is one that was previously grabbed using grabbed_window().  This
 * method hands the focus grab to the dialog and back to the window on exit.
 * Returns the response to the dialog.
 */
gboolean
areyousure(GtkWindow *window, char *message)
{
	GtkWidget *d;
	gint result;

	ungrabbed_window(window);
	d = gtk_message_dialog_new(window, GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
			"%s", message);
	gtk_window_set_title(GTK_WINDOW(d), "Are you sure?");
	gtk_window_set_modal(GTK_WINDOW(d), TRUE);
	gtk_window_set_gravity(GTK_WINDOW(d), GDK_GRAVITY_CENTER);
	gtk_window_set_type_hint(GTK_WINDOW(d), GDK_WINDOW_TYPE_HINT_SPLASHSCREEN);
	gtk_window_set_skip_pager_hint(GTK_WINDOW(d), TRUE);
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(d), TRUE);
	gtk_window_set_position(GTK_WINDOW(d), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_widget_show_all(GTK_WIDGET(d));
	gtk_widget_show_now(GTK_WIDGET(d));
	grabbed_window(GTK_WINDOW(d));
	result = gtk_dialog_run(GTK_DIALOG(d));
	ungrabbed_window(GTK_DINWO(d));
	gtk_window_destroy(GTK_WINDOW(d));
	if (result == GTK_RESPONSE_YES)
		grabbed_window(window);
	return((result == GTK_RESPONSE_YES) ? TRUE : FALSE);
}

gboolean
on_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	logout_result = LOGOUT_ACTION_CANCEL;
	gtk_main_quit();
	return TRUE; /* propagate */
}

gboolean
on_expose_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	GdkPixbuf *p = GDK_PIXBUF(data);
	GdkWindow *w = gtk_widget_get_window(GTK_WIDGET(widget));
	cairo_t *cr = gdk_cairo_create(GDK_DRAWABLE(w));
	gdk_cairo_set_source_pixbuf(cr, p, 0, 0);
	cairo_paint(cr);
	GdkColor color = { .red = 0, .green = 0, .blue = 0, .pixel = 0, };
	gdk_set_source_color(cr, &color);
	cairo_paint_with_alpha(cr, 0.7);
	return FALSE;
}


LogoutActionResult
make_logout_choice()
{
	GtkWidget *w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(w), "delete-event", G_CALLBACK(on_delete_event), (gpointer) NULL);
	gtk_window_set_wmclass(GTK_WINDOW(w), "xde-logout", "XDE-Logout");
	gtk_window_set_title(GTK_WINDOW(w), "XDE Session Logout");
	gtk_window_set_modal(GTK_WINDOW(w), TRUE);
	gtk_window_set_gravity(GTK_WINDOW(w), GDK_GRAVITY_CENTER);
	gtk_window_set_type_hint(GTK_WINDOW(w), GDK_WINDOW_TYPE_HINT_SPLASHSCREEN);
	gtk_window_set_icon_name(GTK_WINDOW(w), "xdm");
	gtk_container_set_border_width(GTK_CONTAINER(w), 15);
	gtk_window_set_skip_pager_hint(GTK_WINDOW(w), TRUE);
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(w), TRUE);
	gtk_window_set_position(GTK_WINDOW(w), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_fullscreen(GTK_WINDOW(w));
	gtk_window_set_decorated(GTK_WINDOW(w), FALSE);

	GdkScreen *scrn = gdk_screen_get_default();
	GdkWindow *root = gdk_screen_get_root_window(scrn);
	gint width = gdk_screen_get_width(scrn);
	gint height = gdk_screen_get_height(scrn);

	gtk_window_set_default_size(GTK_WINDOW(w), width, height);
	gtk_window_set_app_paintable(GTK_WINDOW(w), TRUE);

	GdkPixbuf *pixbuf = gdk_pixbuf_get_from_drawable(NULL,
			GDK_DRAWABLE(root), NULL, 0, 0, 0, 0, width, height);
	GtkWidget *a = gtk_alignment_new(0.5, 0.5, 0.0, 0.0);
	gtk_container_add(GTK_CONTAINER(w), GTK_WIDGET(a));
	GtkWidget *e = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(a), GTK_WIDGET(e));
	gtk_widget_set_size_request(GTK_WIDGET(e), -1, -1);
	g_signal_connect(G_OBJECT(w), "expose-event", G_CALLBACK(on_expose_event), (gpointer) pixbuf);

	GtkWidget *v = gtk_vbox_new(FALSE, 5);
	gtk_container_set_border_width(GTK_CONTAINER(v), 15);
	gtk_container_add(GTK_CONTAINER(e), GTK_WIDGET(v));

	GtkWidget *h = gtk_hbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(v), GTK_WIDGET(h));

	if (options.banner) {
		GtkWidget *f = gtk_frame_new();
		gtk_frame_set_shadow_type(GTK_FRAME(f), GTK_SHADOW_ETCHED_IN);
		gtk_box_pack_start(GTK_BOX(h), GTK_WIDGET(f), FALSE, FALSE, 0);

		GtkWidget *v = gtk_vbox_new(FALSE, 5);
		gtk_container_set_border_width(GTK_CONTAINER(v), 10);
		gtk_container_add(GTK_CONTAINER(f), GTK_WIDGET(v));

		GtkWidget *s = gtk_image_new_from_file(options.banner);
		if (s)
			gtk_container_add(GTK_CONTAINER(v), GTK_WIDGET(s));
		else
			gtk_widget_destroy(GTK_WIDGET(f));
	}

	GtkWidget *f = gtk_frame_new();
	gtk_frame_set_shadow_type(GTK_FRAME(f), GTK_SHADOW_ETCHED_IN);
	gtk_box_pack_start(GTK_BOX(h), TRUE, TRUE, 0);
	v = gtk_vbox_new(FALSE, 5);
	gtk_container_set_border_width(GTK_CONTAINER(v), 10);
	gtk_container_add(GTK_CONTAINER(f), GTK_WIDGET(v));
	GtkWidget *l = gtk_label_new();
	gtk_label_set_markup(GTK_LABEL(l), options.prompt);
	gtk_box_pack_start(GTK_BOX(v), GTK_WIDGET(l), FALSE, TRUE, 0);
	GtkWidget *bb = gtk_vbutton_box_new();
	gtk_container_set_border_width(GTK_CONTAINER(bb), 5);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bb), GTK_BUTTONBOX_SPREAD);
	gtk_box_set_spacing(GTK_BOX(bb), 5);
	gtk_box_pack_end(GTK_BOX(v), GTK_WIDGET(bb), FALSE, TRUE, 0);

	GtkListStore *store = gtk_list_store_new(7,
			GDK_TYPE_PIXBUF, /* pixbuf */
			G_TYPE_STRING, /* name */
			G_TYPE_STRING, /* comment */
			G_TYPE_STRING, /* name and comment markup */
			G_TYPE_STRING, /* label */
			G_TYPE_BOOLEAN, /* SessionManaged ? X-XDE-Managed ? */
			G_TYPE_BOOLEAN, /* X-XDE-Managed original setting */
			-1);
	GtkWidget *view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(view), TRUE);
	gtk_tree_view_set_search_column(GTK_TREE_VIEW(view), 1);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(view), FALSE);
	gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(view), GTK_TREE_VIEW_GRID_LINES_BOTH);
	gtk_container_add(GTK_CONTAINER(sw), GTK_WIDGET(view));

	GtkCellRenderer *rend = gtk_cell_renderer_toggle_new();
	gtk_cell_renderer_toggle_set_activatable(GTK_CELL_RENDERER(rend), TRUE);
	g_signal_connect(G_OBJECT(rend), "toggled", G_CALLBACK(on_toggled), NULL);





}


void
action_PowerOff(GtkButton *button, gpointer data)
{
	GtkWidget *top = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gboolen result = areyousure(top, "Are you sure you want power off the computer?");
	action_result = LOGOUT_ACTION_POWEROFF;
	gtk_main_quit();
}

void
action_SwitchUser(GtkButton * button, gpointer data)
{
	fprintf(stderr, "Unimplementation %s\n", __FUNCTION__);
	return;
}

void
action_SwitchDesk(GtkButton * button, gpointer data)
{
	fprintf(stderr, "Unimplementation %s\n", __FUNCTION__);
	return;
}

void
action_LockScreen(GtkButton * button, gpointer data)
{
	if (options.lockscreen)
		system(options.lockscreen);
	else
		fprintf(stderr, "No screen locking program found!\n");
	return;
}

void
action_Logout(GtkButton * button, gpointer data)
{
	const char *env;
	pid_t pid;

	/* Check for _LXSESSION_PID, _FBSESSION_PID, XDG_SESSION_PID.  When one of these exists,
	   logging out of the session consists of sending a TERM signal to the pid concerned.  When 
	   none of these exists, then we can check to see if there is information on the root
	   window. */
	if ((env = getenv("XDG_SESSION_PID")) && (pid = atoi(env))) {
		/* NOTE: we might actually be killing ourselves here...  */
		if (kill(pid, SIGTERM) == 0)
			return;
		fprintf(stderr, "kill: could not kill XDG_SESSION_PID %d: %s\n", (int) pid, strerror(errno));
	}
	if ((env = getenv("_FBSESSION_PID")) && (pid = atoi(env))) {
		/* NOTE: we might actually be killing ourselves here...  */
		if (kill(pid, SIGTERM) == 0)
			return;
		fprintf(stderr, "kill: could not kill _FBSESSION_PID %d: %s\n", (int) pid, strerror(errno));
	}
	if ((env = getenv("_LXSESSION_PID")) && (pid = atoi(env))) {
		/* NOTE: we might actually be killing ourselves here...  */
		if (kill(pid, SIGTERM) == 0)
			return;
		fprintf(stderr, "kill: could not kill _LXSESSION_PID %d: %s\n", (int) pid, strerror(errno));
	}

	GdkDisplayManager *mgr = gdk_display_manager_get();
	GdkDisplay *disp = gdk_display_manager_get_default_display(mgr);
	GdkScreen *scrn = gdk_display_get_default_screen(disp);
	GdkWindow *root = gdk_screen_get_root_window(srcn);

	/* When the _BLACKBOX_PID atom is set on the desktop, that is the PID of the FLUXBOX window 
	   manager.  Actually it is me that is setting _BLACKBOX_PID using the fluxbox init file
	   rootCommand resource. */
	GdkAtom atom, actual;
	gint format, length;
	guchar *data;

	if ((atom = gdk_atom_intern("_BLACKBOX_PID", TRUE)) != GDK_NONE) {
		if (gdk_property_get(root, atom, GDK_NONE, 0, 1, FALSE, &actual, &format, &length, &data) && format == 32 && length >= 1) {
			if ((pid = *(unsigned long *) data)) {
				/* NOTE: we might actually be killing ourselves here...  */
				if (kill(pid, SIGTERM) == 0)
					return;
				fprintf(stderr, "kill: could not kill _BLACKBOX_PID %d: %s\n", (int) pid, strerror(errno));
			}
		}

	}
	/* Openbox sets _OPENBOX_PID atom on the desktop.  It also sets _OB_THEME to the theme
	   name, _OB_CONFIG_FILE to the configuration file in use, and _OB_VERSION to the version
	   of openbox.  __NET_SUPPORTING_WM_CHECK is set to the WM window, which has _NET_WM_NAME
	   set to "Openbox". */
	if ((atom = gdk_atom_intern("_OPENBOX_PID", TRUE)) != GDK_NONE) {
		if (gdk_property_get(root, atom, GDK_NONE, 0, 1, FALSE, &actual, &format, &length, &data) && format == 32 && length >= 1) {
			if ((pid = *(unsigned long *) data)) {
				/* NOTE: we might actually be killing ourselves here...  */
				if (kill(pid, SIGTERM) == 0)
					return;
				fprintf(stderr, "kill: could not kill _OPENBOX_PID %d: %s\n", (int) pid, strerror(errno));
			}
		}
	}
	/* IceWM-Session does not set environment variables nor elements on the root.
	   _NET_SUPPORTING_WM_CHECK is set to the WM window, which has a _NET_WM_NAME set to "IceWM
	   1.3.7 (Linux 3.4.0-ARCH/x86_64)" but also has _NET_WM_PID set to the pid of "icewm". Note 
	   that this is not the pid of icewm-session when that is running. */
	if ((atom = gdk_atom_intern("_NET_SUPPORTING_WM_CHECK", TRUE)) != GDK_NONE) {
		if (gdk_property_get(root, atom, GDK_NONE, 0, 1, FALSE, &actual, &format, &length, &data) && format == 32 && length >= 1) {
			Window xid = *(unsigned long *) data;
			GtkWindow *win = gdk_window_foreign_new(xid);

			if ((atom = gdk_atom_intern("_NET_WM_PID", TRUE)) != GDK_NONE) {
				if (gdk_property_get(win, atom, GDK_NONE, 0, 1, FALSE, &actual, &format, &length, &data) && format == 32 && length >= 1) {
					if ((pid = *(unsigned long *) data)) {
						/* NOTE: we might actually be killing ourselves
						   here...  */
						if (kill(pid, SIGTERM) == 0) {
							g_object_unref(G_OBJECT(win));
							return;
						}
						fprintf(stderr, "kill: could not kill _NET_WM_PID %d: %s\n", (int) pid, %strerror(errno));
					}
				}
			}
			g_object_unref(G_OBJECT(win));
		}
	}
	/* Some window managers set _NET_WM_PID on the root window... */
	if ((atom = gdk_atom_intern("_NET_WM_PID", TRUE)) != GDK_NONE) {
		if (gdk_property_get(root, atom, GDK_NONE, 0, 1, FALSE, &actual, &format, &length, &data) && format == 32 && length >= 1) {
			if ((pid = *(unsigned long *) data)) {
				/* NOTE: we might actually be killing ourselves here... */
				if (kill(pid, SIGTERM) == 0)
					return;
				fprintf(stderr, "kill: could not kill _NET_WM_PID %d: %s\n", (int) pid, %strerror(errno));
			}
		}
	}
	/* We set _XDE_WM_PID on the root window when we were invoked properly to the window
	   manager PID.  Try that next. */
	if ((atom = gdk_atom_intern("_XDE_WM_PID", TRUE)) != GDK_NONE) {
		if (gdk_property_get(root, atom, GDK_NONE, 0, 1, FALSE, &actual, &format, &length, &data) && format == 32 && length >= 1) {
			if ((pid = *(unsigned long *) data)) {
				/* NOTE: we might actually be killing ourselves here... */
				if (kill(pid, SIGTERM) == 0)
					return;
				fprintf(stderr, "kill: could not kill _XDE_WM_PID %d: %s\n", (int) pid, %strerror(errno));
			}
		}
	}
	/* FIXME: we can do much better than this.  See libxde.  In fact, maybe we should use
	   libxde. */
	fprintf(stderr, "ERROR: cannot find session or window manager PID!\n");
	return;
}
