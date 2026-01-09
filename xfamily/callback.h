#ifndef callbacks_h
#define callbacks_h

#include <FL/Fl.H>
#include <FL/Fl_Menu.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Multiline_Input.H>

#include "classes.h"

///////////////// callback routines for the GUI classes ///////////////

// lots more still to add here

// callback routines from mainUI menu applying to whole program

extern void language_cb(Fl_Menu_ *, void *);

// callback routines from mainUI menu applying to whole tree

extern void save_cb(Fl_Menu_ *, void *);
extern void newview_cb(Fl_Menu_ *, void *);

// callback routines for mainUI

extern void quit_cb(Fl_Menu_ *, void *);
extern void killed_cb(Fl_Window*, void* );
extern void popup_cb(Fl_Button*, void*);

// callback routines from mainUI menu to start help

extern void help_cb(Fl_Menu_ *, void* ); // void* points to leafname of helpfile
void shell_for_help( void* ); // also used by callbacks from help buttons

// callback routines for infoUI

extern void info_cb(Fl_Menu_*, void*);
extern void okinfo_cb(Fl_Return_Button*, void*);
extern void gplinfo_cb(Fl_Button*, void*);

// callback routines for indiUI

extern void edit_cb(Fl_Menu_ *, void *);
extern void okedit_cb(Fl_Return_Button *, void *);
extern void canceledit_cb(Fl_Button*, void*);
extern void helpedit_cb(Fl_Button*, void*);

// callback routines for famUI

extern void famed_cb(Fl_Menu_ *, void *);
extern void okfam_cb(Fl_Return_Button *, void *);
extern void cancelfam_cb(Fl_Button*, void*);
extern void helpfam_cb(Fl_Button*, void*);

// callback routines for statsUI

extern void stats_cb(Fl_Menu_ *, void *);

// callback routines for findUI

extern void find_cb(Fl_Menu_ *, void *);
extern void okfind_cb(Fl_Return_Button*, void *);

// callback routines for prefUI

extern void choices_cb(Fl_Menu_ *, void *);
extern void okchoices_cb(Fl_Button*, void*);
extern void cancelchoices_cb(Fl_Button*, void*);
extern void applychoices_cb(Fl_Button*, void*);
extern void helpchoices_cb(Fl_Button*, void*);

// callback routines to drive notesUI

extern void opennotes_cb(Fl_Menu_ *, void *);
extern void changenotes_cb(Fl_Multiline_Input*, void*);
extern void cancelnotes_cb(Fl_Menu_*, void*);
extern void quitnotes_cb(Fl_Window*, void*);
extern void clearnotes_cb(Fl_Menu_*, void*);
extern void restorenotes_cb(Fl_Menu_*,void*);
extern void savenotes_cb(Fl_Menu_*, void*);
extern void oknotes_cb(Fl_Menu_*, void*);

#endif
