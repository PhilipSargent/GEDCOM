// gui.cxx

#include "fixes.h"

#include <stdlib.h>
#include <string>

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
#include <FL/fl_draw.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/fl_ask.H>

#include "menucodes.h"
#include "classes.h"
#include "gui.h"
#include "structure.h"
#include "trees.h"
#include "callback.h"
#include "display.h"
#include "objects.h"
#include "tags.h"

///////////////////////////////////////////////////////////////////////////
/////////////////     menu structures used by the GUIs      ///////////////
///////////////////////////////////////////////////////////////////////////

/**** Warning: you need to keep the array sizes declared for the members *****
***** of the menu classes (gui.h) in step with the actual menu contents  *****
***** here, 'cos I haven't found a more elegant way of doing it. We have *****
***** separate instanciations of apparently static menu structures since *****
***** FLTK doesn't automatically take a copy - and if it did, it would   *****
***** be more trouble to do the message translations.  Also we need to   *****
***** pass info on the main window instanciation back in the callbacks.  *****
***** Doing it this way also means we can tailor visibility & activation *****
***** for each menu item appropriate to the main window it is raised on. ****/

///////////////////////////////////////////////////////////////////////////////
// class barmenu: for instanciations of the main menu bar
// cast with (Fl_Menu_Item*) to use with ->menu(thing)

barmenu::barmenu(mainUI* view) {
  menu_data[0] = (Fl_Menu_Item){"File",           0, 0, 0,  64, 0, 0, 14, 0 };
  menu_data[1] = (Fl_Menu_Item){"Load",           0, 0, 0,   0, 0, 0, 14, 0 };
  menu_data[2] = (Fl_Menu_Item){"Save",           0, (Fl_Callback*)save_cb, (void*)(view),   0, 0, 0, 14, 0 };
  menu_data[3] = (Fl_Menu_Item){"Save as ...",    0, (Fl_Callback*)saveas_cb, (void*)(view),   0, 0, 0, 14, 0 };
  menu_data[4] = (Fl_Menu_Item){"Print",          0, 0, 0, 128, 0, 0, 14, 0 };
  menu_data[5] = (Fl_Menu_Item){"New View",       0, (Fl_Callback*)newview_cb, (void*)(view),   0, 0, 0, 14, 0 };
  menu_data[6] = (Fl_Menu_Item){"Discard",        0, 0, 0,   0, 0, 0, 14, 0 };
  menu_data[7] = (Fl_Menu_Item){"Statistics",     0, (Fl_Callback*)stats_cb, (void*)(view),   0, 0, 0, 14, 0 };
  menu_data[8] = (Fl_Menu_Item){"Quit",           0, (Fl_Callback*)quit_cb, 0,   0, 0, 0, 14, 0 };
  menu_data[9] = (Fl_Menu_Item){ 0 };
  menu_data[10] = (Fl_Menu_Item){"People",        0, 0, 0,  64, 0, 0, 14, 0 };
  menu_data[11] = (Fl_Menu_Item){"New ...",       0, 0, 0,   0, 0, 0, 14, 0 };
  menu_data[12] = (Fl_Menu_Item){"Find ...",      0, (Fl_Callback*)indifind_cb, (void*)(view),   0, 0, 0, 14, 0 };
  menu_data[13] = (Fl_Menu_Item){ 0 };
  menu_data[14] = (Fl_Menu_Item){"Reports",       0, 0, 0,  64, 0, 0, 14, 0 };
  menu_data[15] = (Fl_Menu_Item){"Standard",      0, 0, 0,   0, 0, 0, 14, 0 };
  menu_data[16] = (Fl_Menu_Item){"Family Groups", 0, 0, 0,   0, 0, 0, 14, 0 };
  menu_data[17] = (Fl_Menu_Item){"Web pages ...", 0, 0, 0,   0, 0, 0, 14, 0 };
  menu_data[18] = (Fl_Menu_Item){ 0 };
  menu_data[19] = (Fl_Menu_Item){"Settings",      0, 0, 0,  64, 0, 0, 14, 0 };
// this needs to go away somehow
//  menu_data[20] = (Fl_Menu_Item){"Language",      0, 0, 0,  64, 0, 0, 14, 0 };
//  menu_data[20] = (Fl_Menu_Item){ 0 };
  menu_data[20] = (Fl_Menu_Item){"Choices ...",        0, (Fl_Callback*)choices_cb, 0,   0, 0, 0, 14, 0 };
  menu_data[21] = (Fl_Menu_Item){"Save Choices",       0, 0, 0,   0, 0, 0, 14, 0 };
  menu_data[22] = (Fl_Menu_Item){ 0 };
  menu_data[23] = (Fl_Menu_Item){"Help",               0, 0, 0,  64, 0, 0, 14, 0 };
// menu captions need translating but help filenames don't
  menu_data[24] = (Fl_Menu_Item){"Introduction",       0, (Fl_Callback*)help_cb, (void*)("intro.html"),  0, 0, 0, 14, 0 };
  menu_data[25] = (Fl_Menu_Item){"Main window",        0, (Fl_Callback*)help_cb, (void*)("mainUI.html"), 0, 0, 0, 14, 0 };
  menu_data[26] = (Fl_Menu_Item){"Person dbox",        0, (Fl_Callback*)help_cb, (void*)("indiUI.html"), 0, 0, 0, 14, 0 };
  menu_data[27] = (Fl_Menu_Item){"Family events dbox", 0, (Fl_Callback*)help_cb, (void*)("famUI.html"),  0, 0, 0, 14, 0 };
  menu_data[28] = (Fl_Menu_Item){"Choices dbox",       0, (Fl_Callback*)help_cb, (void*)("prefUI.html"), 0, 0, 0, 14, 0 };
  menu_data[29] = (Fl_Menu_Item){"GEDCOM files",       0, (Fl_Callback*)help_cb, (void*)("GEDCOM.html"), 128, 0, 0, 14, 0 };
  menu_data[30] = (Fl_Menu_Item){"About",              0, (Fl_Callback*)info_cb, 0,   0, 0, 0, 14, 0 };
  menu_data[31] = (Fl_Menu_Item){ 0 };
  menu_data[32] = (Fl_Menu_Item){ 0 };
}

///////////////////////////////////////////////////////////////////////////////
// class indipopupmenu:
// for instanciations of the main window click-on-INDI popup menu
// cast with (Fl_Menu_Item*) to use with ->menu(thing)

// I think it's redundant putting (void*)(view) as the userdata, as the userdata
// gets set by the popup callback before it calls the callbacks in the menu

indipopupmenu::indipopupmenu(mainUI* view) {
  popup_data[imenubase]    = (Fl_Menu_Item){"Edit",      0, (Fl_Callback*)edit_cb,         (void*)(view), 0,0,0,14,0 };
  popup_data[imenunotes]   = (Fl_Menu_Item){"Notes",     0, (Fl_Callback*)menuopennotes_cb,(void*)(view), 0,0,0,14,0 };
  popup_data[imenuyounger] = (Fl_Menu_Item){"Younger",   0, (Fl_Callback*)menu_younger_cb, (void*)(view), 0, 0, 0, 14, 0 };
  popup_data[imenuolder]   = (Fl_Menu_Item){"Older",     0, (Fl_Callback*)menu_older_cb,   (void*)(view), 0, 0, 0, 14, 0 };
  popup_data[imenunewfam]  = (Fl_Menu_Item){"New family",0, (Fl_Callback*)famnew_cb,       (void*)(view), 0, 0, 0, 14, 0 };
  popup_data[imenustructure]=(Fl_Menu_Item){"Structure", 0, 0, 0, 192, 0, 0, 14, 0 };
  popup_data[imenuremove]  = (Fl_Menu_Item){"Delete",    0, (Fl_Callback*)deleteindi_cb,   (void*)(view), 0, 0, 0, 14, 0 };
  popup_data[imenudetach]  = (Fl_Menu_Item){"Detach",    0, 0, 0,   0, 0, 0, 14, 0 };
  popup_data[imenumove]    = (Fl_Menu_Item){"Move",      0, 0, 0,   0, 0, 0, 14, 0 };
  popup_data[imenuattach]  = (Fl_Menu_Item){"Attach",    0, 0, 0,   0, 0, 0, 14, 0 };
  popup_data[imenuendstruct] = (Fl_Menu_Item){ 0 };
  popup_data[imenuindreps] = (Fl_Menu_Item){"Reports",   0, 0, 0,  64, 0, 0, 14, 0 };
  popup_data[imenuancs]    = (Fl_Menu_Item){"Ancestors", 0, 0, 0,   0, 0, 0, 14, 0 };
  popup_data[imenuendreps] = (Fl_Menu_Item){ 0 };
  popup_data[imenuend]     = (Fl_Menu_Item){ 0 };
}

void indipopupmenu::grey( int n ) {
  popup_data[n].deactivate();
}

void indipopupmenu::black( int n ) {
  popup_data[n].activate();
}



///////////////////////////////////////////////////////////////////////////////
// class fampopupmenu:
// for instanciations of the main window click-on-FAM popup menu
// cast with (Fl_Menu_Item*) to use with ->menu(thing)

// I think it's redundant putting (void*)(view) as the userdata, as the userdata
// gets set by the popup callback before it calls the callbacks in the menu

fampopupmenu::fampopupmenu(mainUI* view) {
  popup_data[fmenubase]    = (Fl_Menu_Item){"Edit",      0, (Fl_Callback*)famed_cb, (void*)(view),         0,0,0,14,0 };
  popup_data[fmenunotes]   = (Fl_Menu_Item){"Notes",     0, (Fl_Callback*)menuopennotes_cb, (void*)(view), 0,0,0,14,0 };
  popup_data[fmenulater]   = (Fl_Menu_Item){"Later",     0, (Fl_Callback*)menu_later_cb, (void*)(view),    0,0,0,14,0 };
  popup_data[fmenuearlier] = (Fl_Menu_Item){"Earlier",   0, (Fl_Callback*)menu_earlier_cb, (void*)(view),  0,0,0,14,0 };
  popup_data[fmenuchild]   = (Fl_Menu_Item){"New child", 0, (Fl_Callback*)newchild_cb, (void*)(view), 0, 0, 0, 14, 0 };
  popup_data[fmenuunmarry] = (Fl_Menu_Item){"Unmarry",   0, (Fl_Callback*)unmarry_cb, (void*)(view), 128, 0, 0, 14, 0 };
  // unmarry is to remove the FAM object entirely. Divorce is a tab in the
  // FAM edit box, and no longer a menu item as it was in !Family
  popup_data[fmenuend]     = (Fl_Menu_Item){ 0 };
}

void fampopupmenu::grey( int n ) {
  popup_data[n].deactivate();
}

void fampopupmenu::black( int n ) {
  popup_data[n].activate();
}

///////////////////////////////////////////////////////////////////////////////
// class genpopupmenu:
// for instanciations of the main window click-on-blank popup menu
// cast with (Fl_Menu_Item*) to use with ->menu(thing)

genpopupmenu::genpopupmenu(mainUI* view) {
  popup_data[0] = (Fl_Menu_Item){"Save window as graphic",  0, (Fl_Callback*)savescr_cb, (void*)(view),   0, 0, 0, 14, 0 };
  popup_data[1] = (Fl_Menu_Item){ 0 };
}

///////////////////////////////////////////////////////////////////////////////
// class notesmenu:

notesmenu::notesmenu( notesUI* notesui ) {
  menu_data[0] = (Fl_Menu_Item){"Cancel",             0, (Fl_Callback*)cancelnotes_cb, (void*)notesui, 0, 0, 0, 14, 0 };
  menu_data[1] = (Fl_Menu_Item){"Clear",              0, (Fl_Callback*)clearnotes_cb,  (void*)notesui, 0, 0, 0, 14, 0 };
  menu_data[2] = (Fl_Menu_Item){"Restore",            0, (Fl_Callback*)restorenotes_cb, (void*)notesui, 0, 0, 0, 14, 0 };
  menu_data[3] = (Fl_Menu_Item){"Save back to GEDCOM",0, (Fl_Callback*)savenotes_cb,   (void*)notesui, 0, 0, 0, 14, 0 };
  menu_data[4] = (Fl_Menu_Item){"OK (Save and close)",0, (Fl_Callback*)oknotes_cb,     (void*)notesui, 0, 0, 0, 14, 0 };
  menu_data[5] = (Fl_Menu_Item){ 0 };
}

///////////////////////////////////////////////////////////////////////////
///////////////// GUI classes - one for each type of window ///////////////
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// constructor for mainUI : user interface, main window

mainUI::mainUI( treeinstance* which ):
  tree (which),
  next (NULL),   // creator must call setnext() if we are not last
  // set a fairly random initial canvas size - this will be reset as soon
  // as we have a tree to display and a "real" canvas size.
  maxX (600),
  maxY (400),
  person (NULL),
  topind (NULL),
  title (NULL),
  display (NULL)
  {
#ifdef debugging
  printf("initial mainUI at %ld is raised on tree %ld\n",(long)this, (long)tree);
#endif
  // open the window a bit smaller than that
  Fl_Window* w = window = new Fl_Window(600, 400);
  w->labeltype(FL_NORMAL_LABEL);
  w->color(7);
  mainmenu = new barmenu( this );
  { Fl_Menu_Bar* o = main_menu = new Fl_Menu_Bar( 0, 0, 600, 25);
    o->menu((Fl_Menu_Item*)mainmenu);
  }
  { Fl_Scroll* o = scroll = new Fl_Scroll(0,25,600,375);
    o->type(Fl_Scroll::BOTH_ALWAYS);
    // need a canvas child here. I see no reason why this shouldn't behave as a
    // Fl_Button itself, and thus get the mouse clicks and popup the menu
    { treedisplay* o = main = new treedisplay(0,25,maxX-17,maxY-42);
      o->setview( this );
      o->setscroller( scroll );
      o->box(FL_NO_BOX);
      o->labeltype(FL_NO_LABEL);
      o->callback((Fl_Callback*)popup_cb,(void*)(this));
      o->when(FL_WHEN_RELEASE);
    }
    o->end();
  }
#ifdef debugging
  printf("mainmenu %ld, treedisplay %ld\n",(long)mainmenu, (long)main);
#endif
  indimenu = new indipopupmenu( this );
  fammenu  = new  fampopupmenu( this );
  genmenu  = new  genpopupmenu( this );

  w->end();
  w->resizable(scroll);
  w->size_range(400,100,maxX,maxY);
  w->callback((Fl_Callback*)killed_cb);
  w->user_data((void*)(this));
#ifdef debugging
  printf("finished mainUI constructor\n");
#endif
}

mainUI::~mainUI() {
  delete window;
  delete display;
}

void mainUI::settitle() {
GEDCOM_string* oldtitle;
std::string newtitle("X!Family");
  newtitle+=" ";
  newtitle+=tree->getfilename();
  GEDCOM_object *thing = this->getcurrent();
  if (thing != NULL) {
    thing = thing->subobject( NAME_tag );
    if (thing != NULL) {
      newtitle+=" : ";
      newtitle+=thing->value();
    } else {
      newtitle+=" : <unnamed>";
    }
  }
  oldtitle = title;
  title = new GEDCOM_string( (char *)newtitle.c_str() );
  if (window!=NULL) window->label( title->string() );
  if (oldtitle!=NULL) delete oldtitle;
}

void mainUI::show() {
  window->show();
}

void mainUI::hide() {
  window->hide();
}

void mainUI::canvassize( int xsize, int ysize ) {
  // the minimum window width (400) should really be calculated on the width of the
  // menu bar in the current language. Don't yet know how to do that :-(
#ifdef debugging
  printf("mainUI::canvassize called with %d, %d\n", xsize, ysize);
#endif
  int nowX = window->w();
  int nowY = window->h();
  // we could dispense with knowing the size of a scrollbar if we had not chosen
  // to set the Scroll type to BOTH_ALWAYS
  maxX = xsize + 17; // canvas + scrollbar
  maxY = ysize + 42; // canvas + scrollbar + menubar
  if (maxX < 400) { // don't let code shrink window more than user can
     maxX = 400;
     xsize = maxX - 17; // canvas must fill minimum sized window
  }
  if (maxY < 100){
    maxY = 100;
    ysize = maxY - 42;
  }
  if (nowX > maxX) nowX = maxX;
  if (nowY > maxY) nowY = maxY;
#ifdef debugging
  printf("mainUI::canvassize window->size %d , %d\n", nowX, nowY);
  printf("mainUI::canvassize   main->size %d , %d\n", xsize, ysize);
  printf("mainUI::canvassize scroll->size %d , %d\n", nowX, nowY-25);
#endif
  window->size( nowX, nowY ); // must ensure we are smaller than the max we will set
  main->size( xsize, ysize );
  scroll->size( nowX, nowY-25 ); // can we force a scrollbar redraw, and is this
  // the right size. Confused ? I am ...
  // this would appear to be the correct place to scroll the window such that the canvas
  // fills the window - one would hope that fltk would do this itself (it doesn't), so
  // maybe we ought to make this a conditional compilation bit ? FIXME.
#ifdef fix0001
  // fix0001 is supposed to stop us ever getting the canvas off to one side of the window,
  // leaving grey background showing. It isn't working.
  // a) canvassize (+scrollbars and menubar) should never go lower than the minimum window size
  // b) windowsize should never go bigger than canvassize (+scrollbars and menu bar)
  // c) canvas should always be scrolled to ensure the window (-scrollbars and menu bar) is
  //    fully covered by canvas.
  // d) if at all possible, if we've just resized the window in response to a new current person
  //    we should scroll such that the current person is visible. We *don't* want to do any
  //    scrolling off our own bat in response to the *user* resizing the window, except to the
  //    minimum extent needed to ensure that we meet the criteria a-c above. In code associated
  //    with fix0003, we appear to be doing this successfully.
  int offX = scroll->xposition();
  int offY = scroll->yposition();
  if (offX + nowX > xsize + 17) offX = xsize + 17 - nowX;
  if (offX < 0) offX = 0;
  if (offY + (nowY-42) > ysize) offY = ysize + 42 - nowY;
  if (offY < 0) offY = 0;
#ifdef fltk13
  scroll->scroll_to(offX,offY);
#else
  scroll->position(offX,offY);
# endif
#ifdef debugging
  printf("mainUI::canvassize scrolling to %d, %d\n", offX,offY);
  printf("mainUI::canvassize window->size_range %d , %d\n", maxX, maxY);
#endif
#endif
  window->size_range(400,100,maxX,maxY);
}

int mainUI::canvas_w() { return this->main->w(); }
int mainUI::canvas_h() { return this->main->h(); }

mainUI*        mainUI::getnext() const { return next; }
void           mainUI::setnext( mainUI* chained ) { next = chained; }
treeinstance*  mainUI::whichtree() const { return tree; }
displaytree*   mainUI::whattodraw() const { return display; }
GEDCOM_object* mainUI::getcurrent() const { return person; }

void mainUI::setcurrent( GEDCOM_object* newperson ) {
bool redraw;
bool scroll;
  scroll = true;
  // we may want to set the current person unchanged if we have deleted the top
  // person in the tree and want to force mainUI to choose a new treeroot
  // or have added a parent, and also want a new treeroot, so don't reject that call
  // if (person == newperson) return;
  if (newperson == NULL ) {
    // we should never let that happen for a real tree, but when we start with an
    // empty tree, or load a GEDCOM with no INDI objects, it IS going to happen.
    // also, if we delete the current person, we either need a methond of choosing
    // a new current person to maintain the display, or (like !Family) we have no current person
    fl_alert("mainUI::setcurrent called with null person, we are going to barf\n");
    // in the event that we did have a current person, this is our only survival
    // route. If there really are no INDIs, we will need some more code ...
    return;
  }
// industrial strength version would check that the object is indeed an
// INDI object.
#ifdef debugging
  printf("attempting to set current to this object %ld:\n",(long)newperson);
  newperson->outline( stdout, 0 );
//  newperson->subobject( NAME_tag )->print( 1 ); // diagnose
  if ((newperson->subobject( NAME_tag ))!=NULL) newperson->subobject( NAME_tag )->print( 1 );
#endif
  if (person==newperson) scroll=false;
  person = newperson;
// I'm sure this is bad news. "hide"ing a window seems to allow the
// window manager free rein to move it about when we show it again,
// which it ***** well shouldn't.
//  if (redraw=(window->shown())) this->hide();
  redraw=(window->shown()); // so don't hide, even if shown
  this->settitle();
#ifdef debugging
  printf("mainUI::setcurrent set new window title OK\n");
#endif
  this->settop();
#ifdef debugging
  printf("mainUI::setcurrent set new current person OK\n");
#endif
  this->newdisplay();
  if (scroll) this->centre_current();
#ifdef debugging
  printf("mainUI::setcurrent created new display structure OK\n");
#endif
  // FIXME we ought to centre the new person on the display, and adjust the canvas size
  // so that you don't get a blank grey window with all the data scrolled off somewhere
  // newdisplay is supposed to be doing that
  if (redraw) this->show();
}

void mainUI::newdisplay() {
  if (display != NULL)
  {
#ifdef debugging
    printf("mainUI::newdisplay - destroying old at %ld\n", (long)display);
#endif
    delete display;
  }
#ifdef debugging
  printf("mainUI::newdisplay Now start new tree from %ld with current %ld\n", (long)topind, (long)person);
#endif
  display = new displaytree( topind, person );
#ifdef debugging
  printf("mainUI::newdisplay created new displaytree at %ld\n", (long)display);
#endif
  display->buildtree();
#ifdef debugging
  printf("mainUI::newdisplay built the tree structure\n");
#endif
  display->calctree();
#ifdef debugging
  printf("mainUI::newdisplay calculated where to put everything\n");
#endif
  this->canvassize(display->xsize(),display->ysize());
#ifdef debugging
  printf("mainUI::newdisplay finally set the canvassize %d x %d\n",display->xsize(),display->ysize());
#endif
  // OK, canvassize will scroll to ensure that the canvas fills the window
  // is this true ? isn't that one of the bugs ?
  // so we don't want to scroll it afterwards FIXME I'm not (yet) convinced this
  // is right - but we'll accumulate some experience with fix0003 before doing anything
}

void mainUI::centre_current() {
  indidisplay *newcurrent;
  newcurrent = display->getcurrent();
 #ifdef fltk13
   scroll->scroll_to( newcurrent->x() - (this->window->w()/2), 0 );
 #else
   scroll->position( newcurrent->x() - (this->window->w()/2), 0 );
 #endif
}

void mainUI::settop() {
  GEDCOM_object *fam;
  printf("settop person %ld or parent\n",(long)person);
  if (person==NULL) {
    printf("mainUI::settop() called with NULL person, giving up....\n");
    return;
  }
  fam = person->parental_family();
  printf("Found fam %ld\n",(long)fam);
  if (fam == NULL) { topind = person; return; }

  topind = fam->thehusband();
  if (topind == NULL) {
    topind = fam->thewife();
    if (topind == NULL) { topind = person; return; }
  }
  return;
}

GEDCOM_object* mainUI::gettop() const {
  return topind;
}

/*
void mainUI::cleardisplay() {

// clear the display structure prior to recalculating
// we just bin the display structures pointed to by the tree

  indidisplay *indi = topdisplay;

  while (indi != NULL) {
    indi->clearall();
    indi = indi->next_object();
  }

  topdisplay = NULL; // mark display as cleared
}
*/

///////////////////////////////////////////////////////////////////////////////
// class infoUI constructor: build About box - only one instanciation

infoUI::infoUI() {
  Fl_Window* w = info_panel = new Fl_Window( 520, 210, "About this program" );
  w->labeltype(FL_NORMAL_LABEL);
  w->user_data((void*)(this));
  // all the members of this class are static, because there is only ever one window
  // and it always has the same stuff in it. But every use of a class member below
  // (info_group = etc.) generates an undefined reference at link time. Why ?
  { Fl_Group* o = info_group = new Fl_Group( 0, 0, 520, 210);
    o->labeltype(FL_NO_LABEL);
    { Fl_Box* o = info_name = new Fl_Box( 5, 10, 100, 40, "Name" );
      o->labelsize(18);
      o->labelfont(1);
      o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    }
    { Fl_Box* o = author = new Fl_Box(5, 60, 100, 40, "Author");
      o->labelsize(18);
      o->labelfont(1);
      o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    }
    { Fl_Box* o = version = new Fl_Box(5, 110, 100, 40, "Version");
      o->labelsize(18);
      o->labelfont(1);
      o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    }
    { Fl_Box* o = purpose = new Fl_Box(5, 160, 100, 40, "Purpose");
      o->labelsize(18);
      o->labelfont(1);
      o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    }
    { Fl_Box* o = name_l = new Fl_Box(110, 10, 330, 40, "X!Family");
      o->labelsize(18);
      o->labelfont(1);
      o->box(FL_DOWN_BOX);
      o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    }
    { Fl_Box* o = author_l = new Fl_Box(110, 60, 405, 40, "© A.E.R.Waddington, 2023");
      o->labelsize(18);
      o->labelfont(1);
      o->box(FL_DOWN_BOX);
      o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    }
    { Fl_Box* o = version_l = new Fl_Box(110, 110, 405, 40, "v 0.22 alpha 2020-03-26 en-gb");
      o->labelsize(18);
      o->labelfont(1);
      o->box(FL_DOWN_BOX);
    }
    { Fl_Box* o = purpose_l = new Fl_Box(110, 160, 330, 40, "Family Tree Editor");
      o->labelsize(18);
      o->labelfont(1);
      o->box(FL_DOWN_BOX);
      o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    }
    // almost the only label which doesn't need internationalising:
    { Fl_Button* o = gpl_button = new Fl_Button(445, 10, 70, 40, "\xA9");
      o->labelsize(18);
      o->labelfont(1);
      o->down_box(FL_DOWN_BOX);
      o->callback((Fl_Callback*)gplinfo_cb);
    }
    o->end();
  }
  { Fl_Box* o = gpl_box = new Fl_Box(0, 0, 520, 210,
    "The X!Family Genealogical Package version 0.22 Copyright © 2023 by Andy Waddington.\nBased loosely on !Family for RISC OS originally developed by Denis Howe.\n\nThis package is free software: you can redistribute it and/or modify it under the terms of\nthe GNU General Public License as published by the Free Software Foundation; either\nversion 3 of the license, or (at your option) any later version.\n\nThis software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;\nwithout even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR\nPURPOSE. See the GNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this software: if not, write to the Free Software Foundation, Inc.,\n59 Temple Place, Suite 330, Boston. MA 02111-1307 USA\nPlease report faults to family-maint@pennine.ddns.me.uk");
    o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    o->labelsize(12);
    o->labelcolor(92);
    o->box(FL_FLAT_BOX);
    o->color(0);
    o->hide();
  }
  { Fl_Return_Button* o = info_ok = new Fl_Return_Button(445, 160, 70, 40, "OK");
      o->labelsize(18);
      o->labelfont(1);
    o->down_box(FL_DOWN_BOX);
    o->callback((Fl_Callback*)okinfo_cb);
  }
// doesn't seem to do anything -removed w->set_non_modal();
// seems to be a bad idea when added - still have a minimise icon, and using it
// leaves every window unresponsive with no way to get infoUI back  w->set_modal();
  w->end();
}

///////////////////////////////////////////////////////////////////////////////
// class indiUI the "edit individual" dbox. We will find a pointer to a
// GEDCOM object is all we need, to know which individual (and thus which
// treeinstance) this relates to. But if we are creating a new person, we
// don't make a GEDCOM_object until user clicks OK, so we also need a tree
// instance (not a mainUI) pointer in the indiUI class. You would also need
// that for a name-completion-request event on a field (indi_name, indi_ma,
// indi_pa) in the edit box - it is no use setting someone's parents to
// people in two different trees !! [Well, actually, it could be. That
// would be one way of connecting to trees into one, but would need a bit
// more thought to make it work without ID conflicts...]

indiUI::indiUI( treeinstance* thistree, GEDCOM_object* whofor ):
  // fix0004 was redundant and removed, but we think we should retain these pointers
  mum (NULL),
  dad (NULL),
  fam (NULL),
  // end of new pointers
  who (whofor),       // NULL for a new person not yet in the tree
  which (thistree)
{
printf("new indiUI on treeinstance %ld\n",(long)which);
// make editing layout easier (maybe?): should this be an include file?
#define col0 0
#define col1 75
#define normh 25
#define doubleh 55
#define row 30
#define bigh 30
#define bigrow 35
#define smallh 24
#define fullw 390
#define dboxwidth col1+fullw+15
#define smallw 200
#define redw 280
#define datew 130
#define timew 90
#define agew 50
#define deathgap 60
#define buttonw 110
#define fbw 25
#define idw 105
#define addcol col1+redw+5
#define addw  50
#define findcol col1+redw+5+addw+5
#define findw 50
#define biggap 25
#define smallgap 5
#define fb3l col1+redw+5
#define fb3m fb3l+fbw
#define fb3r fb3m+fbw
#define b3l col1+5
#define b3m b3l+buttonw+biggap
#define b3r b3m+buttonw+biggap
#define b41 10
#define b42 b41+buttonw+smallgap
#define b43 b42+buttonw+smallgap
#define b44 b43+buttonw+smallgap
#define namerow 5
#define titlerow namerow+row
#define genderrow titlerow+row
#define gendertop genderrow+3
#define parow genderrow+bigrow
#define marow parow+row
#define notesrow marow+row
#define notestop notesrow+3
#define tabsrow notesrow+bigrow
#define tilerow tabsrow+normh
#define daterow tilerow+10
#define calrow daterow+row
#define caltop calrow+3
#define placerow calrow+bigrow
#define siterow placerow+row
#define causrow siterow+row
#define sourcerow causrow+row
#define tabheight row+row+bigrow+row+row+row+bigrow+5
#define tileheight row+bigrow+row+row+row+bigrow+5
#define helprow sourcerow+row+15
#define okrow helprow+row+15
#define canrow helprow+row+15
#define approw helprow+row+15
#define cancol 45
#define okcol 335
#define helpcol 190
#define appcol 190
#define dboxheight okrow+row+5
  { Fl_Window* o = indi_dbox = new Fl_Window( dboxwidth, dboxheight, buf_indi_title ); // *not* msg_indi_title :-)
    o->callback((Fl_Callback*)canceledit_cb);
    o->user_data((void*)(this));
    indi_name = new Fl_Input(col1, namerow, fullw, normh, "Name");
    indi_id = new Fl_Output(fb3l, titlerow, idw, normh, "ID");
    indi_title = new Fl_Input(col1, titlerow, smallw, normh, "Title");
    { Fl_Box* o = new Fl_Box(col0+5, gendertop, col0-5, smallh, "Gender");
      o->align(FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
    }
    { Fl_Group* o = new Fl_Group(col1, genderrow, fullw, bigh);
      o->box(FL_EMBOSSED_BOX);
      { Fl_Light_Button* o = indi_male = new Fl_Light_Button(b3l, gendertop, buttonw, smallh, "Male");
        o->type(102); /* radio button */
        o->selection_color(FL_RED);
      }
      { Fl_Light_Button* o = indi_female = new Fl_Light_Button(b3m, gendertop, buttonw, smallh, "Female");
        o->type(102); /* radio button */
        o->selection_color(FL_RED);
      }
      { Fl_Light_Button* o = indi_unknown = new Fl_Light_Button(b3r, gendertop, buttonw, smallh, "Unknown");
        o->type(102); /* radio button */
        o->selection_color(FL_RED);
      }
      o->end();
    }
    indi_ma = new Fl_Output(col1, marow, redw, normh, "Mother");
    indi_ma_id = new Fl_Output(fb3l, marow, idw, normh, "" );
    indi_pa = new Fl_Output(col1, parow, redw, normh, "Father");
    indi_pa_id = new Fl_Output(fb3l, parow, idw, normh, "" );
// add buttons (in same place as the ID fields) to Add/Find a parent
    indi_pa_add = new Fl_Button(addcol, parow, addw, normh, "Add");
        indi_pa_add->down_box(FL_DOWN_BOX);
        indi_pa_add->type(0);
        indi_pa_add->selection_color(FL_BLACK);
        indi_pa_add->labelfont(1);
        indi_pa_add->callback((Fl_Callback*)buttonaddindi_pa_cb);
        indi_pa_add->user_data((void*)(this));
        indi_pa_add->hide();
    indi_ma_add = new Fl_Button(addcol, marow, addw, normh, "Add");
        indi_ma_add->down_box(FL_DOWN_BOX);
        indi_ma_add->type(0);
        indi_ma_add->selection_color(FL_BLACK);
        indi_ma_add->labelfont(1);
        indi_ma_add->callback((Fl_Callback*)buttonaddindi_ma_cb);
        indi_ma_add->user_data((void*)(this));
        indi_ma_add->hide();
    indi_fam_find = new Fl_Button(findcol, parow, findw, doubleh, "Find");
        indi_fam_find->down_box(FL_DOWN_BOX);
        indi_fam_find->type(0);
        indi_fam_find->selection_color(FL_BLACK);
        indi_fam_find->labelfont(1);
        indi_fam_find->callback((Fl_Callback*)buttonfindindi_fam_cb);
        indi_fam_find->user_data((void*)(this));
        indi_fam_find->hide();
    { Fl_Group* o = new Fl_Group(col0+5, notesrow, dboxwidth-15, bigh);
      o->box(FL_EMBOSSED_BOX);
      { Fl_Button* o = indi_notes = new Fl_Light_Button(b41, notestop, buttonw, smallh, "Notes");
        o->type(0); // this stops FLTK from toggling the light on the button itself
        o->selection_color(FL_BLACK);
        o->down_box(FL_DOWN_BOX);
        o->labelfont(1);
        o->callback((Fl_Callback*)buttonopennotes_cb);
        o->user_data((void*)(who));
      }
      { Fl_Button* o = indi_sources = new Fl_Button(b42, notestop, buttonw, smallh, "Sources");
        o->down_box(FL_DOWN_BOX);
        o->labelfont(1);
        o->callback((Fl_Callback*)buttonopensourceui_cb);
        o->user_data((void*)(who));
      }
      { Fl_Button* o = indi_will = new Fl_Button(b43, notestop, buttonw, smallh, "Will");
        o->down_box(FL_DOWN_BOX);
        o->labelfont(1);
        o->callback((Fl_Callback*)deadbutton_cb);
        o->user_data((void*)(who));
      }
      { Fl_Button* o = indi_probate = new Fl_Button(b44, notestop, buttonw, smallh, "Probate");
        o->down_box(FL_DOWN_BOX);
        o->labelfont(1);
        o->callback((Fl_Callback*)deadbutton_cb);
        o->user_data((void*)(who));
      }
      o->end();
    }
    { Fl_Tabs* o = indi_events = new Fl_Tabs(col0, tabsrow, dboxwidth, tabheight);
      o->labeltype(FL_NO_LABEL);
      { Fl_Tile* o = indi_birth = new Fl_Tile(col0, tilerow, dboxwidth, tileheight, "Birth");
        o->color(214);
        o->selection_color(214);
        indi_birthdate = new Fl_Input(col1, daterow, datew, normh, "Date");
        indi_birthtime = new Fl_Input(col1+fullw-timew, daterow, timew, normh, "Time");
        { Fl_Box* o = new Fl_Box(col0+5, caltop, col1-5, normh, "Calendar");
          o->align(FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
        }
        { Fl_Group* o = new Fl_Group(col1, calrow, fullw, bigh);
          o->box(FL_EMBOSSED_BOX);
          o->color(214);
          { Fl_Light_Button* o = indi_birthgreg = new Fl_Light_Button(b3l, caltop, buttonw, smallh, "Gregorian");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          { Fl_Light_Button* o = indi_birthjul = new Fl_Light_Button(b3m, caltop, buttonw, smallh, "Julian");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          { Fl_Light_Button* o = indi_birthdef = new Fl_Light_Button(b3r, caltop, buttonw, smallh, "default");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          o->end();
        }
        indi_birthplace = new Fl_Input(col1, placerow, fullw, normh, "Place");
        indi_birthsite = new Fl_Input(col1, siterow, fullw, normh, "Site");
	indi_birthnotes = new Fl_Light_Button(b42, sourcerow, buttonw, normh, "Notes");
	indi_birthnotes->selection_color(FL_BLACK);
        indi_birthnotes->type(0);
	indi_birthnotes->color(214);
        indi_birthnotes->down_box(FL_DOWN_BOX);
        indi_birthnotes->callback((Fl_Callback*)buttonopennotes_cb);
	indi_birthsources = new Fl_Button(b43, sourcerow, buttonw, normh, "Sources");
	indi_birthsources->color(214);
	indi_birthsources->callback((Fl_Callback*)buttonopensourceui_cb);
        o->end();
      }
      { Fl_Tile* o = indi_chr = new Fl_Tile(col0, tilerow, dboxwidth, tileheight, "Christening");
        o->color(246);
        o->selection_color(246);
        indi_chrdate = new Fl_Input(col1, daterow, datew, normh, "Date");
        indi_chrtime = new Fl_Input(col1+fullw-timew, daterow, timew, normh, "Time");
        { Fl_Box* o = new Fl_Box(col0+5, caltop, col1-5, smallh, "Calendar");
          o->align(FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
        }
        { Fl_Group* o = new Fl_Group(col1, calrow, fullw, bigh);
          o->box(FL_EMBOSSED_BOX);
          o->color(246);
          { Fl_Light_Button* o = indi_chrgreg = new Fl_Light_Button(b3l, caltop, buttonw, smallh, "Gregorian");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          { Fl_Light_Button* o = indi_chrjul = new Fl_Light_Button(b3m, caltop, buttonw, smallh, "Julian");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          { Fl_Light_Button* o = indi_chrdef = new Fl_Light_Button(b3r, caltop, buttonw, smallh, "default");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          o->end();
        }
        indi_chrplace = new Fl_Input(col1, placerow, fullw, normh, "Place");
        indi_chrsite = new Fl_Input(col1, siterow, fullw, normh, "Site");
        indi_chrnotes = new Fl_Light_Button(b42, sourcerow, buttonw, normh, "Notes");
        indi_chrnotes->selection_color(FL_BLACK);
        indi_chrnotes->color(246);
        indi_chrnotes->type(0);
        indi_chrnotes->down_box(FL_DOWN_BOX);
        indi_chrnotes->callback((Fl_Callback*)buttonopennotes_cb);
        indi_chrsources = new Fl_Button(b43, sourcerow, buttonw, normh, "Sources");
	indi_chrsources->color(246);
	indi_chrsources->callback((Fl_Callback*)buttonopensourceui_cb);
        //o->hide();
        o->end();
      }
      { Fl_Tile* o = indi_bapm = new Fl_Tile(col0, tilerow, dboxwidth, tileheight, "Baptism");
        o->color(207);
        o->selection_color(207);
        indi_bapmdate = new Fl_Input(col1, daterow, datew, normh, "Date");
        indi_bapmtime = new Fl_Input(col1+fullw-timew, daterow, timew, normh, "Time");
        { Fl_Box* o = new Fl_Box(col0+5, caltop, col1-5, smallh, "Calendar");
          o->align(FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
        }
        { Fl_Group* o = new Fl_Group(col1, calrow, fullw, bigh);
          o->box(FL_EMBOSSED_BOX);
          o->color(207);
          { Fl_Light_Button* o = indi_bapmgreg = new Fl_Light_Button(b3l, caltop, buttonw, smallh, "Gregorian");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          { Fl_Light_Button* o = indi_bapmjul = new Fl_Light_Button(b3m, caltop, buttonw, smallh, "Julian");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          { Fl_Light_Button* o = indi_bapmdef = new Fl_Light_Button(b3r, caltop, buttonw, smallh, "default");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          o->end();
        }
        indi_bapmplace = new Fl_Input(col1, placerow, fullw, normh, "Place");
        indi_bapmsite = new Fl_Input(col1, siterow, fullw, normh, "Site");
        indi_bapmnotes = new Fl_Light_Button(b42, sourcerow, buttonw, normh, "Notes");
	indi_bapmnotes->selection_color(FL_BLACK);
	indi_bapmnotes->color(207);
        indi_bapmnotes->type(0);
        indi_bapmnotes->down_box(FL_DOWN_BOX);
	indi_bapmnotes->callback((Fl_Callback*)buttonopennotes_cb);
	indi_bapmsources = new Fl_Button(b43, sourcerow, buttonw, normh, "Sources");
	indi_bapmsources->color(207);
	indi_bapmsources->callback((Fl_Callback*)buttonopensourceui_cb);
        o->end();
      }
      { Fl_Tile* o = indi_death = new Fl_Tile(col0, tilerow, dboxwidth, tileheight, "Death");
        o->color(175);
        o->selection_color(175);
        indi_deathdate = new Fl_Input(col1, daterow, datew, normh, "Date");
        indi_deathtime = new Fl_Input(col1+datew+deathgap, daterow, timew, normh, "Time");
        indi_deathage = new Fl_Input(col1+fullw-agew, daterow, agew, normh, "Age");
        { Fl_Box* o = new Fl_Box(col0+5, caltop, col1-1, smallh, "Calendar");
          o->align(FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
        }
        { Fl_Group* o = new Fl_Group(col1, calrow, fullw, bigh);
          o->box(FL_EMBOSSED_BOX);
          o->color(175);
          { Fl_Light_Button* o = indi_deathgreg = new Fl_Light_Button(b3l, caltop, buttonw, smallh, "Gregorian");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          { Fl_Light_Button* o = indi_deathjul = new Fl_Light_Button(b3m, caltop, buttonw, smallh, "Julian");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          { Fl_Light_Button* o = indi_deathdef = new Fl_Light_Button(b3r, caltop, buttonw, smallh, "default");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          o->end();
        }
        indi_deathplace = new Fl_Input(col1, placerow, fullw, normh, "Place");
        indi_deathsite = new Fl_Input(col1, siterow, fullw, normh, "Site");
        indi_deathcause = new Fl_Input(col1, causrow, fullw, normh, "Cause");
        indi_deathnotes = new Fl_Light_Button(b42, sourcerow, buttonw, normh, "Notes");
	indi_deathnotes->selection_color(FL_BLACK);
	indi_deathnotes->color(175);
        indi_deathnotes->type(0);
        indi_deathnotes->down_box(FL_DOWN_BOX);
	indi_deathnotes->callback((Fl_Callback*)buttonopennotes_cb);
	indi_deathsources = new Fl_Button(b43, sourcerow, buttonw, normh, "Sources");
	indi_deathsources->color(175);
	indi_deathsources->callback((Fl_Callback*)buttonopensourceui_cb);
        o->end();
      }
      { Fl_Tile* o = indi_crem = new Fl_Tile(col0, tilerow, dboxwidth, tileheight, "Cremation");
        o->color(174);
        o->selection_color(174);
        indi_cremdate = new Fl_Input(col1, daterow, datew, normh, "Date");
        indi_cremtime = new Fl_Input(col1+fullw-timew, daterow, timew, normh, "Time");
        { Fl_Box* o = new Fl_Box(col0+5, caltop, col1-5, smallh, "Calendar");
          o->align(FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
        }
        { Fl_Group* o = new Fl_Group(col1, calrow, fullw, bigh);
          o->box(FL_EMBOSSED_BOX);
          o->color(174);
          { Fl_Light_Button* o = indi_cremgreg = new Fl_Light_Button(b3l, caltop, buttonw, smallh, "Gregorian");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          { Fl_Light_Button* o = indi_cremjul = new Fl_Light_Button(b3m, caltop, buttonw, smallh, "Julian");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          { Fl_Light_Button* o = indi_cremdef = new Fl_Light_Button(b3r, caltop, buttonw, smallh, "default");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          o->end();
        }
        indi_cremplace = new Fl_Input(col1, placerow, fullw, normh, "Place");
        indi_cremsite = new Fl_Input(col1, siterow, fullw, normh, "Site");
        indi_cremnotes = new Fl_Light_Button(b42, sourcerow, buttonw, normh, "Notes");
	indi_cremnotes->selection_color(FL_BLACK);
	indi_cremnotes->color(174);
        indi_cremnotes->type(0);
        indi_cremnotes->down_box(FL_DOWN_BOX);
	indi_cremnotes->callback((Fl_Callback*)buttonopennotes_cb);
	indi_cremsources = new Fl_Button(b43, sourcerow, buttonw, normh, "Sources");
	indi_cremsources->color(174);
	indi_cremsources->callback((Fl_Callback*)buttonopensourceui_cb);
        o->end();
      }
      { Fl_Tile* o = indi_buri = new Fl_Tile(col0, tilerow, dboxwidth, tileheight, "Burial");
        o->color(254);
        o->selection_color(254);
        indi_buridate = new Fl_Input(col1, daterow, datew, normh, "Date");
        indi_buritime = new Fl_Input(col1+fullw-timew, daterow, timew, normh, "Time");
        { Fl_Box* o = new Fl_Box(col0+5, caltop, col1-5, smallh, "Calendar");
          o->align(FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
        }
        { Fl_Group* o = new Fl_Group(col1, calrow, fullw, bigh);
          o->box(FL_EMBOSSED_BOX);
          o->color(254);
          { Fl_Light_Button* o = indi_burigreg = new Fl_Light_Button(b3l, caltop, buttonw, smallh, "Gregorian");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          { Fl_Light_Button* o = indi_burijul = new Fl_Light_Button(b3m, caltop, buttonw, smallh, "Julian");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          { Fl_Light_Button* o = indi_buridef = new Fl_Light_Button(b3r, caltop, buttonw, smallh, "default");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          o->end();
        }
        indi_buriplace = new Fl_Input(col1, placerow, fullw, normh, "Place");
        indi_burisite = new Fl_Input(col1, siterow, fullw, normh, "Site");
        indi_buriplot = new Fl_Input(col1, causrow, fullw, normh, "Plot");
        indi_burinotes = new Fl_Light_Button(b42, sourcerow, buttonw, normh, "Notes");
	indi_burinotes->selection_color(FL_BLACK);
	indi_burinotes->color(254);
        indi_burinotes->type(0);
        indi_burinotes->down_box(FL_DOWN_BOX);
	indi_burinotes->callback((Fl_Callback*)buttonopennotes_cb);
	indi_burisources = new Fl_Button(b43, sourcerow, buttonw, normh, "Sources");
	indi_burisources->color(254);
	indi_burisources->callback((Fl_Callback*)buttonopensourceui_cb);
        o->end();
      }
      o->end();
    }
    { Fl_Button* o = indi_cancel = new Fl_Button(cancol, canrow, buttonw, normh, "Cancel");
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)canceledit_cb);
      o->user_data((void*)(this));
    }
    { Fl_Return_Button* o = indi_ok = new Fl_Return_Button(okcol, okrow, buttonw, normh, "OK");
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)okedit_cb);
      o->user_data((void*)(this));
    }
    { Fl_Button* o = indi_help = new Fl_Button(helpcol, helprow, buttonw, normh, "Help");
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)help_cb);
      o->user_data((void*)("indiUI.html"));
    }
    { Fl_Button* o = indi_commit = new Fl_Button(appcol, approw, buttonw, normh, "Apply");
    // may want a separate msg_commit - but what's the French ?
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)commitedit_cb);
      o->user_data((void*)(this));
    }
    o->end();
  }
#undef col0
#undef col1
#undef normh
#undef row
#undef bigh
#undef bigrow
#undef smallh
#undef fullw
#undef dboxwidth
#undef smallw
#undef redw
#undef datew
#undef timew
#undef buttonw
#undef idw
#undef fbw
#undef biggap
#undef smallgap
#undef fb3l
#undef fb3m
#undef fb3r
#undef b3l
#undef b3m
#undef b3r
#undef b41
#undef b42
#undef b43
#undef b44
#undef namerow
#undef titlerow
#undef genderrow
#undef gendertop
#undef parow
#undef marow
#undef notesrow
#undef notestop
#undef tabsrow
#undef tilerow
#undef daterow
#undef datew
#undef timew
#undef calrow
#undef caltop
#undef placerow
#undef siterow
#undef causrow
#undef sourcerow
#undef tabheight
#undef tileheight
#undef revrow
#undef revcol
#undef helprow
#undef helpcol
#undef okrow
#undef okcol
#undef canrow
#undef cancol
#undef approw
#undef appcol
#undef dboxheight
}

indiUI::~indiUI() {
  // by the time we are here, all ephemeral GEDCOM structures should already be freed
#ifdef destructorlogs
  printf("~indiUI %ld, dbox %ld\n",(long)this,(long)indi_dbox);
#endif
  delete indi_dbox;
  //printf("Survived deletion of indi_dbox\n");
}

GEDCOM_object* indiUI::whois() const {
  return who;
}

treeinstance* indiUI::whichis() const {
  return which;
}

indiUI* indiUI::getnext() const {
  return next;
}

void indiUI::setnext( indiUI* nextui ) {
  next = nextui;
}

indiUI* indiUI::getprevious() const {
  return previous;
}

void indiUI::setprevious( indiUI* prevui ) {
  previous = prevui;
}

void indiUI::clear_details() {
  // this is clearing out the indibox, but NOT removing any ephemeral GEDCOM structures
  // it is used for initialisation, not tidy up !
  printf("indiUI::clear_details started\n");
  indi_name->value("");
  indi_id->value("");
  indi_title->value("");
  indi_ma->value("");
  indi_ma_id->value("");
  indi_pa->value("");
  indi_pa_id->value("");
  indi_notes->value(0);
  indi_notes->user_data((void*)NULL);
  indi_birthdate->value("");
  indi_birthtime->value("");
  indi_birthplace->value("");
  indi_birthsite->value("");
  indi_birthdef->setonly();
  indi_birthnotes->user_data((void*)NULL);
  indi_birthnotes->value(0);
  indi_birthsources->user_data((void*)NULL);
  indi_chrdate->value("");
  indi_chrtime->value("");
  indi_chrplace->value("");
  indi_chrsite->value("");
  indi_chrdef->setonly();
  indi_chrnotes->user_data((void*)NULL);
  indi_chrnotes->value(0);
  indi_chrsources->user_data((void*)NULL);
  indi_bapmdate->value("");
  indi_bapmtime->value("");
  indi_bapmplace->value("");
  indi_bapmsite->value("");
  indi_bapmdef->setonly();
  indi_bapmnotes->user_data((void*)NULL);
  indi_bapmnotes->value(0);
  indi_bapmsources->user_data((void*)NULL);
  indi_deathdate->value("");
  indi_deathtime->value("");
  indi_deathage->value("");
  indi_deathplace->value("");
  indi_deathsite->value("");
  indi_deathcause->value("");
  indi_deathdef->setonly();
  indi_deathnotes->user_data((void*)NULL);
  indi_deathnotes->value(0);
  indi_deathsources->user_data((void*)NULL);
  indi_cremdate->value("");
  indi_cremtime->value("");
  indi_cremplace->value("");
  indi_cremsite->value("");
  indi_cremdef->setonly();
  indi_cremnotes->user_data((void*)NULL);
  indi_cremnotes->value(0);
  indi_cremsources->user_data((void*)NULL);
  indi_buridate->value("");
  indi_buritime->value("");
  indi_buriplace->value("");
  indi_burisite->value("");
  indi_buriplot->value("");
  indi_buridef->setonly();
  indi_burinotes->user_data((void*)NULL);
  indi_burinotes->value(0);
  indi_burisources->user_data((void*)NULL);
  printf("indiUI::clear_details finished\n");
}

void indiUI::insert_details( GEDCOM_object* thisindi ) {
GEDCOM_object *event;
GEDCOM_object *thing; // deeper subobjects - this can get set as a side-effect of the fillin( ... ) macro which is non-obvious
GEDCOM_object *parent;
GEDCOM_object *fam=NULL;
char *val;

// As of v 0.20 we should never be called for a NULL indi.

  if (thisindi==NULL) {
    fl_alert("indiUI::insert_details() called on NULL indi\n");
    return;
  }
  this->clear_details();
  who = thisindi;
  printf("indiUI::insert_details started\n");

#define fillin( field, object, tag ) if ((thing=(object->subobject(tag)))!=NULL) field->value( thing->value() )

  indi_id->value( who->getidname() );

  fillin( indi_name, thisindi, NAME_tag );
  fillin( indi_title, thisindi, TITL_tag );

  if ((thing = thisindi->subobject( SEX_tag )) == NULL)
    indi_unknown->setonly();
  else {
    val = thing->value();
    switch (*val) {
      case 'F': indi_female->setonly(); break;
      case 'M': indi_male->setonly();break;
      default : indi_unknown->setonly();
    }
  }

  // need convenience functions for some things like mother: to find
  // mother we need to look for a FAMC, follow it to a FAM, and look for
  // a WIFE, which points to an INDI, for which we can look up the NAME !
  // Be aware that there is a risk of problems here for same sex marriages

  fam = thisindi->parental_family();
  printf("insert_details found fam %ld/mum/dad:\n",(long)fam);
  if (fam!=NULL) {
    fam->print( 0 );

    parent =  fam->thewife();
    mum = parent;
    if (mum!=NULL) mum->print( 0 );
    printf("insert_details found fam/mum %ld/dad:\n",(long)mum);

    if (parent != NULL ) {
      fillin( indi_ma, parent, NAME_tag );
      indi_ma_id->value(parent->getidname());
    }
    else {
      printf("insert_details finds the mother is NULL\n");
      // 2022-12-14 added these two lines to fix bug found by code inspection
      indi_ma_id->hide();
      indi_ma_add->show();
    }
    parent = fam->thehusband();
    dad = parent;

    printf("insert_details found fam/mum/dad %ld:\n",(long)dad);
    if (dad!=NULL) dad->print( 0 );

    if (parent != NULL ) {
      fillin( indi_pa, parent, NAME_tag );
      indi_pa_id->value(parent->getidname());
    } else {
      printf("insert_details finds the father is NULL\n");
      // 2022-12-14 added these two lines to fix bug found by code inspection
      indi_pa_id->hide();
      indi_pa_add->show();
    }
    // note that it may not be an error for both parents to be null when a FAM exists, since the
    // FAM object may exist to group known siblings, even if both parents are unknown
  } else {
    indi_ma_id->hide();
    indi_pa_id->hide();
    indi_pa_add->show();
    indi_ma_add->show();
    indi_fam_find->show();
  }
  thing = thisindi->subobject( NOTE_tag );
  if (thing!=NULL) {
    // we test both subobject and value because a NOTE object may contain a single line of
    // text, or no text, but have CONT/CONC subobjects.
    if ((thing->subobject()!=NULL)||(thing->value()!=NULL)) indi_notes->value(1);
  }
  indi_notes->user_data((void*)thisindi);
  indi_sources->user_data((void*)thisindi);
  // Anything we can raise a NOTE object on, needs to exist. So if any event below is NULL,
  // we should create a dummy one, which will be destroyed if found empty when we "save"/"OK"
  // but watch out ! this means we need to delete them on "Cancel" too - so we need to do the
  // "delete empty tags" code for every exit route. Even this is somewhat iffy - if you save the
  // GEDCOM file when you have indiUI dbox(es) open, you will save with empty events.
  // And then you have to watch for 1 DEAT yes, which is a valid event which has no subobjects,
  // but does have a value. Although GEDCOM specs only mention this usage in association with
  // DEAT, it could equally apply to other events (although it is wholly redundant for BIRT).
  // Note that at the moment we have no way to generate or show the user the existence of a "yes"
  // It looks as though the way forward must once again be to add "dummy" objects to a dummy
  // chain which isn't seen in the "save file" process. That chain needs to be parsed if the
  // users hits "save" in this dbox, and any non-empty objects get added to thisindi (possibly
  // replacing ones already there). The conceptual difference should be that the chain upon
  // which we hold the dummy objects should be associated with *this dbox* and not something
  // global.
  event = thisindi->subobject( BIRT_tag );
  if (event!=NULL) {
    this->setdatefields( event, indi_birthdate, indi_birthtime, indi_birthgreg, indi_birthjul );
    fillin( indi_birthplace, event, PLAC_tag );
    if (thing!=NULL) fillin( indi_birthsite, thing, SITE_tag);
//    fillin( indi_birthsrc, event, SOUR_tag );
//    if ((thing=(event->subobject(SOUR_tag)))!=NULL) indi_birthsrc->value( thing->getxref()->GEDCOM_idname() );
  } else {
    event = new GEDCOM_object( BIRT_tag );
    thisindi->add_subobject(event);
  }
  thing = event->subobject( NOTE_tag );
  if (thing!=NULL) {
    if ((thing->subobject()!=NULL)||(thing->value()!=NULL)) indi_birthnotes->value(1);
  }
  indi_birthnotes->user_data((void*)event);
  indi_birthsources->user_data((void*)event);

  event = thisindi->subobject( CHR_tag );
  if (event!=NULL) {
    this->setdatefields( event, indi_chrdate, indi_chrtime, indi_chrgreg, indi_chrjul );
    fillin( indi_chrplace, event, PLAC_tag);
    if (thing!=NULL) fillin( indi_chrsite, thing, SITE_tag);
//    if ((thing=(event->subobject(SOUR_tag)))!=NULL) indi_chrsrc->value( thing->getxref()->GEDCOM_idname() );
  } else {
    event = new GEDCOM_object( CHR_tag );
    thisindi->add_subobject(event);
  }
  thing = event->subobject( NOTE_tag );
  if (thing!=NULL) {
    if ((thing->subobject()!=NULL)||(thing->value()!=NULL)) indi_chrnotes->value(1);
  }
  indi_chrnotes->user_data((void*)event);
  indi_chrsources->user_data((void*)event);
  event = thisindi->subobject( BAPM_tag );
  if (event!=NULL) {
    this->setdatefields( event, indi_bapmdate, indi_bapmtime, indi_bapmgreg, indi_bapmjul );
    fillin( indi_bapmplace, event, PLAC_tag);
    if (thing!=NULL) fillin( indi_bapmsite, thing, SITE_tag);
//    if ((thing=(event->subobject(SOUR_tag)))!=NULL) indi_bapmsrc->value( thing->getxref()->GEDCOM_idname() );
  } else {
    event = new GEDCOM_object( BAPM_tag );
    thisindi->add_subobject(event);
  }
  thing = event->subobject( NOTE_tag );
  if (thing!=NULL) {
    if ((thing->subobject()!=NULL)||(thing->value()!=NULL)) indi_bapmnotes->value(1);
  }
  indi_bapmnotes->user_data((void*)event);
  indi_bapmsources->user_data((void*)event);
  event = thisindi->subobject( DEAT_tag );
  if (event!=NULL) {
    this->setdatefields( event, indi_deathdate, indi_deathtime, indi_deathgreg, indi_deathjul );
   fillin( indi_deathage, event, AGE_tag);
    fillin( indi_deathplace, event, PLAC_tag);
    if (thing!=NULL) fillin( indi_deathsite, thing, SITE_tag);
    fillin( indi_deathcause, event, CAUS_tag);
//    if ((thing=(event->subobject(SOUR_tag)))!=NULL) indi_deathsrc->value( thing->getxref()->GEDCOM_idname() );
  } else {
    event = new GEDCOM_object( DEAT_tag );
    thisindi->add_subobject(event);
  }
  thing = event->subobject( NOTE_tag );
  if (thing!=NULL) {
    if ((thing->subobject()!=NULL)||(thing->value()!=NULL)) indi_deathnotes->value(1);
  }
  indi_deathnotes->user_data((void*)event);
  indi_deathsources->user_data((void*)event);

  event = thisindi->subobject( CREM_tag );
  if (event!=NULL) {
    this->setdatefields( event, indi_cremdate, indi_cremtime, indi_cremgreg, indi_cremjul );
    fillin( indi_cremplace, event, PLAC_tag);
    if (thing!=NULL) fillin( indi_cremsite, thing, SITE_tag);
//    if ((thing=(event->subobject(SOUR_tag)))!=NULL) indi_cremsrc->value( thing->getxref()->GEDCOM_idname() );
  } else {
    event = new GEDCOM_object( CREM_tag );
    thisindi->add_subobject(event);
  }
  thing = event->subobject( NOTE_tag );
  if (thing!=NULL) {
    if ((thing->subobject()!=NULL)||(thing->value()!=NULL)) indi_cremnotes->value(1);
  }
  indi_cremnotes->user_data((void*)event);
  indi_cremsources->user_data((void*)event);

  event = thisindi->subobject( BURI_tag );
  if (event!=NULL) {
    this->setdatefields( event, indi_buridate, indi_buritime, indi_burigreg, indi_burijul );
    fillin( indi_buriplace, event, PLAC_tag);
    if (thing!=NULL) {
      fillin( indi_burisite, thing, CEME_tag);
      // 2022-12-14 we should not have a PLOT if CEME is not defined (even if a bug in GEDCOM means it exists...)
      if (thing!=NULL) fillin( indi_buriplot, thing, PLOT_tag);
    }
//    if ((thing=(event->subobject(SOUR_tag)))!=NULL) indi_burisrc->value( thing->getxref()->GEDCOM_idname() );
  } else {
    event = new GEDCOM_object( BURI_tag );
    thisindi->add_subobject(event);
  }
  thing = event->subobject( NOTE_tag );
  if (thing!=NULL) {
    if ((thing->subobject()!=NULL)||(thing->value()!=NULL)) indi_burinotes->value(1);
  }
  indi_burinotes->user_data((void*)event);
  indi_burisources->user_data((void*)event);
  indi_events->value(indi_birth);
  printf("indiUI::insert_details finished\n");
}

void indiUI::refresh_parents( ) {
GEDCOM_object *thing; // deeper subobjects
GEDCOM_object *parent;
  GEDCOM_object* fam = who->parental_family();
  // we should never be called on an indibox with no parents because we are called from a
  // FAM which has a CHIL pointing to our indi... but test anyway
  if (fam!=NULL) {
    parent =  fam->thewife();
    if (parent != NULL ) {
      fillin( indi_ma, parent, NAME_tag );
      indi_ma_id->value(parent->getidname());
      indi_ma_add->hide();
      //indi_pa_add->hide();
      indi_fam_find->hide();
      indi_ma_id->show();
      //indi_pa_id->show();
    }
    parent = fam->thehusband();
    if (parent != NULL ) {
      fillin( indi_pa, parent, NAME_tag );
      indi_pa_id->value(parent->getidname());
      //indi_ma_add->hide();
      indi_pa_add->hide();
      indi_fam_find->hide();
      //indi_ma_id->show();
      indi_pa_id->show();
    }
  }
  else {
    fl_alert("fam should never be null in refresh_parents() - it was \n");
  }
}
#undef fillin

void indiUI::setdatefields( GEDCOM_object* event, Fl_Input* date, Fl_Input* time, Fl_Button* greg, Fl_Button* jul ) {
GEDCOM_object* thing;
char* val;
int i;

  if ((thing=(event->subobject(DATE_tag)))!=NULL) {
    if (*(val=(thing->value()))=='@') {
      switch (*(val+3)) {
        case 'G': greg->setonly(); break;
        case 'J': jul->setonly(); break;
      }
      // now rest of date string into the field ...
      i=2;while ((*(val+i))!='@') i++;i+=2;
      date->value(val+i);
    }
    else date->value(val);
    if ((thing=(thing->subobject(TIME_tag)))!=NULL) time->value( thing->value() );
  }
}

void indiUI::checknotes( GEDCOM_object* raised_on, bool ui_open) {
// called when a notesUI starts up or exits, so we can check which events
// have a NOTE object extant, and light up the appropriate button(s).
// FIXME we know this code doesn't do all it should - if a notesUI is
// already open, we fail to notice when we open the relevant indiUI or
// famUI.
GEDCOM_object *note; // deeper subobjects
GEDCOM_tag *which;
int light;
int colour;

  if (ui_open) { light=1; colour=FL_RED; }
  else {
    note = raised_on->subobject( NOTE_tag );
    // if we know there isn't a notesUI open, then the existence of a NOTE is
    // sufficient without checking inside it
    if (note!=NULL) {
      light=1;
      colour=FL_BLACK;
    } else { light=0; }
  }
  //printf("Think the light should now have value %d, colour %d\n",light, colour);
  // now decide which button we are wanting to deal with:
  which = raised_on->objtype();
  if (which==INDI_tag) {
    // printf("checknotes being called for indiUI at %ld\n",(long)this);
    indi_notes->value(light);
    if (light==1) indi_notes->selection_color(colour);
  }
  if (which==BIRT_tag) {
    indi_birthnotes->value(light);
    if (light==1) indi_birthnotes->selection_color(colour);
  }
  if (which==CHR_tag) {
    indi_chrnotes->value(light);
    if (light==1) indi_chrnotes->selection_color(colour);
  }
  if (which==BAPM_tag) {
    indi_bapmnotes->value(light);
    if (light==1) indi_bapmnotes->selection_color(colour);
  }
  if (which==DEAT_tag) {
    indi_deathnotes->value(light);
    if (light==1) indi_deathnotes->selection_color(colour);
  }
  if (which==CREM_tag) {
    indi_cremnotes->value(light);
    if (light==1) indi_cremnotes->selection_color(colour);
  }
  if (which==BURI_tag) {
    indi_burinotes->value(light);
    if (light==1) indi_burinotes->selection_color(colour);
  }
}


void indiUI::settitle() {

// ensure caller has called insert_details first for a real indi, or you
// will get an "edit <new person>" title.

  std::string title("X!Family edit");

  title += " : ";
  title += which->getfilename();
  title += " : ";

  if (who==NULL) {
    // edit window for new person - no object yet
    printf("In v 0.20 we were not expecting to be called to settitle with NULL who\n");
    title += "<new person>";
  } else {
    printf("indiUI::settitle wants idname for %ld\n",(long)who);
    title += who->getidname();
/*    GEDCOM_object* name = who->subobject( NAME_tag );
    if ( name == NULL ) {
      // edit window for existing person with no name
      title += "<unnamed>";
    } else {
      // edit window for a person with a NAME tag
      char *namestring = name->value();
      if (namestring == NULL) title += "<unnamed>";
      else title += namestring;
      } */
  }
  // now put the title somewhere it won't evaporate as title goes out of scope:
  printf("indiUI::settitle %s\n",title.c_str());
  strncpy(buf_indi_title, title.c_str(), MAX_indititle);
  indi_dbox->label(buf_indi_title);
}

void indiUI::update() {
  who->update(NAME_tag,(char*)(indi_name->value()));
  who->update(TITL_tag,(char*)(indi_title->value()));
  char* gender;
  if ((indi_male->value())==1) gender = strdup("M");
  if ((indi_female->value())==1) gender = strdup("F");
  if ((indi_unknown->value())==1) gender = strdup("");
  who->update(SEX_tag,gender);
  who->update_event(BIRT_tag, (char*)(indi_birthdate->value()), (char*)(indi_birthtime->value()),  (char*)(indi_birthplace->value()),
                              (char*)(indi_birthsite->value()), NULL, NULL, NULL );
  who->update_event(CHR_tag,  (char*)(indi_chrdate->value()),   (char*)(indi_chrtime->value()),    (char*)(indi_chrplace->value()),
                              (char*)(indi_chrsite->value()),   NULL, NULL, NULL );
  who->update_event(BAPM_tag, (char*)(indi_bapmdate->value()),  (char*)(indi_bapmtime->value()),   (char*)(indi_bapmplace->value()),
                              (char*)(indi_bapmsite->value()),  NULL, NULL, NULL );
  who->update_event(DEAT_tag, (char*)(indi_deathdate->value()), (char*)(indi_deathtime->value()),  (char*)(indi_deathplace->value()),
                              (char*)(indi_deathsite->value()), NULL, (char*)(indi_deathcause->value()), (char*)(indi_deathage->value()) );
  who->update_event(CREM_tag, (char*)(indi_cremdate->value()),  (char*)(indi_cremtime->value()),   (char*)(indi_cremplace->value()),
                              (char*)(indi_cremsite->value()),  NULL, NULL, NULL );
  who->update_event(BURI_tag, (char*)(indi_buridate->value()),  (char*)(indi_buritime->value()),   (char*)(indi_buriplace->value()),
                              (char*)(indi_burisite->value()),  (char*)(indi_buriplot->value()), NULL, NULL );
}

void indiUI::show() const {
  indi_dbox->show();
}

void indiUI::hide() const {
  indi_dbox->hide();
}

///////////////////////////////////////////////////////////////////////////////
// class famUI the "edit family" dbox. We will find a pointer to a
// GEDCOM object is all we need, to know which family (and thus which
// treeinstance) this relates to. Its not immediately clear if this UI will
// be used to create new families (ie. the "Marry" menu item), if so we
// don't make a GEDCOM_object until user clicks OK, so we also need a tree
// instance (not a mainUI) pointer in the famUI class.

famUI::famUI( treeinstance* thistree, GEDCOM_object* famfor ):
  fam (famfor),       // NULL for a new family not yet in the tree
                      // if UI is used that way (probably won't be...)
  which (thistree)
{
printf("new famUI on treeinstance %ld\n",(long)which);
// make editing layout easier (maybe?):
#define col0 0
#define col1 75
#define normh 25
#define row 30
#define bigh 30
#define bigrow 35
#define smallh 24
#define fullw 390
//#define redw 310
#define redw 260
#define idcol col1+redw+5
//#define idw 75
#define idw 55
#define addcol col1+redw+5
#define addw  55
#define findcol col1+redw+5+addw+10
#define remcol col1+redw+5+addw+5
#define findw 60
#define remw 65
#define dboxwidth col1+fullw+15
#define datew 200
#define timew 130
#define buttonw 110
#define biggap 25
#define smallgap 5
#define b3l col1+5
#define b3m b3l+buttonw+biggap
#define b3r b3m+buttonw+biggap
#define b41 10
#define b42 b41+buttonw+smallgap
#define b43 b42+buttonw+smallgap
#define b44 b43+buttonw+smallgap
#define husbrow 5
#define wiferow husbrow+row
#define notesrow wiferow+row
#define notestop notesrow+3
#define tabsrow notesrow+bigrow
#define tilerow tabsrow+normh
#define daterow tilerow+10
#define calrow daterow+row
#define caltop calrow+3
#define placerow calrow+bigrow
#define siterow placerow+row
#define sourcerow siterow+row
#define tabheight row+row+bigrow+row+row+bigrow+5
#define tileheight row+bigrow+row+row+bigrow+5
//#define revrow sourcerow+row+15
#define helprow sourcerow+row+15
#define okrow helprow+row+15
#define canrow helprow+row+15
#define approw helprow+row+15
#define cancol 45
#define okcol 335
//#define helpcol 335
#define helpcol 190
#define appcol 190
//#define revcol 45
#define dboxheight okrow+row+5
// don't seem to use:   Fl_Window* w;
  { Fl_Window* o = fam_dbox = new Fl_Window( dboxwidth, dboxheight, buf_fam_title ); // *not* msg_fam_title :-)
// don't seem to use:     w = o;
    o->callback((Fl_Callback*)cancelfam_cb);
    o->user_data((void*)(this));
    husb_name = new Fl_Output(col1, husbrow, redw, normh, "Husband");
    husb_id = new Fl_Output(idcol, husbrow, idw, normh, "");
    wife_name = new Fl_Output(col1, wiferow, redw, normh, "Wife");
    wife_id = new Fl_Output(idcol, wiferow, idw, normh, "");
    husb_add = new Fl_Button(addcol,husbrow,addw,normh,"Add");
        husb_add->down_box(FL_DOWN_BOX);
        husb_add->type(0);
        husb_add->selection_color(FL_BLACK);
        husb_add->labelfont(1);
        husb_add->callback((Fl_Callback*)buttonaddhusb_cb);
        husb_add->user_data((void*)(this));
        husb_add->hide();
    husb_find = new Fl_Button(findcol,husbrow,findw,normh,"Find");
        husb_find->down_box(FL_DOWN_BOX);
        husb_find->type(0);
        husb_find->selection_color(FL_BLACK);
        husb_find->labelfont(1);
        husb_find->callback((Fl_Callback*)buttonfindhusb_cb);
        husb_find->user_data((void*)(this));
        husb_find->hide();
    husb_remove = new Fl_Button(remcol,husbrow,remw,normh,"Remove");
        husb_remove->down_box(FL_DOWN_BOX);
        husb_remove->type(0);
        husb_remove->selection_color(FL_BLACK);
        husb_remove->labelfont(1);
        husb_remove->callback((Fl_Callback*)buttonremovehusb_cb);
        husb_remove->user_data((void*)(this));
        husb_remove->hide();
    wife_add = new Fl_Button(addcol,wiferow,addw,normh,"Add");
        wife_add->down_box(FL_DOWN_BOX);
        wife_add->type(0);
        wife_add->selection_color(FL_BLACK);
        wife_add->labelfont(1);
        wife_add->callback((Fl_Callback*)buttonaddwife_cb);
        wife_add->user_data((void*)(this));
        wife_add->hide();
    wife_find = new Fl_Button(findcol,wiferow,findw,normh,"Find");
        wife_find->down_box(FL_DOWN_BOX);
        wife_find->type(0);
        wife_find->selection_color(FL_BLACK);
        wife_find->labelfont(1);
        wife_find->callback((Fl_Callback*)buttonfindwife_cb);
        wife_find->user_data((void*)(this));
        wife_find->hide();
    wife_remove = new Fl_Button(remcol,wiferow,remw,normh,"Remove");
        wife_remove->down_box(FL_DOWN_BOX);
        wife_remove->type(0);
        wife_remove->selection_color(FL_BLACK);
        wife_remove->labelfont(1);
        wife_remove->callback((Fl_Callback*)buttonremovewife_cb);
        wife_remove->user_data((void*)(this));
        wife_remove->hide();

    { Fl_Group* o = new Fl_Group(col1, notesrow, fullw, bigh);
      o->box(FL_EMBOSSED_BOX);
      { Fl_Button* o = fam_notes = new Fl_Light_Button(b42, notestop, buttonw, smallh, "Notes");
        o->down_box(FL_DOWN_BOX);
        o->type(0);
        o->selection_color(FL_BLACK);
        o->labelfont(1);
        o->callback((Fl_Callback*)buttonopennotes_cb);
        o->user_data((void*)(fam));
      }
      { Fl_Button* o = fam_sources = new Fl_Button(b43, notestop, buttonw, smallh, "Sources");
        o->down_box(FL_DOWN_BOX);
        o->labelfont(1);
        o->callback((Fl_Callback*)buttonopensourceui_cb);
        o->user_data((void*)(fam));
      }
      o->end();
    }
    { Fl_Tabs* o = fam_events = new Fl_Tabs(col0, tabsrow, dboxwidth, tabheight);
      o->labeltype(FL_NO_LABEL);
      { Fl_Tile* o = fam_marry = new Fl_Tile(col0, tilerow, dboxwidth, tileheight, "Marriage");
        o->color(246);
        o->selection_color(246);
        fam_marrydate = new Fl_Input(col1, daterow, datew, normh, "Date");
        fam_marrytime = new Fl_Input(col1+fullw-timew, daterow, timew, normh, "Time");
        { Fl_Box* o = new Fl_Box(col0+5, caltop, col1-5, smallh, "Calendar");
          o->align(FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
        }
        { Fl_Group* o = new Fl_Group(col1, calrow, fullw, bigh);
          o->box(FL_EMBOSSED_BOX);
          o->color(246);
          { Fl_Light_Button* o = fam_marrygreg = new Fl_Light_Button(b3l, caltop, buttonw, smallh, "Gregorian");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          { Fl_Light_Button* o = fam_marryjul = new Fl_Light_Button(b3m, caltop, buttonw, smallh, "Julian");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          { Fl_Light_Button* o = fam_marrydef = new Fl_Light_Button(b3r, caltop, buttonw, smallh, "default");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          o->end();
        }
        fam_marryplace = new Fl_Input(col1, placerow, fullw, normh, "Place");
        fam_marrysite = new Fl_Input(col1, siterow, fullw, normh, "Site");
        fam_marrynotes = new Fl_Light_Button(b42, sourcerow, buttonw, normh, "Notes");
        fam_marrynotes->selection_color(FL_BLACK);
	fam_marrynotes->color(246);
        fam_marrynotes->type(0);
        fam_marrynotes->down_box(FL_DOWN_BOX);
	fam_marrynotes->callback((Fl_Callback*)buttonopennotes_cb);
	fam_marrysources = new Fl_Button(b43, sourcerow, buttonw, normh, "Sources");
	fam_marrysources->color(246);
	fam_marrysources->callback((Fl_Callback*)buttonopensourceui_cb);
//        fam_marrysrc = new Fl_Output(75, 230, 390, 25, msg_event_src);
        //o->hide();
        o->end();
      }
      { Fl_Tile* o = fam_engage = new Fl_Tile(col0, tilerow, dboxwidth, tileheight, "Engagement");
        o->color(214);
        o->selection_color(214);
        fam_engagedate = new Fl_Input(col1, daterow, datew, normh, "Date");
        fam_engagetime = new Fl_Input(col1+fullw-timew, daterow, timew, normh, "Time");
        { Fl_Box* o = new Fl_Box(col0+5, caltop, col1-5, smallh, "Calendar");
          o->align(FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
        }
        { Fl_Group* o = new Fl_Group(col1, calrow, fullw, bigh);
          o->box(FL_EMBOSSED_BOX);
          o->color(214);
          { Fl_Light_Button* o = fam_engagegreg = new Fl_Light_Button(b3l, caltop, buttonw, smallh, "Gregorian");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          { Fl_Light_Button* o = fam_engagejul = new Fl_Light_Button(b3m, caltop, buttonw, smallh, "Julian");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          { Fl_Light_Button* o = fam_engagedef = new Fl_Light_Button(b3r, caltop, buttonw, smallh, "default");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          o->end();
        }
        fam_engageplace = new Fl_Input(col1, placerow, fullw, normh, "Place");
        fam_engagesite = new Fl_Input(col1, siterow, fullw, normh, "Site");
        fam_engagenotes = new Fl_Light_Button(b42, sourcerow, buttonw, normh, "Notes");
        fam_engagenotes->selection_color(FL_BLACK);
	fam_engagenotes->color(214);
        fam_engagenotes->type(0);
        fam_engagenotes->down_box(FL_DOWN_BOX);
	fam_engagenotes->callback((Fl_Callback*)buttonopennotes_cb);
	fam_engagesources = new Fl_Button(b43, sourcerow, buttonw, normh, "Sources");
	fam_engagesources->color(214);
	fam_engagesources->callback((Fl_Callback*)buttonopensourceui_cb);
//        fam_engagesrc = new Fl_Output(75, 230, 390, 25, msg_event_src);
        o->end();
      }
      { Fl_Tile* o = fam_divorce = new Fl_Tile(col0, tilerow, dboxwidth, tileheight, "Divorce");
        o->color(207);
        o->selection_color(207);
        fam_divorcedate = new Fl_Input(col1, daterow, datew, normh, "Date");
        fam_divorcetime = new Fl_Input(col1+fullw-timew, daterow, timew, normh, "Time");
        { Fl_Box* o = new Fl_Box(col0+5, caltop, col1-5, smallh, "Calendar");
          o->align(FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
        }
        { Fl_Group* o = new Fl_Group(col1, calrow, fullw, bigh);
          o->box(FL_EMBOSSED_BOX);
          o->color(207);
          { Fl_Light_Button* o = fam_divorcegreg = new Fl_Light_Button(b3l, caltop, buttonw, smallh, "Gregorian");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          { Fl_Light_Button* o = fam_divorcejul = new Fl_Light_Button(b3m, caltop, buttonw, smallh, "Julian");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          { Fl_Light_Button* o = fam_divorcedef = new Fl_Light_Button(b3r, caltop, buttonw, smallh, "default");
            o->type(102); /* radio button */
            o->selection_color(FL_RED);
          }
          o->end();
        }
        fam_divorceplace = new Fl_Input(col1, placerow, fullw, normh, "Place");
        fam_divorcesite = new Fl_Input(col1, siterow, fullw, normh, "Site");
        fam_divorcenotes = new Fl_Light_Button(b42, sourcerow, buttonw, normh, "Notes");
        fam_divorcenotes->selection_color(FL_BLACK);
	fam_divorcenotes->color(207);
        fam_divorcenotes->type(0);
        fam_divorcenotes->down_box(FL_DOWN_BOX);
	fam_divorcenotes->callback((Fl_Callback*)buttonopennotes_cb);
	fam_divorcesources = new Fl_Button(b43, sourcerow, buttonw, normh, "Sources");
	fam_divorcesources->color(207);
	fam_divorcesources->callback((Fl_Callback*)buttonopensourceui_cb);
//        fam_divorcesrc = new Fl_Output(75, 230, 390, 25, msg_event_src);
        //o->hide();
        o->end();
      }
      o->end();
    }
    { Fl_Button* o = fam_cancel = new Fl_Button(cancol, canrow, buttonw, normh, "Cancel");
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)cancelfam_cb);
      o->user_data((void*)(this));
    }
    { Fl_Return_Button* o = fam_ok = new Fl_Return_Button(okcol, okrow, buttonw, normh, "OK");
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)okfam_cb);
      o->user_data((void*)(this));
    }
    { Fl_Button* o = fam_help = new Fl_Button(helpcol, helprow, buttonw, normh, "Help");
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)help_cb);
      o->user_data((void*)("famUI.html"));
    }
/*    { Fl_Button* o = fam_restore = new Fl_Button(revcol, revrow, buttonw, normh, "Revert");
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)restorefam_cb);
      o->user_data((void*)(this));
    } */
    { Fl_Button* o = fam_commit = new Fl_Button(appcol, approw, buttonw, normh, "Apply");
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)commitfam_cb);
      o->user_data((void*)(this));
    }
    o->end();
  }
#undef col0
#undef col1
#undef normh
#undef row
#undef bigh
#undef bigrow
#undef smallh
#undef fullw
#undef dboxwidth
#undef datew
#undef timew
#undef buttonw
#undef biggap
#undef smallgap
#undef b3l
#undef b3m
#undef b3r
#undef b41
#undef b42
#undef b43
#undef b44
#undef husbrow
#undef wiferow
#undef notesrow
#undef notestop
#undef tabsrow
#undef tilerow
#undef daterow
#undef calrow
#undef caltop
#undef placerow
#undef siterow
#undef sourcerow
#undef tabheight
#undef tileheight
#undef revrow
#undef revcol
#undef helprow
#undef helpcol
#undef okrow
#undef okcol
#undef canrow
#undef cancol
#undef approw
#undef appcol
#undef dboxheight
}

famUI::~famUI() {
#ifdef destructorlogs
  printf("~famUI %ld, dbox %ld\n",(long)this,(long)fam_dbox);
#endif
  delete fam_dbox;
}

GEDCOM_object* famUI::whatis() const {
  return fam;
}

treeinstance* famUI::whichis() const {
  return which;
}

famUI* famUI::getnext() const {
  return next;
}

void famUI::setnext( famUI* nextui ) {
  next = nextui;
}

famUI* famUI::getprevious() const {
  return previous;
}

void famUI::setprevious( famUI* prevui ) {
  previous = prevui;
}

void famUI::clear_details() {
  husb_name->value("");
  wife_name->value("");
  husb_id->value("");
  wife_id->value("");
  fam_marrydate->value("");
  fam_marrytime->value("");
  fam_marryplace->value("");
  fam_marrysite->value("");
  fam_marrydef->setonly();
  fam_marrynotes->user_data(NULL);
  fam_marrysources->user_data(NULL);
  fam_engagedate->value("");
  fam_engagetime->value("");
  fam_engageplace->value("");
  fam_engagesite->value("");
  fam_engagedef->setonly();
  fam_engagenotes->user_data(NULL);
  fam_engagesources->user_data(NULL);
  fam_divorcedate->value("");
  fam_divorcetime->value("");
  fam_divorceplace->value("");
  fam_divorcesite->value("");
  fam_divorcedef->setonly();
  fam_divorcenotes->user_data(NULL);
  fam_divorcesources->user_data(NULL);
}

void famUI::insert_details( GEDCOM_object* thisfam ) {
GEDCOM_object *event;
GEDCOM_object *thing;
GEDCOM_object *indi;
int check;

  this->clear_details();

#define fillin( field, object, tag ) if ((thing=(object->subobject(tag)))!=NULL) field->value( thing->value() )

// with a FAM object, names of spouses are not directly accessible - we need
// to dereference the HUSB and WIFE pointers to get to the relevant INDI
// objects

  check = 0;
  // dereference to husb indi - there is no guarantee that there is one !
  thing = thisfam->subobject( HUSB_tag );
  if (thing!=NULL) {
    check++;
    // then thing->ref is a GEDCOM_id which will lead to the right INDI
    indi = thing->followxref();
    if (indi!=NULL) fillin( husb_name, indi, NAME_tag );
    // if indi *is* NULL, then the GEDCOM is broken and we should probably
    // do something about that (if only report the fact)
    husb_id->value( indi->getidname());
  } else {
    printf("Will try to swap husb id box for add/find buttons\n");
    husb_id->hide();
    husb_add->show();
    husb_find->show();
  }

  // dereference to WIFE INDI
  thing = thisfam->subobject( WIFE_tag );
  if (thing!=NULL) {
    check++;
    indi = thing->followxref();
    if (indi!=NULL) fillin( wife_name, indi, NAME_tag );
    wife_id->value( indi->getidname());
  } else {
    printf("Will try to swap wife id box for add/find buttons\n");
    wife_id->hide();
    wife_add->show();
    wife_find->show();
  }

  if (check==2) {
    wife_remove->show();
    husb_remove->show();
  }

  // we do not in practice expect that thing would have been null for both
  // husband and wife, but we should be able to cope with that. In theory,
  // a FAM object can have just CHIL subobjects to indicate siblinghood.
  // However, in a case with neither HUSB nor WIFE, we probably should not
  // allow the setting of ENGA, MARR, DIV or similar objects...

  thing = thisfam->subobject( NOTE_tag );
  if (thing!=NULL) {
    if ((thing->subobject()!=NULL)||(thing->value()!=NULL)) fam_notes->value(1);
  }
  fam_notes->user_data((void*)thisfam);
  fam_sources->user_data((void*)thisfam);

  event = thisfam->subobject( MARR_tag );
  if (event!=NULL) {
    this->setdatefields( event, fam_marrydate, fam_marrytime, fam_marrygreg, fam_marryjul );
    fillin( fam_marryplace, event, PLAC_tag);
    if (thing!=NULL) fillin( fam_marrysite, thing, SITE_tag);
//    if ((thing=(event->subobject(SOUR_tag)))!=NULL) fam_marrysrc->value( thing->getxref()->GEDCOM_idname() );
  }
  else
  {
    event = new GEDCOM_object( MARR_tag );
    thisfam->add_subobject(event);
  }
// we should only make the effort to look for a NOTE if the MARR object already existed FIXME
  thing = event->subobject( NOTE_tag );
  if (thing!=NULL) {
    if ((thing->subobject()!=NULL)||(thing->value()!=NULL)) fam_marrynotes->value(1);
  }
  fam_marrynotes->user_data((void*)event);
  fam_marrysources->user_data((void*)event);

  event = thisfam->subobject( ENGA_tag );
  if (event!=NULL) {
    this->setdatefields( event, fam_engagedate, fam_engagetime, fam_engagegreg, fam_engagejul );
    fillin( fam_engageplace, event, PLAC_tag );
    if (thing!=NULL) fillin( fam_engagesite, thing, SITE_tag );
//    if ((thing=(event->subobject(SOUR_tag)))!=NULL) fam_engagesrc->value( thing->getxref()->GEDCOM_idname() );
  }
  else
  {
    event = new GEDCOM_object( ENGA_tag );
    thisfam->add_subobject(event);
  }
  thing = event->subobject( NOTE_tag );
  if (thing!=NULL) {
    if ((thing->subobject()!=NULL)||(thing->value()!=NULL)) fam_engagenotes->value(1);
  }
  fam_engagenotes->user_data((void*)event);
  fam_engagesources->user_data((void*)event);

  event = thisfam->subobject( DIV_tag );
  if (event!=NULL) {
    this->setdatefields( event, fam_divorcedate, fam_divorcetime, fam_divorcegreg, fam_divorcejul );
    fillin( fam_divorceplace, event, PLAC_tag);
    if (thing!=NULL) fillin( fam_divorcesite, thing, SITE_tag);
//    if ((thing=(event->subobject(SOUR_tag)))!=NULL) fam_divorcesrc->value( thing->getxref()->GEDCOM_idname() );
  }
  else
  {
    event = new GEDCOM_object( DIV_tag );
    thisfam->add_subobject(event);
  }
  thing = event->subobject( NOTE_tag );
  if (thing!=NULL) {
    if ((thing->subobject()!=NULL)||(thing->value()!=NULL)) fam_divorcenotes->value(1);
  }
  fam_divorcenotes->user_data((void*)event);
  fam_divorcesources->user_data((void*)event);

}
#undef fillin

void famUI::refresh_spouse( ) {

GEDCOM_object *spouse;
GEDCOM_object *thing;

  printf("famUI::refresh_spouse\n");
  spouse = fam->subobject( HUSB_tag );
  if (spouse!=NULL) {
    spouse = spouse->followxref();
    if (spouse!=NULL) {
      // indi should never be null if there was a HUSB tag... but the name might be empty
      if ((thing=(spouse->subobject(NAME_tag)))!=NULL) husb_name->value( thing->value() );
      husb_id->value( spouse->getidname());
      husb_name->show(); // may or may not need this to redraw
      husb_id->show();
      husb_add->hide();
      husb_find->hide();
    } else {
      husb_id->hide();
      husb_add->show();
      husb_find->show();
    }
  }
  spouse = fam->subobject( WIFE_tag );
  if (spouse!=NULL) {
    spouse = spouse->followxref();
    if (spouse!=NULL) {
      // indi should never be null if there was a WIFE tag... but the name might be empty
      if ((thing=(spouse->subobject(NAME_tag)))!=NULL) wife_name->value( thing->value() );
      wife_id->value( spouse->getidname());
      wife_name->show(); // may or may not need this to redraw
      wife_id->show();
      wife_add->hide();
      wife_find->hide();
    } else {
      wife_id->hide();
      wife_add->show();
      wife_find->show();
    }
  }
}

void famUI::setdatefields( GEDCOM_object* event, Fl_Input* date, Fl_Input* time, Fl_Button* greg, Fl_Button* jul ) {
GEDCOM_object* thing;
char* val;
int i;

  if ((thing=(event->subobject(DATE_tag)))!=NULL) {
    if (*(val=(thing->value()))=='@') {
      switch (*(val+3)) {
        case 'G': greg->setonly(); break;
        case 'J': jul->setonly(); break;
      }
      // now rest of date string into the field ...
      i=2;while ((*(val+i))!='@') i++;i+=2;
      date->value(val+i);
    }
    else date->value(val);
    if ((thing=(thing->subobject(TIME_tag)))!=NULL) time->value( thing->value() );
  }
}

void famUI::checknotes( GEDCOM_object* raised_on, bool ui_open) {
// called when a notesUI starts up or exits, so we can check which events
// have a NOTE object extant, and light up the appropriate button(s).
GEDCOM_object *note; // deeper subobjects
GEDCOM_tag *which;
int light;
int colour;

  if (ui_open) { light=1; colour=FL_RED; }
  else {
    note = raised_on->subobject( NOTE_tag );
    // if we know there isn't a notesUI open, then the existence of a NOTE is
    // sufficient without checking inside it
    if (note!=NULL) {
      light=1;
      colour=FL_BLACK;
    } else { light=0; }
  }
  //printf("Think the light should now have value %d, colour %d\n",light, colour);
  // now decide which button we are wanting to deal with:
  which = raised_on->objtype();
  if (which==FAM_tag) {
    // printf("checknotes being called for famUI at %ld\n",(long)this);
    fam_notes->value(light);
    if (light==1) fam_notes->selection_color(colour);
  }
  if (which==MARR_tag) {
    fam_marrynotes->value(light);
    if (light==1) fam_marrynotes->selection_color(colour);
  }
  if (which==ENGA_tag) {
    fam_engagenotes->value(light);
    if (light==1) fam_engagenotes->selection_color(colour);
  }
  if (which==DIV_tag) {
    fam_divorcenotes->value(light);
    if (light==1) fam_divorcenotes->selection_color(colour);
  }
}

void famUI::settitle() {

// ensure caller has called insert_details first for a real indi, or you
// will get an "edit <new person>" title.

  printf("Starting settitle on new famUI %ld\n",(long)fam);

  std::string title("X!Family family events");

  printf("basic title for new famUI %ld as %s\n", (long)fam,title.c_str());
  title += " : ";
  title += which->getfilename();
  title += " : ";
  printf("Added filename to title for new famUI %ld as %s\n", (long)fam,title.c_str());

  if (fam==NULL) {
    // edit window for new family (shouldn't happen) - no object yet
    title += "<new family>";
  } else {
    title += fam->getidname();
    printf("Setting title for new famUI %ld to %s\n", (long)fam,title.c_str());
  }
  // now put the title somewhere it won't evaporate as title goes out of scope:
  strncpy(buf_fam_title, title.c_str(), MAX_famtitle);
  fam_dbox->label(buf_fam_title);
}

void famUI::update() {
  fam->update_event(MARR_tag, (char*)(fam_marrydate->value()), (char*)(fam_marrytime->value()),
    (char*)(fam_marryplace->value()), (char*)(fam_marrysite->value()), NULL, NULL, NULL );
  printf("Updated MARR and returned\n");
  fam->update_event(ENGA_tag, (char*)(fam_engagedate->value()), (char*)(fam_engagetime->value()),
    (char*)(fam_engageplace->value()), (char*)(fam_engagesite->value()), NULL, NULL, NULL );
  printf("Updated ENGA and returned\n");
  fam->update_event(DIV_tag, (char*)(fam_divorcedate->value()), (char*)(fam_divorcetime->value()),
    (char*)(fam_divorceplace->value()), (char*)(fam_divorcesite->value()), NULL, NULL, NULL );
  printf("Updated DIV and returned\n");
}

void famUI::show() const {
  fam_dbox->show();
}

void famUI::hide() const {
  fam_dbox->hide();
}

///////////////////////////////////////////////////////////////////////////////
// class statsUI displays statistics for the tree - one instanciation per tree
// loaded - but we display the relevant filename, so user shouldn't have trouble
// telling which is which (and anyway, can't alter anything)

statsUI::statsUI( treeinstance* instanciation ):
  which (instanciation)
{
// don't seem to use:   Fl_Window* w;
  { Fl_Window* o = statsdbox = new Fl_Window(520, 165, "X!Family Statistics");
// don't seem to use:     w = o;
    o->user_data((void*)(this));
    { Fl_Output* o = statsfile = new Fl_Output(140, 10, 380, 25, "GEDCOM File");
      o->box(FL_RFLAT_BOX);
      o->color(49);
      o->textfont(1);
    }
    { Fl_Output* o = indicount = new Fl_Output(165, 45, 95, 25, "Individuals");
      o->color(49);
    }
    { Fl_Output* o = indimax = new Fl_Output(400, 45, 110, 25, "Maximum INDI Id");
      o->color(49);
    }
    { Fl_Output* o = famcount = new Fl_Output(165, 75, 95, 25, "Families");
      o->color(49);
    }
    { Fl_Output* o = fammax = new Fl_Output(400, 75, 110, 25, "Maximum FAM Id");
      o->color(49);
    }
    { Fl_Output* o = srccount = new Fl_Output(165, 105, 95, 25, "Information Sources");
      o->color(49);
    }
    { Fl_Output* o = srcmax = new Fl_Output(400, 105, 110, 25, "Maximum SOUR Id");
      o->color(49);
    }
    { Fl_Output* o = repocount = new Fl_Output(165, 135, 95, 25, "Information Repositories");
      o->color(49);
    }
    { Fl_Output* o = repomax = new Fl_Output(400, 135, 110, 25, "Maximum REPO Id");
      o->color(49);
    }
    o->end();
  }
}

// we also need a statsUI::show() method which sets the values displayed, as in
// char values[64];
// sprintf(values,"%d",GEDCOM_indicount);
// indicount->value(values);
// sprintf(values,"I%d",GEDCOM_nextIid-1); ... or whatever.

bool statsUI::shown() const {
  return statsdbox->shown();
}

void statsUI::hide() const {
  statsdbox->hide();
}

void statsUI::setvalues() {
  statsfile->value(which->getfilename());
  char buf[64];
  sprintf(buf,"%d",which->getindicount());
  indicount->value(buf);
  sprintf(buf,"%d",which->getfamcount());
  famcount->value(buf);
  sprintf(buf,"%d",which->getsourcount());
  srccount->value(buf);
  sprintf(buf,"%d",which->getrepocount());
  repocount->value(buf);
  sprintf(buf,"%d",which->getmaxindi());
  indimax->value(buf);
  sprintf(buf,"%d",which->getmaxfam());
  fammax->value(buf);
  sprintf(buf,"%d",which->getmaxsour());
  srcmax->value(buf);
  sprintf(buf,"%d",which->getmaxrepo());
  repomax->value(buf);
}

void statsUI::show() const {
  statsdbox->show();
}

///////////////////////////////////////////////////////////////////////////////
// class indifindUI controls find person window - only one instance of the window
// as it is a short-lived transient sort of thing. It will search the tree of
// the window it was raised on, and pass the handle of that window to its OK
// callback, so the current person can be set *for that window*.
// Similarly, if it raises a completions window, the callback from choosing a
// name there will pass the handle of *this* window to *it's* OK callback, so
// we can find out the name that was chosen (or wasn't, if it was Cancel)

indifindUI::indifindUI() {
  { Fl_Window* o = findbox = new Fl_Window(500, 65, "X!Family Search tree for person");
    o->labeltype(FL_NO_LABEL);
    o->user_data((void*)(this));
    find_input = new Fl_Input( 65, 5, 305, 25, "Goto" );
    find_id = new Fl_Output( 375, 5, 40, 25, "");
    { Fl_Button* o = find_fromright = new Fl_Button( 420, 5, 25, 25, "@<");
      o->labeltype(FL_SYMBOL_LABEL);
      o->callback((Fl_Callback*)searchindifind_cb);
    }
    { Fl_Button* o = find_fuzzy = new Fl_Button( 445, 5, 25, 25, "@<->");
      o->labeltype(FL_SYMBOL_LABEL);
      o->callback((Fl_Callback*)searchindifind_cb);
    }
    { Fl_Button* o = find_fromleft = new Fl_Button( 470, 5, 25, 25, "@>");
      o->labeltype(FL_SYMBOL_LABEL);
      o->callback((Fl_Callback*)searchindifind_cb);
    }
    { Fl_Button* o = find_help = new Fl_Button(65, 35, 100, 25, "Help");
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)help_cb);
      o->user_data((void*)("indifindUI.html"));
    }
    { Fl_Button* o = find_cancel = new Fl_Button(190, 35, 100, 25, "Cancel");
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)binnedindifind_cb);
    }
    { Fl_Return_Button* o = find_ok = new Fl_Return_Button( 320, 35, 100, 25, "OK" );
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)okindifind_cb);
    }
    o->hotspot( find_ok, 0);
    o->end();
  }
}

void indifindUI::setcontext( void* raisedon ) {
  find_ok->user_data( (void*)this );
  find_fromleft->user_data((void*)this);
  find_fromright->user_data((void*)this);
  find_fuzzy->user_data((void*)this);
}

void* indifindUI::getcontext() const {
  return context;
}

treeinstance* indifindUI::whichtree() {
  printf("Attempting to get treeinstance for indifindUI %ld, via context %ld\n",(long)this,(long)context);
  switch ((strategy>>4)&1) {
    case 0:
      // for People->Find... context was the view
      return ((mainUI*)context)->whichtree();
    case 1:
      // for famUI Find context is the famUI
      return ((famUI*)context)->whichis();
    default:
      return NULL;
  }
}

void indifindUI::open( void* raisedon, short suggestedx, short suggestedy, short callerstrategy ) {
  strategy = callerstrategy;
  context = raisedon;
  find_input->value("");
  find_id->value("");
  setcontext( raisedon );        // this sets the user_data passed to callbacks
  findbox->position(suggestedx, suggestedy);
  findbox->show();
  find_input->take_focus();
}

void indifindUI::searchfind(Fl_Button* button) {
  short x,y;
  x = findbox->x();
  y = findbox->y();
  strategy &= 0xfffc; // make sure no bits left from previous clicks
  if (button==find_fromright) strategy |= 1;
  if (button==find_fromleft)  strategy |= 2;
  if (button==find_fuzzy)     strategy |= 3;
  // indicompletionsbox is a global
  indicompletionsbox->open( this->whichtree(),
                        (char*)find_input->value(),
                        strategy,
                        (Fl_Callback*) chosenindifind_cb,
                        //(void*) searchfrom->getview(),
                        x+65, y+55 );
}

void indifindUI::setperson( GEDCOM_object* person ) {
  if (person!=NULL) {
    GEDCOM_object* name = person->subobject(NAME_tag);
    if (name!=NULL) find_input->value( name->value() );
    find_id->value( person->getidname() );
  }
}

short indifindUI::getstrategy() {
  return strategy;
}

void indifindUI::OK() {
  printf("indifindUI::OK()\n");
  printf("So the indi we want to lookup is %s\n",(char *)indigotobox->find_input->value());
  printf("and we are trying to look it up in this treeinstace %ld\n",(long)this->whichtree());
  GEDCOM_object* newperson = this->whichtree()->Lookup_INDI( (char *)indigotobox->find_input->value());
  printf("got a newperson %ld\n",(long)newperson);
  if (newperson != NULL) {
    // make sure we didn't leave a completions window lying around
    indicompletionsbox->finish();
    switch ((strategy>>4)&1) {
      case 0: {
        findbox->hide();
        // for People->Find... context was the view
        ((mainUI*)context)->setcurrent(newperson);
        return;
      }
      case 1: {
        char* gender;
        GEDCOM_object* test = newperson->subobject( SEX_tag );
        if (test!=NULL) gender=test->value(); else gender=strdup("U");
        // for famUI Find context is the famUI
        GEDCOM_object* fam = ((famUI*)context)->whatis();
        switch (strategy&12) {
          case 4:
            if (*gender!='M') return; else {
              GEDCOM_object* husb = new GEDCOM_object(HUSB_tag,newperson->getid());
              fam->add_subobject(husb);
              newperson->add_subobject(new GEDCOM_object(FAMS_tag,fam->getid()));
              findbox->hide();
              break;
            }
          case 8:
            if (*gender!='F') return; else {
              GEDCOM_object* wife = new GEDCOM_object(WIFE_tag,newperson->getid());
              fam->add_subobject(wife);
              newperson->add_subobject(new GEDCOM_object(FAMS_tag,fam->getid()));
              findbox->hide();
              break;
          }
        }
        ((famUI*)context)->refresh_spouse();
        this->whichtree()->redraw();
        return;
      }
      default:
        return;
    }
  }
}

void indifindUI::finish( ) {
  findbox->hide();
}

///////////////////////////////////////////////////////////////////////////////
// class indicompletionsUI controls find completion window - only one instance of
// the window as it is a short-lived transient sort of thing.
// It will search the tree of the window it was raised on, and display matching
// names. When one is selected it will pass the GEDCOM object back to its caller
// so the name chosen can be set *for that window*.

indicompletionsUI::indicompletionsUI() {
// the constructor does *not* set up and fill in a window/browser, just creates
// the class - which means we do nothing explicit here
  completionswin = (Fl_Window*) NULL;
  completions = (Fl_Select_Browser*) NULL;
  first = (indicompletion_item*) NULL;
}

void indicompletionsUI::open( treeinstance* tree,
                          char* partialname,
                          int strategy,
                          Fl_Callback* indicompletionok_cb,
                          //void* raisedon,
                          short suggestedx, short suggestedy ) {
GEDCOM_object* found_indi;
GEDCOM_object* thing;
bool accept_indi;
char* gender;
indicompletion_item* item;
indicompletion_item* last;
int openheight; openheight = 0;
// initial widths for columns - will be incremented as text is found
  widths[0] = 1;
  widths[1] = 1;
  widths[2] = 1;
  widths[3] = 15; // fixed text in this column " - "
  widths[4] = 1;
  widths[5] = 0;
  // we only allow one copy of this window at a time, so if we are called to open
  // when we are already open, we must first close:
  if (completionswin != (Fl_Window*) NULL) this->finish();
  scantree = tree;
  if (scantree==NULL) fl_alert("Blew it - NULL treeinstance to search\n");
  searchfor = partialname;
  searchmethod = strategy;
  { Fl_Window* o = completionswin = new Fl_Window( 400, 500, "X!Family matching names" );
    o->labeltype(FL_NO_LABEL);
    { Fl_Select_Browser* o = completions = new Fl_Select_Browser( 0, 0, 400, 500);
      // fluid says o->type(1); but this isn't very documented. Why ?
      o->labeltype(FL_NO_LABEL);
      // the callback on the Select_Browser is called when user chooses
      o->callback(indicompletionok_cb);
      //o->user_data(raisedon);
      Fl_Group::current()->resizable(o);
    }
    // the callback on the Fl_Window is called if the user closes it
    o->callback((Fl_Callback*)indicompletionbinned_cb);
    o->end();
  }
  found_indi = (GEDCOM_object*) NULL;
  last = first;
  printf("Search strategy %d bits %d\n",strategy,strategy&&3);
  switch (strategy&3) {
    case 1: found_indi = scantree->INDI_fromright( found_indi, searchfor ); break;
    case 2: found_indi = scantree->INDI_fromleft( found_indi, searchfor ); break;
    case 3: found_indi = scantree->INDI_fuzzymatch( found_indi, searchfor ); break;
    default: printf("strategy unset !\n"); found_indi = (GEDCOM_object*) NULL;
  }
  while (found_indi != (GEDCOM_object*) NULL) {
    // test gender against requirements
    accept_indi = false;
    if ((strategy&12)==12) {
      accept_indi = true;
    } else {
      thing = found_indi->subobject( SEX_tag );
      if (thing!=NULL) gender=thing->value(); else gender=strdup("U");
      if ((strategy&4)==4) if (*gender=='M') accept_indi = true;
      if ((strategy&8)==8) if (*gender=='F') accept_indi = true;
    }
    if (accept_indi) {  
      item = new indicompletion_item( found_indi );
      openheight++;
      if (last == (indicompletion_item*) NULL) first=item; else last->setnext( item );
      item->setwidths( widths );
      last=item;
      completions->add(item->display(), (void*)0);
    }
    switch (strategy&3) {
      case 1: found_indi = scantree->INDI_fromright( found_indi, searchfor ); break;
      case 2: found_indi = scantree->INDI_fromleft( found_indi, searchfor ); break;
      case 3: found_indi = scantree->INDI_fuzzymatch( found_indi, searchfor ); break;
    }
  }
  // at this point, if first is still NULL, there were no hits and we should not
  // raise the completions window at all. Equally, if there was only one hit, we
  // should probably simply pass the GEDCOM_object back to the callback as if the
  // completions window were being closed with that selected. But not yet ...
  if (first==NULL) return;
  completionswin->position( suggestedx, suggestedy );
  completions->column_widths( widths );
  // each column has allowed 5 pixels over reported width, and we need to add
  // another chunk for the scroll bar
  openheight *= fl_height();
  openheight += 5;
  if (openheight>500) openheight=500;
  completionswin->size(widths[0]+widths[1]+widths[2]+widths[3]+widths[4]+widths[5]+25,openheight);
  completionswin->show();
}

void indicompletionsUI::finish() {
indicompletion_item* destroyme;
// since open does very little, there is not much to undo. Yet.
  if (completionswin == (Fl_Window*) NULL) return;
  delete completionswin; // does this automatically lose completions ? well, does your destructor do so ?
  completions = (Fl_Select_Browser*) NULL;
  completionswin = (Fl_Window*) NULL;
  while (first != NULL) {
    destroyme = first;
    first = first->next();
    delete destroyme;
  }
}

GEDCOM_object* indicompletionsUI::chosen_indi( int chosen ) {
int count=0;
indicompletion_item* item=first;

  while (item!=NULL) {
    count++;
    if (count==chosen) return item->indi();
    item = item->next();
  }
  return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// this class is for holding items on a list of names (and related info) used
// by the name-completion code

// It knows about internal representations in FLTK Browsers, so it probably
// should not be in object.cxx. In fact, it probably belongs in gui.cxx
//

indicompletion_item::indicompletion_item( GEDCOM_object* found_indi ):
  indiptr (found_indi)
{
  std::string displayline; displayline="";
  char * rawdate; int i;
  GEDCOM_object* subitem;
  GEDCOM_id* identifier;
  char * idname;
  GEDCOM_object* thing; thing = (GEDCOM_object*) NULL;
  char* val;

  if (indiptr == NULL) {
    fl_alert("Bombing out with null pointer passed to indicompletion_item()\n");
    exit(0);
  }
//  printf("Trying to set up new completion_item for %s\n", indiptr->subobject( NAME_tag )->value());
  if ((subitem = indiptr->subobject( TITL_tag ))!=NULL) {
    displayline += "@C0@r@.";
    val = subitem->value();
    width[0] = 5 + (int) fl_width( val );
//    printf("width of %s is %d\n",val,width[0]);
    displayline += val;
  }
  else width[0] = 0;
  displayline += "\t";
  if ((subitem = indiptr->subobject( SEX_tag )) != NULL) {
    val = subitem->value();
    switch (*val) {
      case 'F' : displayline += "@C1@."; break;
      case 'M' : displayline += "@C4@."; break;
      default :  displayline += "@C60@.";
    }
  }
  else displayline += "@C60@.";
  val = indiptr->subobject( NAME_tag )->value();
  width[1] = 5 + (int) fl_width( val );
  displayline += val;
  displayline += "\t";
  rawdate = "?"; thing = (GEDCOM_object*) NULL;
  if ((subitem = indiptr->subobject( BIRT_tag )) != NULL )
    thing = subitem->subobject( DATE_tag );
  if (thing != NULL) {
    displayline += "@C0@r";
    rawdate = thing->value();
  }
  else {
    if ((subitem = indiptr->subobject( CHR_tag )) != NULL )
      thing = subitem->subobject( DATE_tag );
    if (thing != NULL) {
      displayline += "@C136@r";
      rawdate = thing->value();
    }
    else {
      if ((subitem = indiptr->subobject( BAPM_tag )) != NULL )
        thing = subitem->subobject( DATE_tag );
      if (thing != NULL) {
        displayline += "@C115@r";
        rawdate = thing->value();
      }
      else {
        displayline += "@C0@r";
      }
    }
  }
  if ((*rawdate)=='@') {
    i=1;
    while((*(rawdate+i))!='@') i++;
    rawdate += i+2;
  }
  width[2] = 5 + (int) fl_width( rawdate );
  displayline += rawdate;
  displayline += "\t@C0 -\t";
  rawdate=""; thing = (GEDCOM_object*) NULL;
  if ((subitem = indiptr->subobject( DEAT_tag )) != NULL )
    thing = subitem->subobject( DATE_tag );
  if (thing != NULL) {
    displayline += "@C0 ";
    rawdate = thing->value();
  }
  else {
    if ((subitem = indiptr->subobject( CREM_tag )) != NULL )
      thing = subitem->subobject( DATE_tag );
    if (thing != NULL) {
      displayline += "@C1 ";
      rawdate = thing->value();
    }
    else {
      if ((subitem = indiptr->subobject( BURI_tag )) != NULL )
        thing = subitem->subobject( DATE_tag );
      if (thing != NULL) {
        displayline += "@C121 ";
        rawdate = thing->value();
      }
    }
  }
  if ((*rawdate)=='@') {
    i=1;
    while((*(rawdate+i))!='@') i++;
    rawdate += i+2;
  }
  width[4] = 10 + (int) fl_width( rawdate );
  displayline += rawdate;
  displayline += '\t';
  identifier = indiptr->getid();
  if (identifier != NULL) {
    idname = identifier->GEDCOM_idname();
    width[5] = 5 + (int) fl_width( idname );
    displayline += "@C0 ";
    displayline += idname;
  }
  /* else bombsville - should never have an INDI with no ID ! */
  displayptr = new GEDCOM_string( (char*)displayline.c_str() );
  nextptr = (indicompletion_item*) NULL;
}

indicompletion_item::~indicompletion_item() {
  delete displayptr;
}

void indicompletion_item::setwidths( int widths[] ) {
// we can do this 'cos arrays are passed by address, so get returned
  if (widths[0]<width[0]) widths[0] = width[0];
  if (widths[1]<width[1]) widths[1] = width[1];
  if (widths[2]<width[2]) widths[2] = width[2];
  if (widths[4]<width[4]) widths[4] = width[4];
  if (widths[5]<width[5]) widths[5] = width[5];
//  printf("Widths: %d, %d, %d, %d, %d\n", widths[0], widths[1], widths[2], widths[3], widths[4], widths[5]);
}

GEDCOM_object* indicompletion_item::indi() const {
  return indiptr;
}

indicompletion_item* indicompletion_item::next() const {
  return nextptr;
}

void indicompletion_item::setnext( indicompletion_item* newitem ) {
  nextptr = newitem;
}

char* indicompletion_item::display() const {
  return displayptr->string();
}

///////////////////////////////////////////////////////////////////////////////
// class famfindUI controls find family window - only one instance of the window
// as it is a short-lived transient sort of thing. It will search the tree of
// the window it was raised on, and pass the handle of that window to its OK
// callback
// Similarly, if it raises a completions window, the callback from choosing a
// name there will pass the handle of *this* window to *it's* OK callback, so
// we can find out the family that was chosen (or wasn't, if it was Cancel)

famfindUI::famfindUI() {
  { Fl_Window* o = findbox = new Fl_Window(515, 100, "X!Family Search tree for family");
    o->labeltype(FL_NO_LABEL);
    o->user_data((void*)(this));
    find_input = new Fl_Input( 65, 5, 305, 25, "Goto" );
    find_id = new Fl_Output( 375, 5, 50, 25, "");
    { Fl_Button* o = find_fromright = new Fl_Button( 435, 5, 25, 25, "@<");
      o->labeltype(FL_SYMBOL_LABEL);
      o->callback((Fl_Callback*)searchfamfind_cb);
      o->user_data((void*)this);
    }
    { Fl_Button* o = find_fuzzy = new Fl_Button( 460, 5, 25, 25, "@<->");
      o->labeltype(FL_SYMBOL_LABEL);
      o->callback((Fl_Callback*)searchfamfind_cb);
      o->user_data((void*)this);
    }
    { Fl_Button* o = find_fromleft = new Fl_Button( 485, 5, 25, 25, "@>");
      o->labeltype(FL_SYMBOL_LABEL);
      o->callback((Fl_Callback*)searchfamfind_cb);
      o->user_data((void*)this);
    }
    spouse = new Fl_Output( 65, 35, 305, 25, "Spouse" );
    spouse_id = new Fl_Output( 375, 35, 50, 25, "");
    fam_id = new Fl_Output( 435, 35, 50, 25, "");
    { Fl_Button* o = find_new = new Fl_Button(65, 65, 100, 25, "New");
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
    }
    { Fl_Button* o = find_help = new Fl_Button(180, 65, 100, 25, "Help");
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      // FIXME need a callback and code to deal with it
      // 2022-03-04 the plan is to have a single callback routine to deal with all
      // the help buttons, but the userdata should point to the URL leafname for the
      // relevant help page. We don't have one yet, but it should be "famfindUI.html"
      o->callback((Fl_Callback*)help_cb);
      o->user_data((void*)("famfindUI.html"));
    }
    { Fl_Button* o = find_cancel = new Fl_Button(295, 65, 100, 25, "Cancel");
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)binnedfamfind_cb);
    }
    { Fl_Return_Button* o = find_ok = new Fl_Return_Button( 410, 65, 100, 25, "OK" );
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)okfamfind_cb);
      o->user_data((void*)this);
    }
    o->hotspot( find_ok, 0);
    o->end();
  }
}

void famfindUI::setcontext( void* raisedon ) {
  find_ok->user_data( (void*)this );
  find_fromleft->user_data((void*)this);
  find_fromright->user_data((void*)this);
  find_fuzzy->user_data((void*)this);
}

void* famfindUI::getcontext() const {
  return context;
}

treeinstance* famfindUI::whichtree() {
  printf("Attempting to get treeinstance for famfindUI %ld, via context %ld\n",(long)this,(long)context);
  switch ((strategy>>4)&1) {
/*    case 0: there is, as yet, no context comparable to the People menu Find/Goto item
      // for People->Find... context was the view
      return ((mainUI*)context)->whichtree(); */
    case 1:
      // for indiUI Find context is the indiUI
      return ((indiUI*)context)->whichis();
    default:
      return NULL;
  }
}

void famfindUI::open( void* raisedon, short suggestedx, short suggestedy, short callerstrategy ) {
  strategy = callerstrategy;
  context = raisedon;
  chosenfam = NULL;
  find_input->value("");
  find_id->value("");
  setcontext( raisedon );        // this sets the user_data passed to callbacks
  findbox->position(suggestedx, suggestedy);
  findbox->show();
  find_input->take_focus();
}

void famfindUI::searchfind(Fl_Button* button) {
  short x,y;
  x = findbox->x();
  y = findbox->y();
  strategy &= 0xfffc; // make sure no bits left from previous clicks
  if (button==find_fromright) strategy |= 1;
  if (button==find_fromleft)  strategy |= 2;
  if (button==find_fuzzy)     strategy |= 3;
  // famcompletionsbox is a global
  famcompletionsbox->open( this->whichtree(),
                        (char*)find_input->value(),
                        strategy,
                        (Fl_Callback*) chosenfamfind_cb,
                        //(void*) searchfrom->getview(),
                        x+65, y+55 );
}

void famfindUI::setfam( famcompletion_item* match ) {
// need to extract names and IDs of both spouses, but we need some way to decide which one first
// must fill in find_input
//              find_id
//              spouse
//              spouse_id
//              fam_id->value( fam->getidname() );
  if (match!=NULL) {
    chosenfam = match->fam();
    printf("famfindUI::setfam attempting to complete the gotobox for id %s\n",chosenfam->getidname());
    fam_id->value( chosenfam->getidname() );
    char* gender = match->gender();
    GEDCOM_object* spouseF;
    GEDCOM_object* spouseM;
    spouseF = chosenfam->thewife();
    spouseM = chosenfam->thehusband();
    // we are going to assume, since we've got here as the result of name matching, that the first
    // spouse will definitely have a value for NAME. This is a less clearcut assumption for the
    // second spouse, but we're not going to code defensively yet... Just remember that there's
    // potential for a null pointer lurking in here.
    switch (*gender) {
      case 'F' :
         find_input->value(spouseF->subobject(NAME_tag)->value());
         find_id->value(spouseF->getidname() );
         if (spouseM!=NULL) {
           spouse->value(spouseM->subobject(NAME_tag)->value());
           spouse_id->value(spouseM->getidname() );
         } else {
           spouse->value("");
           spouse_id->value("");
         }
         break;
      case 'M' :
         find_input->value(spouseM->subobject(NAME_tag)->value());
         find_id->value(spouseM->getidname() );
         if (spouseF!=NULL) {
           spouse->value(spouseF->subobject(NAME_tag)->value());
           spouse_id->value(spouseF->getidname() );
         } else {
           spouse->value("");
           spouse_id->value("");
         }
         break;
      default :  fl_alert("Oh shit\n"); // think of a more helpful message, please, and some idea of what to do...
    }
  } else {
    fl_alert("famfindUI::setfam called with NULL match\n");
  }
}

short famfindUI::getstrategy() {
  return strategy;
}

void famfindUI::OK() {
  printf("famfindUI::OK()\n");
  printf("So the family we want to lookup is %s\n",(char *)famgotobox->find_input->value());
  printf("and we are trying to look it up in this treeinstance %ld\n",(long)this->whichtree());
/* The completions item got destroyed when we closed the window, but we have the info we need in the
   findUI dbox including chosenfam which is a pointer to the FAM object. I am unclear why the
   chosenfam needs to be referenced through famgotobox and not this->
*/
  if (famgotobox->chosenfam != NULL) {
    // make sure we didn't leave a completions window lying around
    famcompletionsbox->finish();
    // so at this point we need to add a FAMC to the indi from the indiUI context, and
    // add him or her as a CHIL to the FAM
    GEDCOM_object* indi = ((indiUI*)context)->whois();
    indi->add_subobject(new GEDCOM_object(FAMC_tag,famgotobox->chosenfam->getid()));
    famgotobox->chosenfam->add_subobject(new GEDCOM_object(CHIL_tag,indi->getid()));
   findbox->hide();
   return;
  }
  // if chosenfam has not been set, OK should do nothing, unless you choose to whinge at user...
  fl_alert("Bastards ! chosenfam seems to be NULL\n");
}

void famfindUI::finish( ) {
  findbox->hide();
}

///////////////////////////////////////////////////////////////////////////////
// class famcompletionsUI controls family completion window - only one instance of
// the window as it is a short-lived transient sort of thing.
// It will search the tree of the window it was raised on, and display matching
// names. When one is selected it will pass the GEDCOM object back to its caller
// so the name chosen can be set *for that window*.

famcompletionsUI::famcompletionsUI() {
// the constructor does *not* set up and fill in a window/browser, just creates
// the class - which means we do nothing explicit here
  completionswin = (Fl_Window*) NULL;
  completions = (Fl_Select_Browser*) NULL;
  first = (famcompletion_item*) NULL;
}

void famcompletionsUI::open( treeinstance* tree,
                          char* partialname,
                          int strategy,
                          Fl_Callback* famcompletionok_cb,
                          //void* raisedon,
                          short suggestedx, short suggestedy ) {
GEDCOM_object* found_indi;
GEDCOM_object* fams;
GEDCOM_object* thing;
bool accept_indi;
char* gender;
famcompletion_item* item;
famcompletion_item* last;
int openheight; openheight = 0;
// initial widths for columns - will be incremented as text is found, there are no fixed width columns in the fam version
  widths[0] = 1;
  widths[1] = 1;
  widths[2] = 1;
  widths[3] = 1;
  // we only allow one copy of this window at a time, so if we are called to open
  // when we are already open, we must first close:
  if (completionswin != (Fl_Window*) NULL) this->finish();
  scantree = tree;
  if (scantree==NULL) fl_alert("Blew it - NULL treeinstance to search\n"); //should actually do something about that - continuing just crashes
  searchfor = partialname;
  searchmethod = strategy;
  { Fl_Window* o = completionswin = new Fl_Window( 400, 500, "X!Family matching families" );
    o->labeltype(FL_NO_LABEL);
    { Fl_Select_Browser* o = completions = new Fl_Select_Browser( 0, 0, 400, 500);
      // fluid says o->type(1); but this isn't very documented. Why ?
      o->labeltype(FL_NO_LABEL);
      // the callback on the Select_Browser is called when user chooses
      o->callback(famcompletionok_cb);
      //o->user_data(raisedon);
      Fl_Group::current()->resizable(o);
    }
    // the callback on the Fl_Window is called if the user closes it
    o->callback((Fl_Callback*)famcompletionbinned_cb);
    o->end();
  }
  found_indi = (GEDCOM_object*) NULL;
  last = first;
  printf("Search strategy %d bits %d\n",strategy,strategy&&3);
  switch (strategy&3) {
    case 1: found_indi = scantree->INDI_fromright( found_indi, searchfor ); break;
    case 2: found_indi = scantree->INDI_fromleft( found_indi, searchfor ); break;
    case 3: found_indi = scantree->INDI_fuzzymatch( found_indi, searchfor ); break;
    default: printf("strategy unset !\n"); found_indi = (GEDCOM_object*) NULL;
  }
  while (found_indi != (GEDCOM_object*) NULL) {
    // just because we have a match doesn't mean we have a marriage that fits, we need to look
    // at every FAMS object for this spouse and add it to the list - there may be none, of course.
    thing = found_indi->subobject( SEX_tag );
    if (thing==NULL) break; // no point in looking for FAMS for an indi whose gender we don't know
    gender = thing->value();
    printf("famcompletionsbox->open matched %s %s, gender %s\n",found_indi->getidname(),found_indi->subobject(NAME_tag)->value(),gender);
    fams = found_indi->subobject(FAMS_tag);
    while (fams!=NULL) {
      item = new famcompletion_item( fams->followxref(), gender );
      openheight++;
      if (last == (famcompletion_item*) NULL) first=item; else last->setnext( item );
      item->setwidths( widths );
      last=item;
      completions->add(item->display(), (void*)0);
      fams = fams->next_object(FAMS_tag);
    }
    switch (strategy&3) {
      case 1: found_indi = scantree->INDI_fromright( found_indi, searchfor ); break;
      case 2: found_indi = scantree->INDI_fromleft( found_indi, searchfor ); break;
      case 3: found_indi = scantree->INDI_fuzzymatch( found_indi, searchfor ); break;
    }
  }
  // at this point, if first is still NULL, there were no hits and we should not
  // raise the completions window at all. But we already created the FL_Window, so need to destroy that
  // which possibly means calling ->finish()
  // Equally, if there was only one hit, we
  // should probably simply pass the GEDCOM_object back to the callback as if the
  // completions window were being closed with that selected. But not yet ...
  if (first==NULL) return;
  completionswin->position( suggestedx, suggestedy );
  completions->column_widths( widths );
  // each column has allowed 5 pixels over reported width, and we need to add
  // another chunk for the scroll bar
  openheight *= fl_height();
  openheight += 5;
  if (openheight>500) openheight=500;
  completionswin->size(widths[0]+widths[1]+widths[2]+widths[3]+widths[4]+widths[5]+25,openheight);
  completionswin->show();
}

void famcompletionsUI::finish() {
famcompletion_item* destroyme;
// since open does very little, there is not much to undo. Yet.
  if (completionswin == (Fl_Window*) NULL) return;
  delete completionswin; // does this automatically lose completions ?
  completions = (Fl_Select_Browser*) NULL;
  completionswin = (Fl_Window*) NULL;
  while (first != NULL) {
    destroyme = first;
    first = first->next();
    delete destroyme;
  }
}

famcompletion_item* famcompletionsUI::chosen_fam( int chosen ) {
int count=0;
famcompletion_item* item=first;

  while (item!=NULL) {
    count++;
    // item->fam() is the GEDCOM_object* pointing to the FAM, I believe, but we really
    // want the caller to get the display() too, to fill into the gotobox
    // but beware if that gets filled in, hitting the search button(s) again will be useless
    // our idiom here is a little poorly defined. Probably what we want is to have two
    // fields in the search box, one writable by the user and one not. The search value goes in
    // the first one, and when the completions window hits OK, we copy one spuse (the one
    // matched, 'cos that's how we'll have them) into that row, and the other spouse into
    // the one below, as well as the F<nnn> ID into a box for that. The user could then
    // change his mind and retype something into the top row, and hitting a search button
    // again would clear the other boxes and do the search. Alternatively, we cold make
    // it so you can only do search once, then the buttons are greyed, but have a "New"
    // button alongside "Help", "Cancel" and "OK" which would clear the fields (and, if
    // still open, close the completions window) then ungrey the search buttons.
    if (count==chosen) return item;
    item = item->next();
  }
  return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// this class is for holding items on a list of names (and related info) used
// by the name-completion code

// It knows about internal representations in FLTK Browsers, so it probably
// should not be in object.cxx. In fact, it probably belongs in gui.cxx
//

famcompletion_item::famcompletion_item( GEDCOM_object* found_fam, char* gender ):
  famptr (found_fam)
{
  std::string displayline; displayline="";
  char * rawdate; int i;
  GEDCOM_object* subitem;
  GEDCOM_id* identifier;
  char * idname;
  GEDCOM_object* thing; thing = (GEDCOM_object*) NULL;
  char* val;
  sex = gender;

  if (famptr == NULL) {
    fl_alert("Bombing out with null pointer passed to famcompletion_item()\n");
    exit(0);
  }
  switch (*gender) {
    case 'F' : displayline += "@C1@.";
      subitem = found_fam->thewife();
      break;
    case 'M' : displayline += "@C4@.";
      subitem = found_fam->thehusband();
      break;
  }
  val = subitem->subobject(NAME_tag)->value();
  printf("New famcompletion_item for spouse %s %s\n",subitem->getidname(),val);
  width[0] = 5 + (int) fl_width( val );
  displayline += val;
  displayline += "\t";
  subitem = found_fam->subobject(MARR_tag);
  if (subitem!=NULL) {
    val = subitem->subobject(DATE_tag)->value();
    displayline += val;
  }
  displayline += "\t";
  width[1] = 5 + (int) fl_width( val );
  switch (*gender) {
    case 'M' : displayline += "@C1@.";
      subitem = found_fam->thewife();
      break;
    case 'F' : displayline += "@C4@.";
      subitem = found_fam->thehusband();
      break;
  }
  if (subitem==NULL) {
    // yes, we can match a family with only one known spouse...
    printf("New famcompletion_item with no second spouse\n");
    width[2] = 0;
  } else {
    val = subitem->subobject(NAME_tag)->value();
    printf("New famcompletion_item for other spouse %s %s\n",subitem->getidname(),val);
    width[2] = 5 + (int) fl_width( val );
    displayline += val;
  }
  displayline += "\t";
  identifier = famptr->getid();
  if (identifier != NULL) {
    idname = identifier->GEDCOM_idname();
    width[3] = 5 + (int) fl_width( idname );
    displayline += "@C0 ";
    displayline += idname;
  }

  /* else bombsville - should never have a FAM with no ID ! */
  displayptr = new GEDCOM_string( (char*)displayline.c_str() );
  nextptr = (famcompletion_item*) NULL;
}

famcompletion_item::~famcompletion_item() {
  delete displayptr;
}

void famcompletion_item::setwidths( int widths[] ) {
// we can do this 'cos arrays are passed by address, so get returned
  if (widths[0]<width[0]) widths[0] = width[0];
  if (widths[1]<width[1]) widths[1] = width[1];
  if (widths[2]<width[2]) widths[2] = width[2];
  if (widths[3]<width[3]) widths[3] = width[3];
  printf("famcompletion_item Widths: %d, %d, %d, %d\n", widths[0], widths[1], widths[2], widths[3]);
}

GEDCOM_object* famcompletion_item::fam() const {
  printf("famcompletion_item::fam() returning %ld\n",(long)famptr);
  return famptr;
}

famcompletion_item* famcompletion_item::next() const {
  return nextptr;
}

void famcompletion_item::setnext( famcompletion_item* newitem ) {
  nextptr = newitem;
}

char* famcompletion_item::display() const {
  return displayptr->string();
}

char* famcompletion_item::gender() const {
  return sex;
}

///////////////////////////////////////////////////////////////////////////////
// class prefUI controls user-preferences window - global to program so only
// one instance.

// hmmm. if we want (eventually) to be able to set things like "show descendants"
// and "show ancestors" on a per-window basis, that will need to change. Some
// settings may be global, but others are going to be per-tree or per-view

prefUI::prefUI() {
// don't seem to use:   Fl_Window* w;
  { Fl_Window* o = optsbox = new Fl_Window(460, 315, "X!Family preferences");
// don't seem to use:     w = o;
    o->color(50);
    o->labeltype(FL_NORMAL_LABEL);
    o->user_data((void*)(this));
    { Fl_Tabs* o = choices = new Fl_Tabs(0, 20, 460, 240);
      o->labeltype(FL_NO_LABEL);
      { Fl_Tile* o = nameprefs = new Fl_Tile(0, 50, 460, 210, "Names");
        o->color(246);
        o->selection_color(246);
        o->hide();
        { Fl_Group* o = new Fl_Group(15, 75, 255, 75);
          o->box(FL_EMBOSSED_BOX);
          o->labeltype(FL_NO_LABEL);
          { Fl_Check_Button* o = showfam = new Fl_Check_Button(25, 85, 235, 25, "Always show family names");
            o->box(FL_UP_BOX);
            o->down_box(FL_DOWN_BOX);
          }
          { Fl_Check_Button* o = showtitle = new Fl_Check_Button(25, 115, 235, 25, "Show titles");
            o->box(FL_UP_BOX);
            o->down_box(FL_DOWN_BOX);
          }
          o->end();
        }
        o->end();
      }
      { Fl_Tile* o = dateprefs = new Fl_Tile(0, 50, 460, 210, "Displayed dates");
        o->color(207);
        o->selection_color(207);
        { Fl_Group* o = new Fl_Group(5, 75, 450, 135);
          o->box(FL_EMBOSSED_BOX);
          o->labeltype(FL_NO_LABEL);
          { Fl_Check_Button* o = treedates = new Fl_Check_Button(15, 85, 200, 25, "Show dates on tree");
            o->box(FL_UP_BOX);
            o->down_box(FL_DOWN_BOX);
          }
          { Fl_Box* o = new Fl_Box(15, 115, 200, 25, "including ...");
            o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
          }
          { Fl_Check_Button* o = spousedates = new Fl_Check_Button(15, 145, 200, 25, "Show spouse's dates");
            o->box(FL_UP_BOX);
            o->down_box(FL_DOWN_BOX);
          }
          { Fl_Check_Button* o = marrdates = new Fl_Check_Button(15, 175, 200, 25, "Show wedding dates");
            o->box(FL_UP_BOX);
            o->down_box(FL_DOWN_BOX);
          }
          { Fl_Check_Button* o = yearonly = new Fl_Check_Button(240, 85, 200, 25, "Show just year on tree");
            o->box(FL_UP_BOX);
            o->down_box(FL_DOWN_BOX);
          }
          { Fl_Box* o = new Fl_Box(240, 115, 200, 25, "Date formats ...");
            o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
          }
          { Fl_Group* o = new Fl_Group(235, 145, 210, 55);
            { Fl_Light_Button* o = prefgedcom = new Fl_Light_Button(240, 145, 200, 25, "GEDCOM dates");
              o->type(102);
            }
            { Fl_Light_Button* o = prefios = new Fl_Light_Button(240, 175, 200, 25, "ISO-8601 dates");
              o->type(102);
            }
            o->end();
          }
          o->end();
        }
        o->end();
      }
      { Fl_Tile* o = miscopts = new Fl_Tile(0, 65, 460, 195, "Miscellaneous");
        o->color(175);
        o->selection_color(175);
        o->hide();
        { Fl_Group* o = xref = new Fl_Group(5, 70, 185, 175);
          o->box(FL_EMBOSSED_BOX);
          o->labeltype(FL_NO_LABEL);
          { Fl_Box* o = xref1 = new Fl_Box(20, 85, 105, 25, "Xref errors:");
            o->labelfont(1);
            o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
          }
          { Fl_Light_Button* o = xrefquit = new Fl_Light_Button(25, 120, 150, 25, "Abandon load");
            o->type(102);
          }
          { Fl_Light_Button* o = xrefwarn = new Fl_Light_Button(25, 160, 150, 25, "Warnings");
            o->type(102);
          }
          { Fl_Light_Button* o = xrefignore = new Fl_Light_Button(25, 200, 150, 25, "Ignore silently");
            o->type(102);
          }
          o->end();
        }
        { Fl_Group* o = datestamp = new Fl_Group(195, 70, 260, 175);
          o->box(FL_EMBOSSED_BOX);
          o->labeltype(FL_NO_LABEL);
          { Fl_Box* o = datestamp1 = new Fl_Box(210, 85, 235, 25, "Report datestamp format:");
            o->labelfont(1);
            o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
          }
          { Fl_Light_Button* o = stampiso = new Fl_Light_Button(215, 120, 100, 25, "ISO-8601");
            o->type(102);
          }
          { Fl_Light_Button* o = stampgedcom = new Fl_Light_Button(335, 120, 100, 25, "GEDCOM");
            o->type(102);
          }
          { Fl_Light_Button* o = stamplocal = new Fl_Light_Button(215, 160, 100, 25, "As locale");
            o->type(102);
          }
          { Fl_Light_Button* o = stampother = new Fl_Light_Button(335, 160, 100, 25, "Other:");
            o->type(102);
          }
          { Fl_Input* o = stampformat = new Fl_Input(215, 200, 220, 30);
            o->labeltype(FL_NO_LABEL);
          }
          o->end();
        }
        o->end();
      }
      o->end();
    }
    { Fl_Button* o = cancelpref = new Fl_Button(16, 275, 96, 25, "Cancel");
      o->labelfont(1);
      o->callback((Fl_Callback*)cancelchoices_cb);
    }
    { Fl_Button* o = applypref = new Fl_Button(128, 275, 96, 25, "Apply");
      o->labelfont(1);
      o->callback((Fl_Callback*)applychoices_cb);
    }
    { Fl_Return_Button* o = okpref = new Fl_Return_Button(240, 275, 96, 25, "Save");
      o->selection_color(33);
      o->labelfont(1);
      o->callback((Fl_Callback*)okchoices_cb);
    }
    { Fl_Button* o = applypref = new Fl_Button(352, 275, 96, 25, "Help");
      o->labelfont(1);
      o->callback((Fl_Callback*)help_cb);
      o->user_data((void*)("prefUI.html"));
    }
    o->end();
  }
}

////////////////////////////////////////////////////////////////////////////////
//
// class notesUI

notesUI::notesUI( treeinstance* whichtree, GEDCOM_object* newobject, GEDCOM_tag* newedittag ):
  edittree (whichtree),
  editobject (newobject),
  edittag (newedittag),
  modified (false)
{

// window to edit the NOTES+CONC|CONT or TEXT+CONC|CONT of potentially
// *any* GEDCOM object, although most often this will a NOTE for an INDI
// (in !Family it would always be an INDI, but here we are going to be as
// general as possible, so want at least INDI, FAM and SOUR)
// beware that a SOUR may have both a NOTE and a TEXT and that the TEXT
// may itself have a NOTE subobject. Thus there are potentially at least
// three notesUI windows to open for a single SOUR record :-(

// edittag could thus be NOTE_tag or TEXT_tag
// editobject is the INDI, FAM or event object on which we are raised

  window = new Fl_Window(720, 305, notes_title);
  // with this font, that is width for 80 characters, which are
  // 15 pixels high.
  ident = new Fl_Output(100,3,600,24,"Text for:");
   //printf("In notesUI constructor, having created a window\n");
   this->retitle();

   // do we need a menu ? or buttons "Cancel" and "save to GEDCOM" ?
   // we might need a menu if we are going to implement things like
   // "save notes to a file", "load notes from a file", "append file
   // to notes" ... although equally, we should be able to do that
   // with some sort of drag and drop ....
   // lets start with just the buttons on a menu bar:
   menu = new notesmenu( (this) );
   menubar = new Fl_Menu_Bar( 0, 30, 720, 25);
   menubar->menu((Fl_Menu_Item*)menu);

   input = new Fl_Text_Buffer(0,1024);
   scroll = new Fl_Text_Editor(0,55,720,250);
   scroll->buffer(input);
   scroll->type(Fl_Scroll::BOTH_ALWAYS);
   scroll->callback((Fl_Callback*)changenotes_cb);
   scroll->user_data((void*)(this));
   scroll->when(0);
   scroll->textfont(FL_SCREEN_BOLD);

  window->end();
  window->user_data((void*)(this));
  window->callback((Fl_Callback*)quitnotes_cb);
   window->resizable(scroll);
  // this->restore expects that there is a NOTE (or TEXT) object from which it
  // can extract the contents of the editor box. Whilst restore() can cope with
  // a NULL object when there is no existing NOTE, doing so risks being orphaned
  // if we are raised on an event object and the dbox on the owning INDI or FAM
  // object is closed with the event object empty, because it will be garbage
  // collected. So we need to ensure that there is a NOTE or TEXT object hanging
  // from that event throughout the time that the notesUI is open:
  //printf("NotesUI widgets complete\n");
  textobject = editobject->subobject( edittag );
  if (textobject == NULL) {
#ifdef debugging
    printf("creating new NOTE object\n");
#endif
    textobject = new GEDCOM_object( edittag );
#ifdef debugging
    printf("Created ok at %ld, as subobject of %ld\n",(long)textobject,(long)editobject);
#endif
    editobject->add_subobject( textobject );
  }
#ifdef debugging
  else printf("Found extant NOTE object at %ld\n",(long)textobject);
#endif
  this->status(true);
  // OK, that (possibly empty) object will continue to exist throughout the life
  // of the notesUI, and can be used to restore the original text contents of
  // the window. When the window is closed, the text will be put into the object
  // (and CONC or CONT subobjects) replacing any previous contents, or, if there
  // is no text, textobject will be destroyed.
  // if textobject is garbage collected, we should call code to check whether its
  // parent is now an empty object, and recurse upwards, freeing objects as needed
  // but beware - if any objects are marked ephemeral, then the UI for which they
  // were created probably still exists and we should *not* garbage collect.
  window->size(720,80+(this->restore())*15);
// we had the equivalent of this before fix0007 - do we need it, and if so, how ?
// this says when to call the callback on the Text_Buffer* input which is "whenever it is changed"
// however, in FLTK 1.3 (which is what we think we are now building against) this stuff seems to
// have all changed...
  scroll->when(FL_WHEN_CHANGED);
}

void notesUI::status( bool extant) {
// find if there is an indiUI or famUI relating to these notes, and call
// its checknotes method to update the "notes" Light_Button(s)
  GEDCOM_object *mainobject;
  GEDCOM_tag *objecttag = editobject->objtype();
  mainobject = editobject;
  while ((objecttag!=INDI_tag)&&(objecttag!=FAM_tag)) {
    mainobject = mainobject->parent();
    objecttag = mainobject->objtype();
  }
  if (objecttag==INDI_tag) {
    indiUI *whototell;
    if ((whototell=(editUIs->editbox( mainobject )))!=NULL) {
      whototell->checknotes(editobject, extant);
    }
  } else {
    if(objecttag==FAM_tag) {
      famUI *whototell;
      if ((whototell=(famUIs->fambox( mainobject )))!=NULL) {
        whototell->checknotes(editobject, extant);
      }
    }
    // and more code as above once you have NOTEs or TEXT for SOURces, WILLs, PROBate ...
  }
}

notesUI::~notesUI() {
  // textobject may point to a GEDCOM_object we created ephemerally to
  // ensure the continued existence of editobject if the dbox on that
  // were to be closed. But if textobject is empty, we shouldn't save
  // it into the GEDCOM.
#ifdef destructorlogs
  printf("~notesUI %ld, editobject %ld, textobject %ld, text in textobject %s, first subobject at %ld\n",(long)this,(long)editobject,(long)textobject,textobject->value(),(long)textobject->subobject());
#endif
  if (textobject!=NULL) {
    if ( ((textobject->value())==NULL) && ((textobject->subobject())==NULL) ) {
      editobject->delete_subobject(textobject);
    }
  } else {
    printf("Yeay ! We would have barfed at this point as textobject was NULL so textobject->value()==segfault\n");
  }
  this->status(false);
  // if the textobject continues to exist, we needn't worry that our pointer
  // to it is being destroyed here - the parent GEDCOM object still points to
  // it, as one of its subobjects...
}

int notesUI::restore() {
  // concatenate all subobjects of type edittag, and any of their
  // subobjects of type CONT_tag or CONC_tag. the value() of a
  // CONT object should be inserted into the buffer *after* a '\n'

  GEDCOM_object *note, *sub, *conc, *debug;
  int len, pos = 0, lines = 0;
  char *string;

  // input is our editor buffer, start off with it empty:
  input->text("");
  note = textobject; //  = editobject->subobject( edittag );
#ifdef debugging
  printf("notesUI::restore passed %ld\n",(long)note);
#endif
  while (note != NULL ) {
#ifdef debugging
    printf("parsing a non-null NOTE %ld\n",(long)note);
#endif
    string = note->value();
    if (string!=NULL) {
#ifdef debugging
      printf("value of NOTE object was %s\n",string);
#endif
      len=strlen(string);
      input->replace(pos,pos,string);
      pos += len;
    }
    sub = note->subobject();
    if (sub==note) { printf("object at %p is subobject of itself\n",note); return 0; }
    if (sub != NULL) {
#ifdef debugging
      printf("non-null subobject\n");
#endif
      while ((sub->objtype())==CONC_tag) {
         string = sub->value();
         if (string!=NULL) { // (which it is allowed to be - a blank line)
           input->replace(pos,pos,string);
           len=strlen(string);
           pos += len;
         }
         sub = sub->next_object();
         if (sub==NULL) break;
      } // end of CONC lines, no CONT to follow if sub is NULL
    }
    while (sub != NULL) {
      if ((sub->objtype())==CONT_tag) {
#ifdef debugging
        printf("Concatenating a CONT into buffer\n");
#endif
        input->replace(pos,pos,"\n"); lines++;
        pos++;
        string = sub->value();
        if (string!=NULL) { // (which it is allowed to be - a blank line)
          input->replace(pos,pos,string);
          len=strlen(string);
          pos += len;
        }
        conc = sub->subobject();
        while (conc!=NULL) {
#ifdef debugging
           printf("Concatenating a CONC into buffer\n");
#endif
           if ((conc->objtype())==CONC_tag) {
             string = conc->value();
             if (string!=NULL) { // (which it is allowed to be - a blank line)
               input->replace(pos,pos,string);
               len=strlen(string);
               pos += len;
             }
          }
          conc = conc->next_object();
        } // end of CONCs of this CONT
      } // end of CONT object
      sub = sub->next_object(); // get another CONT if there is one
    } // no more subobjects
    input->replace(pos,pos,"\n"); lines++;
    debug = note;
    note = note->next_object( edittag );
    if (note==debug) { printf("object at %p is its own next_object\n",note); break; }
  } // loop concatenates all edittag objects into one
  size = pos;
#ifdef debugging
  printf("NOTE object has text of %d chars, %d lines\n", size, lines);
#endif
  return lines;
}

void notesUI::update() {
// update the GEDCOM from the edited text
  if (!modified) return; // save a load of effort here kiddies !
#ifdef debugging
  printf("saving modified notes back into GEDCOM\n");
#endif
// bin the old notes object(s)
  GEDCOM_object *oldnotes;
  oldnotes = editobject->subobject( edittag );
  if (oldnotes != textobject) printf("notesUI::update proceeding under a misapprehension\n");
  while (oldnotes!=NULL) {
#ifdef debugging
    printf("Deleting %ld (tag %s) as a subobject of %ld (tag %s)\n",(long)oldnotes,oldnotes->tagname(),(long)editobject,editobject->tagname());
#endif
    editobject->delete_subobject( oldnotes );
#ifdef debugging
    printf("Deleted OK, checking for additional %s objects\n",edittag->GEDCOM_namefromtag());
#endif
    oldnotes = editobject->subobject( edittag );
  }
  size = input->length(); // this isn't updated automatically ...
  if (size==0) { // we must still ensure that we retain an empty NOTE/TEXT
#ifdef debugging
    printf("now creating replacement %s\n",edittag->GEDCOM_namefromtag());
#endif
    textobject = new GEDCOM_object( edittag );
    editobject->add_subobject(textobject);
#ifdef debugging
    printf("new %s at %ld as subobject of %ld\n",edittag->GEDCOM_namefromtag(),(long)textobject,(long)editobject);
#endif
    return; // nothing else to do if all text now gone
  }
#ifdef debugging
  printf("now creating replacement %s of finite size %d\n",edittag->GEDCOM_namefromtag(), size);
#endif
  char *line, *ptr, *end;
// don't seem to use: GEDCOM_object *noteobject;
  line = (char*) input->text(); // discard const-ness
  end = line + size;
  ptr = line; while ((*ptr!='\n')&&(ptr<end)) ptr++;
  *ptr = '\0';
  textobject = new GEDCOM_object( edittag, line );
  *ptr = '\n'; // we are fiddling with the real buffer here, so restore that
  ptr++;
  while (ptr<end) {
    line = ptr;
    while ((*ptr!='\n')&&(ptr<end)) ptr++;
    *ptr = '\0';
    textobject -> add_subobject( new GEDCOM_object( CONT_tag, line ));
    *ptr = '\n'; // we are fiddling with the real buffer here, so restore that
    ptr++;
  }
  if (editobject==NULL) {
    printf("notesUI::update trying to add textobject %ld to a null editobject\n",(long)textobject);
  } else {
    editobject -> add_subobject( textobject );
  }
}

notesUI* notesUI::getnext() const {
  return nextui;
}

notesUI* notesUI::getprevious() const {
  return previousui;
}

void notesUI::setnext( notesUI* newnotes ) {
  nextui = newnotes;
}

void notesUI::setprevious( notesUI* newnotes ) {
  previousui = newnotes;
}

GEDCOM_object* notesUI::object() const {
  return editobject;
}

GEDCOM_tag* notesUI::tag() const {
  return edittag;
}

void notesUI::changed(bool whether) {
  modified = whether;
}

bool notesUI::changed() const {
  return modified;
}

void notesUI::clear() {
//  input->replace(0, size, "", 0); would be fine if size was maintained up to date
  input->text("");
  modified = true;
}

void notesUI::retitle() {
GEDCOM_tag* objecttag = editobject->objtype();
GEDCOM_object* mainobject;
char *tempstring;
std::string title("X!Family");
// used to be called after doing a language change (all code for which has now been removed)
// So now it's only when the notes window is first created

  title += " ";
  title += edittree->getfilename();
  // now put the title where the window expects it and it won't evaporate as title goes out of scope:
  strncpy(notes_title, title.c_str(), MAX_notestitle);

  title  = objecttag->GEDCOM_namefromtag();
  title += " ";
  // this bit needs fixing to actually get an idname by looking at parent object
  // until it finds an INDI or FAM tag.
  mainobject = editobject;
  while ((objecttag!=INDI_tag)&&(objecttag!=FAM_tag)) {
    mainobject = mainobject->parent();
    objecttag = mainobject->objtype();
  }
  if ((tempstring=mainobject->getidname())!=NULL) {
    title += tempstring;
    title += " ";
  }
  // and this bit needs fixing to look up through parents until it gets an INDI
  // tag, in which case extracting a name is easy, or a FAM tag, in which case
  // we need to make one up - probably from a year and two surnames ?
  if (objecttag==INDI_tag) {
    GEDCOM_object* nameobj;
    if ((nameobj=(mainobject->subobject(NAME_tag)))==NULL)
      title += "<unnamed>";
    else
      title += nameobj->value();
  }
  if (objecttag==FAM_tag) {
    GEDCOM_object *thing, *indi, *nameobj;
    thing = mainobject->subobject( HUSB_tag );
    if (thing!=NULL) {
      indi = thing->followxref();
      if (indi!=NULL) {
        if ((nameobj=(indi->subobject(NAME_tag)))!=NULL) title+= nameobj->value();
      }
    }
    title += " + ";
    thing = mainobject->subobject( WIFE_tag );
    if (thing!=NULL) {
      indi = thing->followxref();
      if (indi!=NULL) {
        if ((nameobj=(indi->subobject(NAME_tag)))!=NULL) title+= nameobj->value();
      }
    }
  }
  title += " (";
  title += edittag->GEDCOM_namefromtag();
  // potentially there is a usable string associated with a SOUR object,
  // but it might appear under TITL, or various other things, including
  // the SOUR tag itself.
  title += ") ";
  if (modified) title += "*";
  strncpy(notes_id, title.c_str(), MAX_notestitle);
  ident->value( notes_id );

  if (window->shown()) {
    window->hide();
    window->show();
  }
}

void notesUI::hide() {
  window->hide();
}

void notesUI::show() {
  window->show();
}


