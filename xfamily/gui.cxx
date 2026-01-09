// gui.cxx

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

#include "classes.h"
#include "languages.h"
#include "gui.h"
#include "structure.h"
#include "trees.h"
#include "callback.h"
#include "display.h"
#include "objects.h"
#include "strings.h"
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
  menu_data[0] = (Fl_Menu_Item){msg_menu_file,     0, 0, 0,  64, 0, 0, 14, 0 };
  menu_data[1] = (Fl_Menu_Item){msg_menu_load,     0, 0, 0,   0, 0, 0, 14, 0 };
  menu_data[2] = (Fl_Menu_Item){msg_menu_save,     0, (Fl_Callback*)save_cb, (void*)(view),   0, 0, 0, 14, 0 };
  menu_data[3] = (Fl_Menu_Item){msg_menu_saveas,   0, 0, 0,   0, 0, 0, 14, 0 };
  menu_data[4] = (Fl_Menu_Item){msg_menu_print,    0, 0, 0, 128, 0, 0, 14, 0 };
  menu_data[5] = (Fl_Menu_Item){msg_menu_newview,  0, (Fl_Callback*)newview_cb, (void*)(view),   0, 0, 0, 14, 0 };
  menu_data[6] = (Fl_Menu_Item){msg_menu_discard,  0, 0, 0,   0, 0, 0, 14, 0 };
  menu_data[7] = (Fl_Menu_Item){msg_menu_stats,    0, (Fl_Callback*)stats_cb, (void*)(view->whichtree()),   0, 0, 0, 14, 0 };
  menu_data[8] = (Fl_Menu_Item){msg_menu_quit,     0, (Fl_Callback*)quit_cb, 0,   0, 0, 0, 14, 0 };
  menu_data[9] = (Fl_Menu_Item){ 0 };
  menu_data[10] = (Fl_Menu_Item){msg_menu_folk,     0, 0, 0,  64, 0, 0, 14, 0 };
  menu_data[11] = (Fl_Menu_Item){msg_menu_add,      0, 0, 0,   0, 0, 0, 14, 0 };
  menu_data[12] = (Fl_Menu_Item){msg_menu_goto,     0, (Fl_Callback*)find_cb, (void*)(view),   0, 0, 0, 14, 0 };
  menu_data[13] = (Fl_Menu_Item){ 0 };
  menu_data[14] = (Fl_Menu_Item){msg_menu_reports,  0, 0, 0,  64, 0, 0, 14, 0 };
  menu_data[15] = (Fl_Menu_Item){msg_menu_repstd,   0, 0, 0,   0, 0, 0, 14, 0 };
  menu_data[16] = (Fl_Menu_Item){msg_menu_repfam,   0, 0, 0,   0, 0, 0, 14, 0 };
  menu_data[17] = (Fl_Menu_Item){msg_menu_repweb,   0, 0, 0,   0, 0, 0, 14, 0 };
  menu_data[18] = (Fl_Menu_Item){ 0 };
  menu_data[19] = (Fl_Menu_Item){msg_menu_settings, 0, 0, 0,  64, 0, 0, 14, 0 };
  menu_data[20] = (Fl_Menu_Item){msg_menu_lang,     0, 0, 0,  64, 0, 0, 14, 0 };
  #define next_menu_index 21
  #include "languages.cxx"
  #undef next_menu_index
  menu_data[21+language_count] = (Fl_Menu_Item){ 0 };
  menu_data[22+language_count] = (Fl_Menu_Item){msg_menu_choices,  0, (Fl_Callback*)choices_cb, 0,   0, 0, 0, 14, 0 };
  menu_data[23+language_count] = (Fl_Menu_Item){msg_menu_savech,   0, 0, 0,   0, 0, 0, 14, 0 };
  menu_data[24+language_count] = (Fl_Menu_Item){ 0 };
  menu_data[25+language_count] = (Fl_Menu_Item){msg_help,          0, 0, 0,  64, 0, 0, 14, 0 };
// menu captions need translating but help filenames don't
  menu_data[26+language_count] = (Fl_Menu_Item){msg_menu_intro,    0, (Fl_Callback*)help_cb, (void*)("intro.html"),  0, 0, 0, 14, 0 };
  menu_data[27+language_count] = (Fl_Menu_Item){msg_menu_mainUI,   0, (Fl_Callback*)help_cb, (void*)("mainUI.html"), 0, 0, 0, 14, 0 };
  menu_data[28+language_count] = (Fl_Menu_Item){msg_menu_indiUI,   0, (Fl_Callback*)help_cb, (void*)("indiUI.html"), 0, 0, 0, 14, 0 };
  menu_data[29+language_count] = (Fl_Menu_Item){msg_menu_famUI,    0, (Fl_Callback*)help_cb, (void*)("famUI.html"),  0, 0, 0, 14, 0 };
  menu_data[30+language_count] = (Fl_Menu_Item){msg_menu_prefUI,   0, (Fl_Callback*)help_cb, (void*)("prefUI.html"), 0, 0, 0, 14, 0 };
  menu_data[31+language_count] = (Fl_Menu_Item){msg_menu_GEDCOM,   0, (Fl_Callback*)help_cb, (void*)("GEDCOM.html"), 128, 0, 0, 14, 0 };
  menu_data[32+language_count] = (Fl_Menu_Item){msg_menu_about,    0, (Fl_Callback*)info_cb, 0,   0, 0, 0, 14, 0 };
  menu_data[33+language_count] = (Fl_Menu_Item){ 0 };
  menu_data[34+language_count] = (Fl_Menu_Item){ 0 };
}

///////////////////////////////////////////////////////////////////////////////
// class indipopupmenu:
// for instanciations of the main window click-on-INDI popup menu
// cast with (Fl_Menu_Item*) to use with ->menu(thing)

indipopupmenu::indipopupmenu(mainUI* view) {
  popup_data[0] = (Fl_Menu_Item){msg_menu_edit,      0, (Fl_Callback*)edit_cb, (void*)(view),   0, 0, 0, 14, 0 };
  popup_data[1] = (Fl_Menu_Item){msg_menu_notes,     0, (Fl_Callback*)opennotes_cb, (void*)(view->whichtree()), 0,0,0,14, 0 };
  popup_data[2] = (Fl_Menu_Item){msg_menu_marry,     0, 0, 0,   0, 0, 0, 14, 0 };
  popup_data[3] = (Fl_Menu_Item){msg_menu_older,     0, 0, 0,   0, 0, 0, 14, 0 };
  popup_data[4] = (Fl_Menu_Item){msg_menu_child,     0, 0, 0,   0, 0, 0, 14, 0 };
  popup_data[5] = (Fl_Menu_Item){msg_menu_remove,    0, 0, 0, 128, 0, 0, 14, 0 };
  popup_data[6] = (Fl_Menu_Item){msg_menu_indreps,   0, 0, 0,  64, 0, 0, 14, 0 };
  popup_data[7] = (Fl_Menu_Item){msg_menu_ancs,      0, 0, 0,   0, 0, 0, 14, 0 };
  popup_data[8] = (Fl_Menu_Item){ 0 };
  popup_data[9] = (Fl_Menu_Item){ 0 };
}

///////////////////////////////////////////////////////////////////////////////
// class fampopupmenu:
// for instanciations of the main window click-on-FAM popup menu
// cast with (Fl_Menu_Item*) to use with ->menu(thing)

// FIXME: check whether you really think the same Callback routines will
// work ... (hint, the edit one definitely wouldn't and is now famed_cb)

fampopupmenu::fampopupmenu(mainUI* view) {
  popup_data[0] = (Fl_Menu_Item){msg_menu_edit,      0, (Fl_Callback*)famed_cb, (void*)(view),   0, 0, 0, 14, 0 };
  popup_data[1] = (Fl_Menu_Item){msg_menu_notes,     0, (Fl_Callback*)opennotes_cb, (void*)(view), 0,0,0,14, 0 };
  popup_data[2] = (Fl_Menu_Item){msg_menu_earlier,   0, 0, 0,   0, 0, 0, 14, 0 };
  popup_data[3] = (Fl_Menu_Item){msg_menu_child,     0, 0, 0,   0, 0, 0, 14, 0 };
  popup_data[4] = (Fl_Menu_Item){msg_menu_unmarry,   0, 0, 0, 128, 0, 0, 14, 0 };
  // unmarry is to remove the FAM object entirely. Divorce is a tab in the
  // FAM edit box, and no longer a menu item as it was in !Family
  popup_data[5] = (Fl_Menu_Item){ 0 };
}

///////////////////////////////////////////////////////////////////////////////
// class notesmenu:

notesmenu::notesmenu( notesUI* notesui ) {
  menu_data[0] = (Fl_Menu_Item){msg_cancel, 0, (Fl_Callback*)cancelnotes_cb, (void*)notesui, 0, 0, 0, 14, 0 };
  menu_data[1] = (Fl_Menu_Item){msg_clear,  0, (Fl_Callback*)clearnotes_cb,  (void*)notesui, 0, 0, 0, 14, 0 };
  menu_data[2] = (Fl_Menu_Item){msg_restore, 0, (Fl_Callback*)restorenotes_cb, (void*)notesui, 0, 0, 0, 14, 0 };
  menu_data[3] = (Fl_Menu_Item){msg_saveto, 0, (Fl_Callback*)savenotes_cb,   (void*)notesui, 0, 0, 0, 14, 0 };
  menu_data[4] = (Fl_Menu_Item){msg_oksave, 0, (Fl_Callback*)oknotes_cb,     (void*)notesui, 0, 0, 0, 14, 0 };
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
  indimenu = new indipopupmenu( this );
  fammenu  = new  fampopupmenu( this );

  w->end();
  w->resizable(scroll);
  w->size_range(400,100,maxX,maxY);
  w->callback((Fl_Callback*)killed_cb);
  w->user_data((void*)(this));
}

void mainUI::settitle() {
GEDCOM_string* oldtitle;
string newtitle(msg_window_title);
  newtitle+=" ";
  newtitle+=tree->getfilename();
  GEDCOM_object *thing = this->getcurrent();
  if (thing != NULL) {
    thing = thing->subobject( NAME_tag );
    if (thing != NULL) {
      newtitle+=" : ";
      newtitle+=thing->value();
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
  //printf("canvassize called with %d, %d\n", xsize, ysize);
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
  window->size( nowX, nowY ); // must ensure we are smaller than the max we will set  
  main->size( xsize, ysize );
  window->size_range(400,100,maxX,maxY);
}

mainUI*        mainUI::getnext() const { return next; }
void           mainUI::setnext( mainUI* chained ) { next = chained; }
treeinstance*  mainUI::whichtree() const { return tree; }
displaytree*   mainUI::whattodraw() const { return display; }
GEDCOM_object* mainUI::getcurrent() const { return person; }

void mainUI::setcurrent( GEDCOM_object* newperson ) {
bool redraw;
  if (person == newperson) return;
// industrial strength version would check that the object is indeed an
// INDI object.
  printf("attempting to set current to this object:\n");
  newperson->outline( stdout, 1 );
  newperson->subobject( NAME_tag )->output( stdout, 1 ); // diagnose

  person = newperson;
  if (redraw=(window->shown())) this->hide();
  this->settitle();
  // printf("set new window title OK\n");
  this->settop();
  // printf("set new current person OK\n");
  this->newdisplay();
  // printf("created new display structure OK\n");
  if (redraw) this->show();
}

void mainUI::newdisplay() {
  // printf("newdisplay - destroying old at %d\n", (int)display);
  if (display != NULL) delete display;
  // printf("destroyed that OK. Now start new tree from %d\n", (int)topind);
  display = new displaytree( topind );
  // printf("created new displaytree at %d\n", (int)display);
  display->buildtree();
  // printf("built the tree structure\n");
  display->calctree();
  // printf("calculated where to put everything\n");
  this->canvassize(display->xsize(),display->ysize());
  // printf("and finally set the canvassize\n");
  // which is presumably where we should ensure that the window is actually
  // scrolled within that area, or ideally, scroll to put the current person
  // in the centre at the top ...
}

void mainUI::settop() {
  GEDCOM_object *fam;
  fam = person->parental_family();
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
  Fl_Window* w = info_panel = new Fl_Window( 520, 210, msg_info_title );
  w->labeltype(FL_NORMAL_LABEL);
  w->user_data((void*)(this));
  // all the members of this class are static, because there is only ever one window
  // and it always has the same stuff in it. But every use of a class member below
  // (info_group = etc.) generates an undefined reference at link time. Why ?
  { Fl_Group* o = info_group = new Fl_Group( 0, 0, 520, 210);
    o->labeltype(FL_NO_LABEL);
    { Fl_Box* o = info_name = new Fl_Box( 5, 10, 100, 40, msg_info_name );
      o->labelsize(18);
      o->labelfont(1);
      o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    }
    { Fl_Box* o = author = new Fl_Box(5, 60, 100, 40, msg_info_author);
      o->labelsize(18);
      o->labelfont(1);
      o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    }
    { Fl_Box* o = version = new Fl_Box(5, 110, 100, 40, msg_info_version);
      o->labelsize(18);
      o->labelfont(1);
      o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    }
    { Fl_Box* o = purpose = new Fl_Box(5, 160, 100, 40, msg_info_purpose);
      o->labelsize(18);
      o->labelfont(1);
      o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    }
    { Fl_Box* o = name_l = new Fl_Box(110, 10, 330, 40, msg_appname);
      o->labelsize(18);
      o->labelfont(1);
      o->box(FL_DOWN_BOX);
      o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    }
    { Fl_Box* o = author_l = new Fl_Box(110, 60, 405, 40, msg_author);
      o->labelsize(18);
      o->labelfont(1);
      o->box(FL_DOWN_BOX);
      o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    }
    { Fl_Box* o = version_l = new Fl_Box(110, 110, 405, 40, msg_version);
      o->labelsize(18);
      o->labelfont(1);
      o->box(FL_DOWN_BOX);
    }
    { Fl_Box* o = purpose_l = new Fl_Box(110, 160, 330, 40, msg_purpose);
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
  { Fl_Box* o = gpl_box = new Fl_Box(0, 0, 520, 210, msg_gpl_box);
    o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    o->labelsize(12);
    o->labelcolor(92);
    o->box(FL_FLAT_BOX);
    o->color(0);
    o->hide();
  }
  { Fl_Return_Button* o = info_ok = new Fl_Return_Button(445, 160, 70, 40, msg_ok);
      o->labelsize(18);
      o->labelfont(1);
    o->down_box(FL_DOWN_BOX);
    o->callback((Fl_Callback*)okinfo_cb);
  }
  w->set_non_modal();
  w->end();
}

///////////////////////////////////////////////////////////////////////////////
// class indUI the "edit individual" dbox. We will find a pointer to a
// GEDCOM object is all we need, to know which individual (and thus which
// treeinstance) this relates to. But if we are creating a new person, we
// don't make a GEDCOM_object until user clicks OK, so we also need a tree
// instance (not a mainUI) pointer in the indiUI class. You would also need
// that for a name-completion-request event on a field (indi_name, indi_ma,
// indi_pa) in the edit box - it is no use setting someone's parents to
// people in two different trees !!

indiUI::indiUI( treeinstance* thistree, GEDCOM_object* whofor ):
  who (whofor),       // NULL for a new person not yet in the tree
  which (thistree)
{
  Fl_Window* w;
  { Fl_Window* o = indi_dbox = new Fl_Window( 480, 375, buf_indi_title ); // *not* msg_indi_title :-)
    w = o;
    o->user_data((void*)(this));
    indi_name = new Fl_Input(75, 5, 390, 25, msg_name);
    indi_title = new Fl_Input(75, 35, 255, 25, msg_title);
    { Fl_Check_Button* o = indi_living = new Fl_Check_Button(350, 35, 110, 25, msg_living);
      o->box(FL_UP_BOX);
      o->down_box(FL_DOWN_BOX);
      o->selection_color(91);
      o->labelfont(1);
      o->labelsize(16);
      o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    }
    { Fl_Box* o = new Fl_Box(5, 70, 70, 25, msg_gender);
      o->align(FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
    }
    { Fl_Group* o = new Fl_Group(75, 65, 390, 35);
      o->box(FL_EMBOSSED_BOX);
      { Fl_Light_Button* o = indi_male = new Fl_Light_Button(80, 70, 115, 25, msg_male);
        o->type(102); /* radio button */
        o->down_box(FL_DOWN_BOX);
        o->labelfont(1);
        o->labelsize(16);
        o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
      }
      { Fl_Light_Button* o = indi_female = new Fl_Light_Button(215, 70, 115, 25, msg_female);
        o->type(102); /* radio button */
        o->down_box(FL_DOWN_BOX);
        o->labelfont(1);
        o->labelsize(16);
        o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
      }
      { Fl_Light_Button* o = indi_unknown = new Fl_Light_Button(350, 70, 110, 25, msg_unknown);
        o->type(102); /* radio button */
        o->down_box(FL_DOWN_BOX);
        o->labelfont(1);
        o->labelsize(16);
        o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
      }
      o->end();
    }
    indi_ma = new Fl_Input(75, 105, 390, 25, msg_indi_ma);
    indi_pa = new Fl_Input(75, 135, 390, 25, msg_indi_pa);
    { Fl_Tabs* o = indi_events = new Fl_Tabs(0, 170, 480, 160);
      o->labeltype(FL_NO_LABEL);
      { Fl_Tile* o = indi_birth = new Fl_Tile(0, 195, 480, 135, msg_birth);
        o->color(214);
        indi_birthdate = new Fl_Input(75, 205, 200, 25, msg_event_date);
        indi_birthtime = new Fl_Input(335, 205, 130, 25, msg_event_time);
        indi_birthplace = new Fl_Input(75, 235, 390, 25, msg_event_place);
        indi_birthsite = new Fl_Input(75, 265, 390, 25, msg_event_site);
        o->end();
      }
      { Fl_Tile* o = indi_chr = new Fl_Tile(0, 195, 480, 135, msg_chr);
        o->color(246);
        indi_chrdate = new Fl_Input(75, 205, 200, 25, msg_event_date);
        indi_chrtime = new Fl_Input(335, 205, 130, 25, msg_event_time);
        indi_chrplace = new Fl_Input(75, 235, 390, 25, msg_event_place);
        indi_chrsite = new Fl_Input(75, 265, 390, 25, msg_event_site);
        //o->hide();
        o->end();
      }
      { Fl_Tile* o = indi_bapm = new Fl_Tile(0, 195, 480, 135, msg_bapm);
        o->color(207);
        indi_bapmdate = new Fl_Input(75, 205, 200, 25, msg_event_date);
        indi_bapmtime = new Fl_Input(335, 205, 130, 25, msg_event_time);
        indi_bapmplace = new Fl_Input(75, 235, 390, 25, msg_event_place);
        indi_bapmsite = new Fl_Input(75, 265, 390, 25, msg_event_site);
        //o->hide();
        o->end();
      }
      { Fl_Tile* o = indi_death = new Fl_Tile(0, 195, 480, 135, msg_death);
        o->color(175);
        indi_deathdate = new Fl_Input(75, 205, 200, 25, msg_event_date);
        indi_deathtime = new Fl_Input(335, 205, 130, 25, msg_event_time);
        indi_deathplace = new Fl_Input(75, 235, 390, 25, msg_event_place);
        indi_deathsite = new Fl_Input(75, 265, 390, 25, msg_event_site);
        indi_deathcause = new Fl_Input(75, 295, 390, 25, msg_event_cause);
        //o->hide();
        o->end();
      }
      { Fl_Tile* o = indi_crem = new Fl_Tile(0, 195, 480, 135, msg_crem);
        o->color(174);
        indi_cremdate = new Fl_Input(75, 205, 200, 25, msg_event_date);
        indi_cremtime = new Fl_Input(335, 205, 130, 25, msg_event_time);
        indi_cremplace = new Fl_Input(75, 235, 390, 25, msg_event_place);
        indi_cremsite = new Fl_Input(75, 265, 390, 25, msg_event_site);
        //o->hide();
        o->end();
      }
      { Fl_Tile* o = indi_buri = new Fl_Tile(0, 195, 480, 135, msg_buri);
        o->color(254);
        indi_buridate = new Fl_Input(75, 205, 200, 25, msg_event_date);
        indi_buritime = new Fl_Input(335, 205, 130, 25, msg_event_time);
        indi_buriplace = new Fl_Input(75, 235, 390, 25, msg_event_place);
        indi_burisite = new Fl_Input(75, 265, 390, 25, msg_event_site);
        indi_buriplot = new Fl_Input(75, 295, 390, 25, msg_event_plot);
        //o->hide();
        o->end();
      }
      o->end();
    }
    { Fl_Button* o = indi_cancel = new Fl_Button(45, 340, 100, 25, msg_cancel);
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)canceledit_cb);
      o->user_data((void*)(this));
    }
    { Fl_Return_Button* o = indi_ok = new Fl_Return_Button(190, 340, 100, 25, msg_ok);
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)okedit_cb);
      o->user_data((void*)(this));
    }
    { Fl_Button* o = indi_help = new Fl_Button(335, 340, 100, 25, msg_help);
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)helpedit_cb);
      o->user_data((void*)("indiUI.html"));
    }
    o->end();
  }
}

indiUI::~indiUI() {
  delete indi_dbox;
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
  indi_name->value("");
  indi_title->value("");
  indi_ma->value("");
  indi_pa->value("");
  indi_birthdate->value("");
  indi_birthtime->value("");
  indi_birthplace->value("");
  indi_birthsite->value("");
  indi_chrdate->value("");
  indi_chrtime->value("");
  indi_chrplace->value("");
  indi_chrsite->value("");
  indi_bapmdate->value("");
  indi_bapmtime->value("");
  indi_bapmplace->value("");
  indi_bapmsite->value("");
  indi_deathdate->value("");
  indi_deathtime->value("");
  indi_deathplace->value("");
  indi_deathsite->value("");
  indi_deathcause->value("");
  indi_cremdate->value("");
  indi_cremtime->value("");
  indi_cremplace->value("");
  indi_cremsite->value("");
  indi_buridate->value("");
  indi_buritime->value("");
  indi_buriplace->value("");
  indi_burisite->value("");
  indi_buriplot->value("");
}

void indiUI::insert_details( GEDCOM_object* thisindi ) {
GEDCOM_object *event;
GEDCOM_object *thing; // deeper subobjects
GEDCOM_object *parent;
GEDCOM_object *fam=NULL;
char *val;

  this->clear_details();
  who = thisindi;

#define fillin( field, object, tag ) if ((thing=(object->subobject(tag)))!=NULL) field->value( thing->value() )

  if ((thing=(thisindi->subobject(NAME_tag)))!=NULL) {
    
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
  indi_living->value((thisindi->subobject( LVG_tag ))!=NULL); indi_living->show();
  // we should check for death, cremation or burial being recorded, and disable the living tag if so

  // need convenience functions for some things like mother: to find
  // mother we need to look for a FAMC, follow it to a FAM, and look for
  // a WIFE, which points to an INDI, for which we can look up the NAME !

  fam = thisindi->parental_family();
  if (fam != NULL ) {
    if ((parent =  fam->thewife()) != NULL ) { fillin( indi_ma, parent, NAME_tag ); }
    if ((parent = fam->thehusband()) != NULL){ fillin( indi_pa, parent, NAME_tag ); }
  } 

  event = thisindi->subobject( BIRT_tag );
  if (event!=NULL) {
    fillin( indi_birthdate, event, DATE_tag );
    fillin( indi_birthtime, event, TIME_tag );
    fillin( indi_birthplace, event, PLAC_tag );
    if (thing!=NULL) { fillin( indi_birthsite, thing, SITE_tag ); }
  }

  event = thisindi->subobject( CHR_tag );
  if (event!=NULL) {
    fillin( indi_chrdate, event, DATE_tag);
    fillin( indi_chrtime, event, TIME_tag);
    fillin( indi_chrplace, event, PLAC_tag);
    if (thing!=NULL) fillin( indi_chrsite, thing, SITE_tag);
  }

  event = thisindi->subobject( BAPM_tag );
  if (event!=NULL) {
    fillin( indi_bapmdate, event, DATE_tag);
    fillin( indi_bapmtime, event, TIME_tag);
    fillin( indi_bapmplace, event, PLAC_tag);
    if (thing!=NULL) { fillin( indi_bapmsite, thing, SITE_tag); }
  }

  event = thisindi->subobject( DEAT_tag );
  if (event!=NULL) {
    indi_living->value(0); indi_living->hide();
    fillin( indi_deathdate, event, DATE_tag);
    fillin( indi_deathtime, event, TIME_tag);
    fillin( indi_deathplace, event, PLAC_tag);
    if (thing!=NULL) { fillin( indi_deathsite, thing, SITE_tag); }
    fillin( indi_deathcause, event, CAUS_tag);
  }
  event = thisindi->subobject( CREM_tag );
  if (event!=NULL) {
    indi_living->value(0); indi_living->hide();
    fillin( indi_cremdate, event, DATE_tag);
    fillin( indi_cremtime, event, TIME_tag);
    fillin( indi_cremplace, event, PLAC_tag);
    if (thing!=NULL) { fillin( indi_cremsite, thing, SITE_tag); }
  }

  event = thisindi->subobject( BURI_tag );
  if (event!=NULL) {
    indi_living->value(0); indi_living->hide();
    fillin( indi_buridate, event, DATE_tag);
    fillin( indi_buritime, event, TIME_tag);
    fillin( indi_buriplace, event, PLAC_tag);
    if (thing!=NULL) { fillin( indi_burisite, thing, CEME_tag); }
      if (thing!=NULL) { fillin( indi_buriplot, thing, PLOT_tag); }
    }
  }
  indi_events->value(indi_birth);
}
#undef fillin

void indiUI::settitle() {

// ensure caller has called insert_details first for a real indi, or you
// will get an "edit <new person>" title.

  string title(msg_indi_title);

  title += " : ";
  title += which->getfilename();
  title += " : ";

  if (who==NULL) {
    // edit window for new person - no object yet
    title += msg_newperson;
  } else {
    title += who->getidname();
/*    GEDCOM_object* name = who->subobject( NAME_tag );
    if ( name == NULL ) {
      // edit window for existing person with no name
      title += msg_anonymous;
    } else {
      // edit window for a person with a NAME tag
      char *namestring = name->value();
      if (namestring == NULL) title += msg_anonymous;
      else title += namestring;
      } */
  }
  // now put the title somewhere it won't evaporate as title goes out of scope:
  strncpy(buf_indi_title, title.c_str(), MAX_indititle);
  indi_dbox->label(buf_indi_title);
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
  Fl_Window* w;
  { Fl_Window* o = fam_dbox = new Fl_Window( 480, 275, buf_fam_title ); // *not* msg_fam_title :-)
    w = o;     o->user_data((void*)(this));
    husb_name = new Fl_Output(75, 5, 390, 25, msg_husband);
    wife_name = new Fl_Output(75, 35, 390, 25, msg_wife);
    { Fl_Tabs* o = fam_events = new Fl_Tabs(0, 70, 480, 160);
      o->labeltype(FL_NO_LABEL);
      { Fl_Tile* o = fam_marry = new Fl_Tile(0, 95, 480, 135, msg_marry);
        o->color(246);
        fam_marrydate = new Fl_Input(75, 105, 200, 25, msg_event_date);
        fam_marrytime = new Fl_Input(335, 105, 130, 25, msg_event_time);
        fam_marryplace = new Fl_Input(75, 135, 390, 25, msg_event_place);
        fam_marrysite = new Fl_Input(75, 165, 390, 25, msg_event_site);
        //o->hide();
        o->end();
      }
      { Fl_Tile* o = fam_engage = new Fl_Tile(0, 95, 480, 135, msg_engage);
        o->color(214);
        fam_engagedate = new Fl_Input(75, 105, 200, 25, msg_event_date);
        fam_engagetime = new Fl_Input(335, 105, 130, 25, msg_event_time);
        fam_engageplace = new Fl_Input(75, 135, 390, 25, msg_event_place);
        fam_engagesite = new Fl_Input(75, 165, 390, 25, msg_event_site);
        o->end();
      }
      { Fl_Tile* o = fam_divorce = new Fl_Tile(0, 95, 480, 135, msg_divorce);
        o->color(207);
        fam_divorcedate = new Fl_Input(75, 105, 200, 25, msg_event_date);
        fam_divorcetime = new Fl_Input(335, 105, 130, 25, msg_event_time);
        fam_divorceplace = new Fl_Input(75, 135, 390, 25, msg_event_place);
        fam_divorcesite = new Fl_Input(75, 165, 390, 25, msg_event_site);
        //o->hide();
        o->end();
      }
      o->end();
    }
    { Fl_Button* o = fam_cancel = new Fl_Button(45, 240, 100, 25, msg_cancel);
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)cancelfam_cb);
      o->user_data((void*)(this));
    }
    { Fl_Return_Button* o = fam_ok = new Fl_Return_Button(190, 240, 100, 25, msg_ok);
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)okfam_cb);
      o->user_data((void*)(this));
    }
    { Fl_Button* o = fam_cancel = new Fl_Button(335, 240, 100, 25, msg_help);
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)helpfam_cb);
      o->user_data((void*)("famUI.html"));
    }
    o->end();
  }
}

famUI::~famUI() {
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
  fam_marrydate->value("");
  fam_marrytime->value("");
  fam_marryplace->value("");
  fam_marrysite->value("");
  fam_engagedate->value("");
  fam_engagetime->value("");
  fam_engageplace->value("");
  fam_engagesite->value("");
  fam_divorcedate->value("");
  fam_divorcetime->value("");
  fam_divorceplace->value("");
  fam_divorcesite->value("");
}

void famUI::insert_details( GEDCOM_object* thisfam ) {
GEDCOM_object *event;
GEDCOM_object *thing;
GEDCOM_object *indi;
char *val;

  this->clear_details();

#define fillin( field, object, tag ) if ((thing=(object->subobject(tag)))!=NULL) field->value( thing->value() )

// with a FAM object, names of spouses are not directly accessible - we need
// to dereference the HUSB and WIFE pointers to get to the relevant INDI
// objects

  // dereference to husb indi - there is no guarantee that there is one !
  thing = thisfam->subobject( HUSB_tag );
  if (thing!=NULL) {
    // then thing->ref is a GEDCOM_id which will lead to the right INDI
    indi = thing->followxref();
    if (indi!=NULL) fillin( husb_name, indi, NAME_tag );
    // if indi *is* NULL, then the GEDCOM is broken and we should probably
    // do something about that (if only report the fact)
  }

  // dereference to WIFE INDI
  thing = thisfam->subobject( WIFE_tag );
  if (thing!=NULL) {
    indi = thing->followxref();
    if (indi!=NULL) fillin( wife_name, indi, NAME_tag );
  }

  // we do not in practice expect that thing would have been null for both
  // husband and wife, but we should be able to cope with that. In theory,
  // a FAM object can have just CHIL subobjects to indicate siblinghood.
  // However, in a case with neither HUSB nor WIFE, we probably should not
  // allow the setting of ENGA, MARR, DIV or similar objects...

  event = thisfam->subobject( MARR_tag );
  if (event!=NULL) {
    fillin( fam_marrydate, event, DATE_tag);
    fillin( fam_marrytime, event, TIME_tag);
    fillin( fam_marryplace, event, PLAC_tag);
    if (thing!=NULL) fillin( fam_marrysite, thing, SITE_tag);
  }

  event = thisfam->subobject( ENGA_tag );
  if (event!=NULL) {
    fillin( fam_engagedate, event, DATE_tag );
    fillin( fam_engagetime, event, TIME_tag );
    fillin( fam_engageplace, event, PLAC_tag );
    if (thing!=NULL) { fillin( fam_engagesite, thing, SITE_tag ); }
  }

  event = thisfam->subobject( DIV_tag );
  if (event!=NULL) {
    fillin( fam_divorcedate, event, DATE_tag);
    fillin( fam_divorcetime, event, TIME_tag);
    fillin( fam_divorceplace, event, PLAC_tag);
    if (thing!=NULL) { fillin( fam_divorcesite, thing, SITE_tag); }
  }

}
#undef fillin

void famUI::settitle() {

// ensure caller has called insert_details first for a real indi, or you
// will get an "edit <new person>" title.

  string title(msg_fam_title);

  title += " : ";
  title += which->getfilename();
  title += " : ";

  if (fam==NULL) {
    // edit window for new family (shouldn't happen) - no object yet
    title += msg_newfamily;
  } else {
    title += fam->getidname();
  }
  // now put the title somewhere it won't evaporate as title goes out of scope:
  strncpy(buf_fam_title, title.c_str(), MAX_famtitle);
  fam_dbox->label(buf_fam_title);
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
  Fl_Window* w;
  { Fl_Window* o = statsdbox = new Fl_Window(520, 165, msg_stats_title);
    w = o;
    o->user_data((void*)(this));
    { Fl_Output* o = statsfile = new Fl_Output(140, 10, 380, 25, msg_stats_file);
      o->box(FL_RFLAT_BOX);
      o->color(49);
      o->textfont(1);
    }
    { Fl_Output* o = indicount = new Fl_Output(165, 45, 95, 25, msg_stats_indis);
      o->color(49);
    }
    { Fl_Output* o = indimax = new Fl_Output(400, 45, 110, 25, msg_stats_max_indi);
      o->color(49);
    }
    { Fl_Output* o = famcount = new Fl_Output(165, 75, 95, 25, msg_stats_fams);
      o->color(49);
    }
    { Fl_Output* o = fammax = new Fl_Output(400, 75, 110, 25, msg_stats_maxfam);
      o->color(49);
    }
    { Fl_Output* o = srccount = new Fl_Output(165, 105, 95, 25, msg_stats_srcs);
      o->color(49);
    }
    { Fl_Output* o = srcmax = new Fl_Output(400, 105, 110, 25, msg_stats_max_src);
      o->color(49);
    }
    { Fl_Output* o = repocount = new Fl_Output(165, 135, 95, 25, msg_stats_repos);
      o->color(49);
    }
    { Fl_Output* o = repomax = new Fl_Output(400, 135, 110, 25, msg_stats_max_repo);
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
// class findUI controls find person window - only one instance of the window
// as it is a short-lived transient sort of thing. It will search the tree of
// the window it was raised on, and pass the handle of that window to its OK
// callback, so the current person can be set *for that window*

findUI::findUI() {
  { Fl_Window* o = findbox = new Fl_Window(450, 65, msg_find_title);
    o->labeltype(FL_NO_LABEL);
    o->user_data((void*)(this));
    find_input = new Fl_Input( 65, 5, 380, 25, msg_find );
    { Fl_Return_Button* o = find_ok = new Fl_Return_Button( 345, 35, 100, 25, msg_ok );
      o->down_box(FL_DOWN_BOX);
      o->labelfont(1);
      o->callback((Fl_Callback*)okfind_cb);
    }
    o->hotspot( find_ok, 0);
    o->end();
  }
}

void findUI::setview( mainUI* raisedon ) {
  find_ok->user_data( (void*)raisedon );
}


///////////////////////////////////////////////////////////////////////////////
// class prefUI controls user-preferences window - global to program so only
// one instance.

// hmmm. if we want (eventually) to be able to set things like "show descendants"
// and "show ancestors" on a per-window basis, that will need to change. Some
// settings may be global, but others are going to be per-tree or per-view

prefUI::prefUI() {
  Fl_Window* w;
  { Fl_Window* o = optsbox = new Fl_Window(460, 315, msg_prefs_title);
    w = o;
    o->color(50);
    o->labeltype(FL_NORMAL_LABEL);
    o->user_data((void*)(this));
    { Fl_Tabs* o = choices = new Fl_Tabs(0, 20, 460, 240);
      o->labeltype(FL_NO_LABEL);
      { Fl_Tile* o = nameprefs = new Fl_Tile(0, 50, 460, 210, msg_prefs_names);
        o->color(246);
        o->hide();
        { Fl_Group* o = new Fl_Group(15, 75, 255, 75);
          o->box(FL_EMBOSSED_BOX);
          o->labeltype(FL_NO_LABEL);
          { Fl_Check_Button* o = showfam = new Fl_Check_Button(25, 85, 235, 25, msg_prefs_famnames);
            o->box(FL_UP_BOX);
            o->down_box(FL_DOWN_BOX);
          }
          { Fl_Check_Button* o = showtitle = new Fl_Check_Button(25, 115, 235, 25, msg_prefs_titles);
            o->box(FL_UP_BOX);
            o->down_box(FL_DOWN_BOX);
          }
          o->end();
        }
        o->end();
      }
      { Fl_Tile* o = dateprefs = new Fl_Tile(0, 50, 460, 210, msg_prefs_dates);
        o->color(207);
        { Fl_Group* o = new Fl_Group(5, 75, 450, 135);
          o->box(FL_EMBOSSED_BOX);
          o->labeltype(FL_NO_LABEL);
          { Fl_Check_Button* o = treedates = new Fl_Check_Button(15, 85, 200, 25, msg_prefs_treedates);
            o->box(FL_UP_BOX);
            o->down_box(FL_DOWN_BOX);
          }
          { Fl_Box* o = new Fl_Box(15, 115, 200, 25, msg_prefs_including);
            o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
          }
          { Fl_Check_Button* o = spousedates = new Fl_Check_Button(15, 145, 200, 25, msg_prefs_spousedates);
            o->box(FL_UP_BOX);
            o->down_box(FL_DOWN_BOX);
          }
          { Fl_Check_Button* o = marrdates = new Fl_Check_Button(15, 175, 200, 25, msg_prefs_marrydates);
            o->box(FL_UP_BOX);
            o->down_box(FL_DOWN_BOX);
          }
          { Fl_Check_Button* o = yearonly = new Fl_Check_Button(240, 85, 200, 25, msg_prefs_yearonly);
            o->box(FL_UP_BOX);
            o->down_box(FL_DOWN_BOX);
          }
          { Fl_Box* o = new Fl_Box(240, 115, 200, 25, msg_prefs_dateformat);
            o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
          }
          { Fl_Group* o = new Fl_Group(235, 145, 210, 55);
            { Fl_Light_Button* o = prefgedcom = new Fl_Light_Button(240, 145, 200, 25, msg_prefs_gedcom);
              o->type(102);
            }
            { Fl_Light_Button* o = prefios = new Fl_Light_Button(240, 175, 200, 25, msg_prefs_iso);
              o->type(102);
            }
            o->end();
          }
          o->end();
        }
        o->end();
      }
      { Fl_Tile* o = miscopts = new Fl_Tile(0, 65, 460, 195, msg_prefs_misc);
        o->color(175);
        o->hide();
        { Fl_Group* o = xref = new Fl_Group(5, 70, 185, 175);
          o->box(FL_EMBOSSED_BOX);
          o->labeltype(FL_NO_LABEL);
          { Fl_Box* o = xref1 = new Fl_Box(20, 85, 105, 25, msg_prefs_xref);
            o->labelfont(1);
            o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
          }
          { Fl_Light_Button* o = xrefquit = new Fl_Light_Button(25, 120, 150, 25, msg_prefs_xrefquit);
            o->type(102);
          }
          { Fl_Light_Button* o = xrefwarn = new Fl_Light_Button(25, 160, 150, 25, msg_prefs_xrefwarn);
            o->type(102);
          }
          { Fl_Light_Button* o = xrefignore = new Fl_Light_Button(25, 200, 150, 25, msg_prefs_xrefignore);
            o->type(102);
          }
          o->end();
        }
        { Fl_Group* o = datestamp = new Fl_Group(195, 70, 260, 175);
          o->box(FL_EMBOSSED_BOX);
          o->labeltype(FL_NO_LABEL);
          { Fl_Box* o = datestamp1 = new Fl_Box(210, 85, 235, 25, msg_prefs_repdates);
            o->labelfont(1);
            o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
          }
          { Fl_Light_Button* o = stampiso = new Fl_Light_Button(215, 120, 100, 25, msg_prefs_repiso);
            o->type(102);
          }
          { Fl_Light_Button* o = stampgedcom = new Fl_Light_Button(335, 120, 100, 25, msg_prefs_repgedcom);
            o->type(102);
          }
          { Fl_Light_Button* o = stamplocal = new Fl_Light_Button(215, 160, 100, 25, msg_prefs_replocal);
            o->type(102);
          }
          { Fl_Light_Button* o = stampother = new Fl_Light_Button(335, 160, 100, 25, msg_prefs_repother);
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
    { Fl_Button* o = cancelpref = new Fl_Button(16, 275, 96, 25, msg_cancel);
      o->labelfont(1);
      o->callback((Fl_Callback*)cancelchoices_cb);
    }
    { Fl_Button* o = applypref = new Fl_Button(128, 275, 96, 25, msg_apply);
      o->labelfont(1);
      o->callback((Fl_Callback*)applychoices_cb);
    }
    { Fl_Return_Button* o = okpref = new Fl_Return_Button(240, 275, 96, 25, msg_save);
      o->selection_color(33);
      o->labelfont(1);
      o->callback((Fl_Callback*)okchoices_cb);
    }
    { Fl_Button* o = applypref = new Fl_Button(352, 275, 96, 25, msg_help);
      o->labelfont(1);
      o->callback((Fl_Callback*)helpchoices_cb);
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

  window = new Fl_Window(720, 275, notes_title);
  // with this font, that is width for 80 characters, which are
  // 15 pixels high.
   this->retitle();

   // do we need a menu ? or buttons "Cancel" and "save to GEDCOM" ?
   // we might need a menu if we are going to implement things like
   // "save notes to a file", "load notes from a file", "append file
   // to notes" ... although equally, we should be able to do that
   // with some sort of drag and drop ....
   // lets start with just the buttons on a menu bar:
   menu = new notesmenu( (this) );
   menubar = new Fl_Menu_Bar( 0, 0, 720, 25);
   menubar->menu((Fl_Menu_Item*)menu);

   input = new Fl_Multiline_Input(0, 25, 720, 250);
   input->callback((Fl_Callback*)changenotes_cb);
   input->user_data((void*)(this));
   input->when(0);
   input->textfont(FL_SCREEN_BOLD);

  window->end();
  window->user_data((void*)(this));
  window->callback((Fl_Callback*)quitnotes_cb);
  window->resizable(input);
  window->size(720,50+(this->restore())*15);
  input->when(FL_WHEN_CHANGED);
}

int notesUI::restore() {
  // concatenate all subobjects of type edittag, and any of their
  // subobjects of type CONT_tag or CONC_tag. the value() of a
  // CONT object should be inserted into the buffer *after* a '\n'

  GEDCOM_object *note, *sub, *conc;
  int len, pos = 0, lines = 0;
  char *string;

  input->value("");
  note = editobject->subobject( edittag );
  while (note != NULL ) {
    string = note->value();
    if (string!=NULL) {
      len=strlen(string);
      input->replace(pos,pos,string,0);
      pos += len;
    }
    sub = note->subobject();
    if (sub != NULL) {
      while ((sub->objtype())==CONC_tag) {
         string = sub->value();
         if (string!=NULL) { // (which it is allowed to be - a blank line)
           input->replace(pos,pos,string,len=strlen(string));
           pos += len;
         }
         sub = sub->next_object();
         if (sub==NULL) break;
      } // end of CONC lines, no CONT to follow if sub is NULL
    }
    while (sub != NULL) {
      if ((sub->objtype())==CONT_tag) {
        input->replace(pos,pos,"\n",1); lines++;
        pos++;
        string = sub->value();
        if (string!=NULL) { // (which it is allowed to be - a blank line)
          input->replace(pos,pos,string,len=strlen(string));
          pos += len;
        }
        conc = sub->subobject();
        while (conc!=NULL) {
           if ((conc->objtype())==CONC_tag) {
             string = conc->value();
             if (string!=NULL) { // (which it is allowed to be - a blank line)
               input->replace(pos,pos,string,len=strlen(string));
               pos += len;
             }
          }
          conc = conc->next_object();
        } // end of CONCs of this CONT
      } // end of CONT object
      sub = sub->next_object(); // get another CONT if there is one
    } // no more subobjects
    input->replace(pos,pos,"\n",1); lines++;
    note = note->next_object( edittag );
  } // loop concatenates all edittag objects into one
  size = pos;
  return lines;
}

void notesUI::update() {
// update the GEDCOM from the edited text
  if (!modified) return; // save a load of effort here kiddies !
// bin the old notes object(s)
  GEDCOM_object *oldnotes;
  oldnotes = editobject->subobject( edittag );
  while (oldnotes!=NULL) {
    if (!editobject->remove_subobject( oldnotes )) printf("Internal error: couldn't remove subobject - bad things will happen soon\n");
    delete oldnotes;
    oldnotes = editobject->subobject( edittag );
  }
  size = input->size(); // this isn't updated automatically ...
  if (size==0) return; // nothing to do if all text now gone
  char *line, *ptr, *end; GEDCOM_object *noteobject;
  line = (char*) input->value(); // discard const-ness
  end = line + size;
  ptr = line; while (*ptr!='\n') ptr++;
  *ptr = '\0';
  noteobject = new GEDCOM_object( edittag, line );
  *ptr = '\n'; // we are fiddling with the real buffer here, so restore that
  ptr++; line = ptr;
  while (ptr<end) {
    while ((*ptr!='\n')&&(ptr<end)) ptr++;
    *ptr = '\0';
    noteobject -> add_subobject( new GEDCOM_object( CONT_tag, line ));
    *ptr = '\n'; // we are fiddling with the real buffer here, so restore that
    ptr++; line = ptr;
  }
  editobject -> add_subobject( noteobject );
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

void notesUI::changed(bool whether) {
  modified = whether;
}

bool notesUI::changed() const {
  return modified;
}

void notesUI::clear() {
//  input->replace(0, size, "", 0); would be fine if size was maintained up to date
  input->value("");
}

void notesUI::retitle() {
GEDCOM_tag* objecttag = editobject->objtype();
char *tempstring;
string title(msg_notes_title);
// called after doing a language change as well as when the
// notes window is first created

  title += " ";
  title += objecttag->GEDCOM_namefromtag();
  title += " ";
  if ((tempstring=editobject->getidname())!=NULL) {
    title += tempstring;
    title += " ";
  }
  if (objecttag==INDI_tag) {
    GEDCOM_object* nameobj;
    if ((nameobj=(editobject->subobject(NAME_tag)))==NULL)
      title += msg_unnamed;
    else
      title += nameobj->value();
  }
  title += " (";
  title += edittag->GEDCOM_namefromtag();
  // potentially there is a usable string associated with a SOUR object,
  // but it might appear under TITL, or various other things, including
  // the SOUR tag itself.
  title += ") ";
  if (modified) title += "* ";
  title += msg_in;
  title += " ";
  title += edittree->getfilename();
  // now put the title where the window expects it and it won't evaporate as title goes out of scope:
  strncpy(notes_title, title.c_str(), MAX_notestitle);
  
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


