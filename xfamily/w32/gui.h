// gui.h

#ifndef gui_h
#define gui_h

#include "fixes.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
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
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Editor.H>

#include "menucodes.h"

#include "classes.h"
#include "family.h"

///////////////// menu structures used by the GUIs ///////////////

class barmenu {
public:
  Fl_Menu_Item menu_data[33];

  barmenu( mainUI* );
};

class indipopupmenu {
public:
  Fl_Menu_Item popup_data[imenusize];

  indipopupmenu( mainUI* );
  void grey( int );
  void black( int );
};

class fampopupmenu {
public:
  Fl_Menu_Item popup_data[fmenusize];

  fampopupmenu( mainUI* );
  void grey( int );
  void black( int );
};

class genpopupmenu {
public:
  Fl_Menu_Item popup_data[2];

  genpopupmenu( mainUI* );
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
  genpopupmenu   *genmenu;

  mainUI( treeinstance* ); // our constructor
  ~mainUI(); // we need to have a destructor to get rid of the Window and the display structure
  void settitle();
  void show();
  void hide();
  void canvassize( int, int );
  int canvas_w();
  int canvas_h();
  mainUI *getnext() const;
  void setnext( mainUI* );
  treeinstance* whichtree() const;
  displaytree* whattodraw() const;
  GEDCOM_object* getcurrent() const;
  void setcurrent( GEDCOM_object* );
  void newdisplay();
  void centre_current();
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
// there must be only one instanciation of this dbox for any one
// individual. The input values must be personalised for each individual.
// The labels remain unchanged except for the window title which should
// be "Edit John Doe (Innnn in test.ged)" or "Edit new person in test.ged",
// where the title reflects the state when editing started. If the dbox
// was opened in response to the "child of" menu item, we should be able
// to deduce a surname and we could use "Edit new child Bloggs"

// we created these three pointers under fix0004, in which they could point to
// ephemeral objects which were being built. Now that that approach is backed out
// we have *retained* these three pointers, but now they *may* be NULL and we may
// not change them as a side effect of anything done in this dbox
  GEDCOM_object *mum;    // pointer to the female parent of the person
  GEDCOM_object *dad;    // pointer to the male parent of the person
  GEDCOM_object *fam;    // pointer to a FAM object for the person and parents
  // each (or all) of the above may be ephemeral objects which exists only whilst indiUI open
// end of not-fix0004-anymore
  GEDCOM_object *who;    // pointer to the INDI object representing that person
  treeinstance  *which;  // the treeinstance from which this indiUI was opened
  indiUI        *next;
  indiUI        *previous;
  char buf_indi_title[MAX_indititle];

public:
  Fl_Window *indi_dbox;
  Fl_Input  *indi_name;
  Fl_Output  *indi_id;
  Fl_Input  *indi_title;
  Fl_Light_Button *indi_male;
  Fl_Light_Button *indi_female;
  Fl_Light_Button *indi_unknown;
  Fl_Output  *indi_ma_id;
  Fl_Output  *indi_pa_id;
  Fl_Output *indi_pa;
  Fl_Output *indi_ma;
  Fl_Button *indi_pa_add;
  Fl_Button *indi_ma_add;
  Fl_Button *indi_fam_find;
  Fl_Button *indi_notes;
  Fl_Button *indi_sources;
  Fl_Button *indi_will;
  Fl_Button *indi_probate;
  Fl_Tabs   *indi_events;
  Fl_Tile   *indi_birth;
  Fl_Input  *indi_birthdate;
  Fl_Input  *indi_birthtime;
  Fl_Light_Button *indi_birthgreg;
  Fl_Light_Button *indi_birthjul;
  Fl_Light_Button *indi_birthdef;
  Fl_Input  *indi_birthplace;
  Fl_Input  *indi_birthsite;
//  Fl_Output *indi_birthsrc;
  Fl_Button *indi_birthnotes;
  Fl_Button *indi_birthsources;
  Fl_Tile   *indi_chr;
  Fl_Input  *indi_chrdate;
  Fl_Input  *indi_chrtime;
  Fl_Light_Button *indi_chrgreg;
  Fl_Light_Button *indi_chrjul;
  Fl_Light_Button *indi_chrdef;
  Fl_Input  *indi_chrplace;
  Fl_Input  *indi_chrsite;
//  Fl_Output *indi_chrsrc;
  Fl_Button *indi_chrnotes;
  Fl_Button *indi_chrsources;
  Fl_Tile   *indi_bapm;
  Fl_Input  *indi_bapmdate;
  Fl_Input  *indi_bapmtime;
  Fl_Light_Button *indi_bapmgreg;
  Fl_Light_Button *indi_bapmjul;
  Fl_Light_Button *indi_bapmdef;
  Fl_Input  *indi_bapmplace;
  Fl_Input  *indi_bapmsite;
//  Fl_Output *indi_bapmsrc;
  Fl_Button *indi_bapmnotes;
  Fl_Button *indi_bapmsources;
  Fl_Tile   *indi_death;
  Fl_Input  *indi_deathdate;
  Fl_Input  *indi_deathtime;
  Fl_Input  *indi_deathage;
  Fl_Light_Button *indi_deathgreg;
  Fl_Light_Button *indi_deathjul;
  Fl_Light_Button *indi_deathdef;
  Fl_Input  *indi_deathplace;
  Fl_Input  *indi_deathsite;
  Fl_Input  *indi_deathcause;
//  Fl_Output *indi_deathsrc;
  Fl_Button *indi_deathnotes;
  Fl_Button *indi_deathsources;
  Fl_Tile   *indi_crem;
  Fl_Input  *indi_cremdate;
  Fl_Input  *indi_cremtime;
  Fl_Light_Button *indi_cremgreg;
  Fl_Light_Button *indi_cremjul;
  Fl_Light_Button *indi_cremdef;
  Fl_Input  *indi_cremplace;
  Fl_Input  *indi_cremsite;
//  Fl_Output *indi_cremsrc;
  Fl_Button *indi_cremnotes;
  Fl_Button *indi_cremsources;
  Fl_Tile   *indi_buri;
  Fl_Input  *indi_buridate;
  Fl_Input  *indi_buritime;
  Fl_Light_Button *indi_burigreg;
  Fl_Light_Button *indi_burijul;
  Fl_Light_Button *indi_buridef;
  Fl_Input  *indi_buriplace;
  Fl_Input  *indi_burisite;
  Fl_Input  *indi_buriplot;
//  Fl_Output *indi_burisrc;
  Fl_Button *indi_burinotes;
  Fl_Button *indi_burisources;
  Fl_Button *indi_cancel;
  Fl_Return_Button *indi_ok;
  Fl_Button *indi_help;
  Fl_Button *indi_restore;
  Fl_Button *indi_commit;

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
  void refresh_parents( );
  void setdatefields(GEDCOM_object*, Fl_Input*, Fl_Input*, Fl_Button*, Fl_Button* );
  void checknotes(GEDCOM_object*, bool);
  void settitle();
  void update();
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
  Fl_Output *husb_id;
  Fl_Output *wife_id;
  Fl_Button *husb_add;
  Fl_Button *husb_find;
  Fl_Button *husb_remove;
  Fl_Button *wife_add;
  Fl_Button *wife_find;
  Fl_Button *wife_remove;
  Fl_Button *fam_notes;
  Fl_Button *fam_sources;
  Fl_Tabs   *fam_events;
  Fl_Tile   *fam_engage;
  Fl_Input  *fam_engagedate;
  Fl_Input  *fam_engagetime;
  Fl_Light_Button *fam_engagegreg;
  Fl_Light_Button *fam_engagejul;
  Fl_Light_Button *fam_engagedef;
  Fl_Input  *fam_engageplace;
  Fl_Input  *fam_engagesite;
  Fl_Button *fam_engagenotes;
  Fl_Button *fam_engagesources;
//  Fl_Output *fam_engagesrc;
  Fl_Tile   *fam_marry;
  Fl_Input  *fam_marrydate;
  Fl_Input  *fam_marrytime;
  Fl_Light_Button *fam_marrygreg;
  Fl_Light_Button *fam_marryjul;
  Fl_Light_Button *fam_marrydef;
  Fl_Input  *fam_marryplace;
  Fl_Input  *fam_marrysite;
  Fl_Button *fam_marrynotes;
  Fl_Button *fam_marrysources;
//  Fl_Output *fam_marrysrc;
// potentially we should be able to add things like witnesses, and of
// course, a source reference
  Fl_Tile   *fam_divorce;
  Fl_Input  *fam_divorcedate;
  Fl_Input  *fam_divorcetime;
  Fl_Light_Button *fam_divorcegreg;
  Fl_Light_Button *fam_divorcejul;
  Fl_Light_Button *fam_divorcedef;
  Fl_Input  *fam_divorceplace;
  Fl_Input  *fam_divorcesite;
  Fl_Button *fam_divorcenotes;
  Fl_Button *fam_divorcesources;
//  Fl_Output *fam_divorcesrc;
  Fl_Button *fam_cancel;
  Fl_Return_Button *fam_ok;
  Fl_Button *fam_help;
  Fl_Button *fam_restore;
  Fl_Button *fam_commit;

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
  void refresh_spouse();
  void setdatefields(GEDCOM_object*, Fl_Input*, Fl_Input*, Fl_Button*, Fl_Button* );
  void checknotes(GEDCOM_object*, bool);
  void settitle();
  void update();
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

class indifindUI {
public:
  Fl_Window* findbox;
  Fl_Input* find_input;
  Fl_Output* find_id;
  // the searchfind_cb callback needs to be able to see the search buttons:
  Fl_Button* find_fromright;
  Fl_Button* find_fuzzy;
  Fl_Button* find_fromleft;
  Fl_Button* find_help;
  Fl_Button* find_cancel;
  Fl_Return_Button* find_ok;
  void*    context;
  short strategy; // a bitfield restricting how to search and what to do with results
  // bits 0-1 left/right/fuzzy - gets set in response to which search button pressed
  // in the long term, both bits zero might be used for regular expression search but
  // most genealogists won't grok this.
  // bits 2-3 male/female/any  - gets set according to needs of caller
  // bits 4-...  0 make current, 16 spouse completion, 32 ...
  // wishlist: a bit which asks to deduce and match on married name might be useful

  indifindUI();
  void setcontext( void* );
  void* getcontext() const;
  treeinstance* whichtree();
  void open( void*, short, short, short);
  void searchfind(Fl_Button*);
  void setperson( GEDCOM_object* );
  short getstrategy();
  void OK();
  void finish();
};

////////////////////////////////////////////////////////////////////////////////

class indicompletionsUI {
public:
  Fl_Window* completionswin;
  Fl_Select_Browser* completions;
  treeinstance* scantree;
  char* searchfor;
  int searchmethod;
  indicompletion_item* first;
  int widths[6];

  indicompletionsUI();
  void open( treeinstance*, char*, int, Fl_Callback*, /*void*,*/ short, short );
  void finish();
  GEDCOM_object* chosen_indi( int );
};


////////////////////////////////////////////////////////////////////////////////

class indicompletion_item {
GEDCOM_object*    indiptr;
indicompletion_item*  nextptr;
GEDCOM_string*    displayptr;
int width[6];

public:
  indicompletion_item( GEDCOM_object* );
  ~indicompletion_item();
  void setwidths( int[] );
  GEDCOM_object* indi() const;
  indicompletion_item* next() const;
  void setnext( indicompletion_item* );
  char* display() const;
};

////////////////////////////////////////////////////////////////////////////////

class famfindUI {
public:
  Fl_Window* findbox;
  Fl_Input* find_input;
  Fl_Output* find_id;
  Fl_Output* spouse;
  Fl_Output* spouse_id;
  Fl_Output* fam_id;
  // the searchfind_cb callback needs to be able to see the search buttons:
  Fl_Button* find_fromright;
  Fl_Button* find_fuzzy;
  Fl_Button* find_fromleft;
  Fl_Button* find_new;
  Fl_Button* find_help;
  Fl_Button* find_cancel;
  Fl_Return_Button* find_ok;
  void*    context;
  GEDCOM_object* chosenfam;
  short strategy; // a bitfield restricting how to search and what to do with results
  // bits 0-1 left/right/fuzzy - gets set in response to which search button pressed

  famfindUI();
  void setcontext( void* );
  void* getcontext() const;
  treeinstance* whichtree();
  void open( void*, short, short, short);
  void searchfind(Fl_Button*);
  // we want to list the matching person first, and the spouse secnd, so need some idea of gender found
  // but we only get one thing back in userdata in the callback. How can we pass context ?
  void setfam( famcompletion_item* );
  short getstrategy();
  void OK();
  void finish();
};

////////////////////////////////////////////////////////////////////////////////

class famcompletionsUI {
public:
  Fl_Window* completionswin;
  Fl_Select_Browser* completions;
  treeinstance* scantree;
  char* searchfor;
  int searchmethod;
  famcompletion_item* first;
  int widths[4];

  famcompletionsUI();
  void open( treeinstance*, char*, int, Fl_Callback*, /*void*,*/ short, short );
  void finish();
  famcompletion_item* chosen_fam( int );
};


////////////////////////////////////////////////////////////////////////////////

class famcompletion_item {
GEDCOM_object*    famptr;
char* sex;
famcompletion_item*  nextptr;
GEDCOM_string*    displayptr;
int width[4];

public:
  famcompletion_item( GEDCOM_object*, char* );
  ~famcompletion_item();
  void setwidths( int[] );
  GEDCOM_object* fam() const;
  famcompletion_item* next() const;
  void setnext( famcompletion_item* );
  char* display() const;
  char* gender() const;
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
  char       notes_id[MAX_notestitle];
  treeinstance  *edittree;   // which tree - we have to know so we can mark it
  // as modified when we save the changed notes back, and so we can put it in
  // the window title - the user needs to know too !
  GEDCOM_object *editobject; // which object we are editing a NOTE/TEXT on ?
  GEDCOM_object *textobject; // this is our (possibly ephemeral) actual NOTE/TEXT
  GEDCOM_tag *edittag;   // which subobject (NOTE or TEXT)
  int size;
  bool modified;

public:
  Fl_Window       *window;   // handle of the main window
  Fl_Output       *ident;    // handle of the text which says what we're editing
  Fl_Menu_Bar     *menubar;  // handle of the row of buttons (not really a menu, yet)
  Fl_Text_Editor  *scroll;   // handle of the pane which contains the editable text
  Fl_Text_Buffer  *input;    // an FLTK buffer for that sort of thing

  notesUI( treeinstance*, GEDCOM_object*, GEDCOM_tag* );
  void status(bool);
  ~notesUI();
  notesUI* getnext() const;
  void setnext( notesUI* );
  notesUI* getprevious() const;
  void setprevious( notesUI* );
  GEDCOM_object* object() const;
  GEDCOM_tag* tag() const;
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


