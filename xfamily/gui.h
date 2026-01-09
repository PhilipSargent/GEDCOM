// gui.h

#ifndef gui_h
#define gui_h

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Window.H>

#include "classes.h"
#include "family.h"
#include "languages.h"

///////////////// menu structures used by the GUIs ///////////////

class barmenu {
public:
  Fl_Menu_Item menu_data[35+language_count];

  barmenu( mainUI* );
};

class indipopupmenu {
public:
  Fl_Menu_Item popup_data[10];

  indipopupmenu( mainUI* );
};

class fampopupmenu {
public:
  Fl_Menu_Item popup_data[6];

  fampopupmenu( mainUI* );
};

class notesmenu {
  Fl_Menu_Item menu_data[6];
public:
  notesmenu( notesUI* );
};

///////////////// GUI classes - one for each type of window ///////////////

class mainUI {
//
// defines main display window and its menu bar - there is one instanciation
// of this window for each view of each tree we are editing
//

  treeinstance *tree;    // the tree we are invoked for
  mainUI *next;          // the next view of the tree (or NULL)
  int maxX;              // the canvas size we need to display
  int maxY;              // this view of the tree
  GEDCOM_object *person;
  GEDCOM_object *topind;
  GEDCOM_string *title;
  displaytree *display;

public: // this lot should become private v. soon !

  Fl_Window      *window;
  Fl_Menu_Bar    *main_menu;
  Fl_Scroll      *scroll;
  treedisplay    *main;
  // the actual menus are identical in all windows, but cannot be static
  // since the user data for each callback has to pass back the
  // mainUI pointer, so we know which dataset to work on.
  barmenu        *mainmenu;
  // size of array is set when the menu structure is defined
  // its not clear that we actually need names to refer to each item
  // so for now we won't bother declaring them
  indipopupmenu  *indimenu;
  fampopupmenu   *fammenu;

  mainUI( treeinstance* ); // our constructor
  void settitle();
  void show();
  void hide();
  void canvassize( int, int );
  mainUI *getnext() const;
  void setnext( mainUI* );
  treeinstance* whichtree() const;
  displaytree* whattodraw() const;
  GEDCOM_object* getcurrent() const;
  void setcurrent( GEDCOM_object* );
  void newdisplay();
  void settop(); // probably should be private
  GEDCOM_object* gettop() const;

//  void calc();
//  void calcfam();
//  void cleardisplay() const;

};

////////////////////////////////////////////////////////////////////////////////

class infoUI {
//
// defines "About" box - only one of these needed no matter how many
// instanciations of the main window we have
//
public:
  Fl_Window *info_panel;
  Fl_Group  *info_group;
  Fl_Box    *info_name;
  Fl_Box    *author;
  Fl_Box    *version;
  Fl_Box    *purpose;
  Fl_Box    *name_l;
  Fl_Box    *author_l;
  Fl_Box    *version_l;
  Fl_Box    *purpose_l;
  Fl_Button *gpl_button;
  Fl_Box    *gpl_box;
  Fl_Return_Button *info_ok;

  infoUI();
  void show() const;
};

////////////////////////////////////////////////////////////////////////////////

class indiUI {
//
// the user interface for editing an individual's details
// there is must be only one instanciation of this dbox for any one
// individual. The input values must be personalised for each individual.
// The labels remain unchanged except for the window title which should
// be "Edit John Doe (Innnn in test.ged)" or "Edit new person in test.ged",
// where the title reflects the state when editing started. If the dbox
// was opened in response to the "child of" menu item, we should be able
// to deduce a surname and we could use "Edit new child Bloggs"

  GEDCOM_object *who;
  treeinstance  *which;
  indiUI        *next;
  indiUI        *previous;
  char buf_indi_title[MAX_indititle];

public:
  Fl_Window *indi_dbox;
  Fl_Input  *indi_name;
  Fl_Input  *indi_title;
  Fl_Light_Button *indi_male;
  Fl_Light_Button *indi_female;
  Fl_Light_Button *indi_unknown;
  Fl_Input  *indi_ma;
  Fl_Input  *indi_pa;
  Fl_Check_Button *indi_living;
  Fl_Tabs   *indi_events;
  Fl_Tile   *indi_birth;
  Fl_Input  *indi_birthdate;
  Fl_Input  *indi_birthtime;
  Fl_Input  *indi_birthplace;
  Fl_Input  *indi_birthsite;
  Fl_Tile   *indi_chr;
  Fl_Input  *indi_chrdate;
  Fl_Input  *indi_chrtime;
  Fl_Input  *indi_chrplace;
  Fl_Input  *indi_chrsite;
  Fl_Tile   *indi_bapm;
  Fl_Input  *indi_bapmdate;
  Fl_Input  *indi_bapmtime;
  Fl_Input  *indi_bapmplace;
  Fl_Input  *indi_bapmsite;
  Fl_Tile   *indi_death;
  Fl_Input  *indi_deathdate;
  Fl_Input  *indi_deathtime;
  Fl_Input  *indi_deathplace;
  Fl_Input  *indi_deathsite;
  Fl_Input  *indi_deathcause;
  Fl_Tile   *indi_crem;
  Fl_Input  *indi_cremdate;
  Fl_Input  *indi_cremtime;
  Fl_Input  *indi_cremplace;
  Fl_Input  *indi_cremsite;
  Fl_Tile   *indi_buri;
  Fl_Input  *indi_buridate;
  Fl_Input  *indi_buritime;
  Fl_Input  *indi_buriplace;
  Fl_Input  *indi_burisite;
  Fl_Input  *indi_buriplot;
  Fl_Button *indi_cancel;
  Fl_Return_Button *indi_ok;
  Fl_Button *indi_help;

  indiUI( treeinstance*, GEDCOM_object* = 0 );
  ~indiUI();
  GEDCOM_object* whois() const;
  treeinstance* whichis() const;
  indiUI* getnext() const;
  void setnext(indiUI*);
  indiUI* getprevious() const;
  void setprevious( indiUI*);
  void clear_details();
  void insert_details(GEDCOM_object*);
  void settitle();
  void show() const;
  void hide() const;
};

////////////////////////////////////////////////////////////////////////////////

class famUI {
//
// the user interface for editing a family's details (by which we mean
// dates and times of marriage / engagement / divorce, not who is a
// member of the family, which is done via menu in mainUI)
// there is must be only one instanciation of this dbox for any one
// family. The input values must be personalised for each family.
// The labels remain unchanged except for the window title which should
// be "Edit John Doe = Mary Smith (Fnnnn in test.ged)"
// where the title reflects the state when editing started (indeed, this
// UI should not be able to change names, so title always OK).

  GEDCOM_object *fam;
  treeinstance  *which;
  famUI         *next;
  famUI         *previous;
  char buf_fam_title[MAX_famtitle];

public:
  Fl_Window *fam_dbox;
  Fl_Output *husb_name;
  Fl_Output *wife_name;
  Fl_Tabs   *fam_events;
  Fl_Tile   *fam_engage;
  Fl_Input  *fam_engagedate;
  Fl_Input  *fam_engagetime;
  Fl_Input  *fam_engageplace;
  Fl_Input  *fam_engagesite;
  Fl_Tile   *fam_marry;
  Fl_Input  *fam_marrydate;
  Fl_Input  *fam_marrytime;
  Fl_Input  *fam_marryplace;
  Fl_Input  *fam_marrysite;
// potentially we should be able to add things like witnesses, and of
// course, a source reference
  Fl_Tile   *fam_divorce;
  Fl_Input  *fam_divorcedate;
  Fl_Input  *fam_divorcetime;
  Fl_Input  *fam_divorceplace;
  Fl_Input  *fam_divorcesite;
  Fl_Button *fam_cancel;
  Fl_Return_Button *fam_ok;
  Fl_Button *fam_help;

  famUI( treeinstance*, GEDCOM_object* = 0 );
  ~famUI();
  GEDCOM_object* whatis() const;
  treeinstance* whichis() const;
  famUI* getnext() const;
  void setnext(famUI*);
  famUI* getprevious() const;
  void setprevious( famUI*);
  void clear_details();
  void insert_details(GEDCOM_object*);
  void settitle();
  void show() const;
  void hide() const;
};

////////////////////////////////////////////////////////////////////////////////

class statsUI {
  treeinstance *which;

public:
  Fl_Window *statsdbox;
  Fl_Output *statsfile;
  Fl_Output *indicount;
  Fl_Output *indimax;
  Fl_Output *famcount;
  Fl_Output *fammax;
  Fl_Output *srccount;
  Fl_Output *srcmax;
  Fl_Output *repocount;
  Fl_Output *repomax;

  statsUI( treeinstance* );
  bool shown() const;
  void hide() const;
  void setvalues();
  void show() const;
};

////////////////////////////////////////////////////////////////////////////////

class findUI {
public:
  Fl_Window* findbox;
  Fl_Input* find_input;
  Fl_Return_Button* find_ok;

  findUI();
  void setview( mainUI* );
};

////////////////////////////////////////////////////////////////////////////////

class prefUI {
public:
  Fl_Window *optsbox;
  Fl_Tabs *choices;
  Fl_Tile *nameprefs;
  Fl_Check_Button *showfam;
  Fl_Check_Button *showtitle;
  Fl_Tile *dateprefs;
  Fl_Check_Button *treedates;
  Fl_Check_Button *spousedates;
  Fl_Check_Button *marrdates;
  Fl_Check_Button *yearonly;
  Fl_Light_Button *prefgedcom;
  Fl_Light_Button *prefios;
  Fl_Tile *miscopts;
  Fl_Group *xref;
  Fl_Box *xref1;
  Fl_Light_Button *xrefquit;
  Fl_Light_Button *xrefwarn;
  Fl_Light_Button *xrefignore;
  Fl_Group *datestamp;
  Fl_Box *datestamp1;
  Fl_Light_Button *stampiso;
  Fl_Light_Button *stampgedcom;
  Fl_Light_Button *stamplocal;
  Fl_Light_Button *stampother;
  Fl_Input *stampformat;
  Fl_Button *cancelpref;
  Fl_Button *applypref;
  Fl_Return_Button *okpref;
  Fl_Button *helppref;

  prefUI();
};

////////////////////////////////////////////////////////////////////////////////

class notesUI {
  notesUI    *nextui;
  notesUI    *previousui;
  notesmenu  *menu;
  char       notes_title[MAX_notestitle];
  treeinstance  *edittree;   // which tree - we have to know so we can mark it
  // as modified when we save the changed notes back, and so we can put it in
  // the window title - the user needs to know too !
  GEDCOM_object *editobject; // which object we are editing
  GEDCOM_tag *edittag;   // which subobject (NOTE or TEXT)
  int size;
  bool modified;

public:
  Fl_Window   *window;
  Fl_Menu_Bar *menubar;
  Fl_Input    *input;

  notesUI( treeinstance*, GEDCOM_object*, GEDCOM_tag* );
//  ~notesUI();
  notesUI* getnext() const;
  void setnext( notesUI* );
  notesUI* getprevious() const;
  void setprevious( notesUI* );
  GEDCOM_object* object() const;
  void changed(bool);
  bool changed() const;
  void clear();
  void update();
  int restore();
  void retitle();
  void hide();
  void show();
};

#endif
