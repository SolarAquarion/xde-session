
AM_CFLAGS = $(X11_CFLAGS) $(XEXT_CFLAGS) $(XSCRNSAVER_CFLAGS) $(XINERAMA_CFLAGS) $(XRANDR_CFLAGS) $(CAIRO_CFLAGS) $(GDK_CFLAGS) $(GTK_CFLAGS) $(WNCK_CFLAGS)
AM_LDFLAGS = $(WNCK_LIBS) $(GTK_LIBS) $(GDK_LIBS) $(CAIRO_LIBS) $(XINERAMA_LIBS) $(XRANDR_LIBS) $(XSCRNSAVER_LIBS) $(XEXT_LIBS) $(X11_LIBS)

EXTRA_PROGRAMS =

bin_PROGRAMS = \
	test-login \
	test-root \
	xde-autostart \
	xde-chooser \
	xde-dockmgr \
	xde-input \
	xde-login \
	xde-logout \
	xde-session \
	xde-sm \
	xde-smproxy \
	xde-traymgr \
	xde-wait \
	xde-wmproxy \
	xde-xchooser \
	xde-xlock \
	xde-xlogin \
	xde-xsession


xde_autostart_CPPFLAGS = -DNAME=\"xde-autostart\"
xde_autostart_SOURCES = xde-autostart.c
xde_autostart_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS)
xde_autostart_LDFLAGS = $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

xde_chooser_CPPFLAGS = -DNAME=\"xde-chooser\"
xde_chooser_SOURCES = xde-chooser.c
xde_chooser_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS)
xde_chooser_LDFLAGS = $(AM_LDFLAGS) $(SMLIB_LIBS) $(ICE_LIBS)

xde_dockmgr_CPPFLAGS = -DNAME=\"xde-dockmgr\"
xde_dockmgr_SOURCES = xde-dockmgr.c
xde_dockmgr_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS)
xde_dockmgr_LDFLAGS = $(AM_LDFLAGS) $(SMLIB_LIBS) $(ICE_LIBS)

xde_input_CPPFLAGS = -DNAME=\"xde-input\"
xde_input_SOURCES = xde-input.c
xde_input_CFLAGS = $(AM_CFLAGS)
xde_input_LDFLAGS = $(AM_LDFLAGS)

xde_login_CPPFLAGS = -DNAME=\"xde-login\"
xde_login_SOURCES = xde-login.c
xde_login_CFLAGS = $(AM_CFLAGS) $(XAU_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS) $(SYSTEMD_CFLAGS)
xde_login_LDFLAGS = $(SMLIB_LIBS) $(ICE_LIBS) $(XAU_LIBS) -lpam_misc -lpam $(SYSTEMD_LIBS) $(AM_LDFLAGS)

xde_logout_CPPFLAGS = -DNAME=\"xde-logout\"
xde_logout_SOURCES = xde-logout.c
xde_logout_CFLAGS = $(AM_CFLAGS) $(XVNC_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS) $(SYSTEMD_CFLAGS)
xde_logout_LDFLAGS = $(SMLIB_LIBS) $(XVNC_LIBS) $(ICE_LIBS) $(SYSTEMD_LIBS) $(AM_LDFLAGS)

xde_session_CPPFLAGS = -DNAME=\"xde-session\"
xde_session_SOURCES = xde-session.c
xde_session_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) $(GOBJ_CFLAGS) $(UNIQUE_CFLAGS)
xde_session_LDFLAGS = $(UNIQUE_LIBS) $(GOBJ_LIBS) $(GLIB_LIBS) $(AM_LDFLAGS)

xde_smproxy_CPPFLAGS = -DNAME=\"xde-smproxy\"
xde_smproxy_SOURCES = xde-smproxy.c
xde_smproxy_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS)
xde_smproxy_LDFLAGS = $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

xde_sm_CPPFLAGS = -DNAME=\"xde-sm\"
xde_sm_SOURCES = xde-sm.c
xde_sm_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS)
xde_sm_LDFLAGS = $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

xde_traymgr_CPPFLAGS = -DNAME=\"xde-traymgr\"
xde_traymgr_SOURCES = xde-traymgr.c
xde_traymgr_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS)
xde_traymgr_LDFLAGS = $(AM_LDFLAGS) $(SMLIB_LIBS) $(ICE_LIBS)

xde_wait_CPPFLAGS = -DNAME=\"xde-wait\"
xde_wait_SOURCES = xde-wait.c
xde_wait_CFLAGS = $(X11_CFLAGS)
xde_wait_LDFLAGS = $(X11_LIBS)

xde_wmproxy_CPPFLAGS = -DNAME=\"xde-wmproxy\"
xde_wmproxy_SOURCES = xde-wmproxy.c
xde_wmproxy_CFLAGS = $(AM_CFLAGS) $(ICE_CFLAGS) $(SMLIB_CFLAGS)
xde_wmproxy_LDFLAGS = $(SMLIB_LIBS) $(ICE_LIBS) $(AM_LDFLAGS)

xde_xchooser_CPPFLAGS = -DNAME=\"xde-xchooser\"
xde_xchooser_SOURCES = xde-xchooser.c
xde_xchooser_CFLAGS = $(AM_CFLAGS) $(XDMCP_CFLAGS) $(XVNC_CFLAGS) $(XFT_CFLAGS) $(SYSTEMD_CFLAGS)
xde_xchooser_LDFLAGS = -lpam $(XDMCP_LIBS) $(XVNC_LIBS) $(XFT_LIBS) $(SYSTEMD_LIBS) $(AM_LDFLAGS)

xde_xlock_CPPFLAGS = -DNAME=\"xde-xlock\"
xde_xlock_SOURCES = xde-xlock.c
xde_xlock_CFLAGS = $(AM_CFLAGS) $(XVNC_CFLAGS) $(XFT_CFLAGS) $(SYSTEMD_CFLAGS)
xde_xlock_LDFLAGS = -lpam $(XVNC_LIBS) $(XFT_LIBS) $(SYSTEMD_LIBS) $(AM_LDFLAGS)

xde_xlogin_CPPFLAGS = -DNAME=\"xde-xlogin\"
xde_xlogin_SOURCES = xde-xlogin.c
xde_xlogin_CFLAGS = $(AM_CFLAGS) $(XDMCP_CFLAGS) $(XVNC_CFLAGS) $(XFT_CFLAGS) $(SYSTEMD_CFLAGS)
xde_xlogin_LDFLAGS = -lpam $(XDMCP_LIBS) $(XVNC_LIBS) $(XFT_LIBS) $(SYSTEMD_LIBS) $(AM_LDFLAGS)

xde_xsession_CPPFLAGS = -DNAME=\"xde-xsession\"
xde_xsession_CFLAGS = $(AM_CFLAGS) $(XFT_CFLAGS)
xde_xsession_LDFLAGS = $(XFT_LIBS) $(AM_LDFLAGS)
xde_xsession_SOURCES = \
                xde-xsession.c xde-xsession.h \
                xsessions.c xsession.h \
                autostart.c autostart.h \
                display.c display.h

test_login_CPPFLAGS = -DNAME=\"test-login\"
test_login_CFLAGS = $(AM_CFLAGS) $(SYSTEMD_CFLAGS)
test_login_LDFLAGS = $(SYSTEMD_LIBS) $(AM_LDFLAGS)
test_login_SOURCES = test-login.c xde-xsession.h

test_root_CPPFLAGS = -DNAME=\"test-root\"
test_root_CFLAGS = $(AM_CFLAGS) $(SYSTEMD_CFLAGS)
test_root_LDFLAGS = $(SYSTEMD_LIBS) $(AM_LDFLAGS)
test_root_SOURCES = test-root.c

