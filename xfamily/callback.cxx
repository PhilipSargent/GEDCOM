// callback.cxx

#include <FL/Fl.H>
#include <FL/Fl_Menu.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Multiline_Input.H>

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "classes.h"
#include "family.h"
#include "structure.h"
#include "gui.h"
#include "display.h"
#include "trees.h"
#include "objects.h"
#include "xlate.h"
#include "callback.h"
#include "strings.h"
#include "tags.h"

///////////////// callback routines for the GUI classes ///////////////

// callback routines from mainUI menu applying to whole program

void language_cb(Fl_Menu_ *, void *index) {
  xlate_messages(languages[(int)index]);
  int x,y;
  // eventually we will call ->reopen on a linked list class for these UIs
  trees->reopen();
  if (aboutbox->info_panel->shown()) {
    aboutbox->info_panel->hide();
    aboutbox->info_panel->show();
  }
  // these should become ->reopen() methods of the respective UIs:
  if (choicebox->optsbox->shown()) {
    x = choicebox->optsbox->x();
    y = choicebox->optsbox->y();
    //printf("moving from %d, %d\n",x,y);
    choicebox->optsbox->hide();
    choicebox->optsbox->position(x-6,y-22);
    choicebox->optsbox->show();
  }
  if (gotobox->findbox->shown()) {
    gotobox->findbox->hide();
    gotobox->findbox->show();
  }
  if ((completionsbox->completionswin)!=NULL) {
    if (completionsbox->completionswin->shown()) {
      completionsbox->completionswin->hide();
      completionsbox->completionswin->show();
    }
  }
  // these have already become methods of their UIs - there may be
  // multiple instances of these window types:
  noteUIs->retitle();
  editUIs->retitle();
}

////////////////////////////////////////////////////////////////////////////////
//
// callback routines from mainUI menu applying to whole tree

void save_cb( Fl_Menu_ *, void *userdata) {
// we should find the TRLR section level 0 NOTE object which specifies
// the current person, and replace its value() with the string
// "Person "+<name_of_current_person> before writing the file.

// this gets more complex with multiplew views - do we use the current
// person in this view ? In which case the callback needs to receive
// the mainUI pointer, not the treeinstance pointer:

  mainUI *view = (mainUI*)userdata;
  treeinstance *tree = view->whichtree();

  tree->setfilename("/var/test.ged");
  tree->save();

}

void newview_cb(Fl_Menu_ *, void *userdata) {

  mainUI *view = (mainUI*)userdata;       // need this to decide what to set current
  //printf("Newview from existing view at %ld\n",(long)view);
  treeinstance *tree = view->whichtree();
  //printf("View will be on treeinstance %ld\n",(long)tree);
  mainUI *newview  = new mainUI( tree );
  printf("incarnated new mainUI at %ld\n",(long)newview);
  tree->addview(newview);
  printf("added it to tree\n");
  GEDCOM_object* tempfortest = view->getcurrent();
  printf("getcurrent for existing view was %ld\n",(long)tempfortest);
  newview->setcurrent( tempfortest );
  //newview->setcurrent( view->getcurrent() );
  printf("Set current\n");
  newview->show();
  printf("and now you should see it !\n");
}

////////////////////////////////////////////////////////////////////////////////
//
// callback routines for mainUI

// FIXME: these need to get a load more complex as we start to allow
// multiple views on one tree:

// File=>Quit will cease to be a menu item on the mainUI shortly

void quit_cb(Fl_Menu_ *, void *userdata) {
mainUI *view = (mainUI*)userdata;
  // in the real world, we need to check for unsaved data and perhaps
  // open a save/cancel/discard dbox. Gets more complex if we have
  // multiple instanciations of the main window, with multiple GEDCOM
  // files loaded....
  exit(0);
}

// destroying a single view will not exit shortly, it will just remove
// the view. If it was the last view open (on all trees), it should pop
// up the "master control" UI

// we should also rename this to reflect which window is being killed, since
// we need to provide callbacks for every window ...

void killed_cb(Fl_Window*, void*userdata) {
mainUI *view = (mainUI*)userdata;
  // essentially identical to the above, except for the type of the first
  // parameter - this one is from the callback we get when a mainUI window
  // is closed by its close icon - until we do save/cancel/discard, its sole
  // function is to stop other windows being left open
  exit(0);
}

void popup_cb(Fl_Button *, void *userdata) {
int x,y,button;
GEDCOM_object *clicked;
mainUI *view = (mainUI*)userdata;

Fl_Menu_Item *picked;
  x = Fl::event_x();
  y = Fl::event_y();
  button = Fl::event_button();
  //printf("button clicked was %d.\n", button);
  clicked = view->main->whoisat(x,y);

  if (button == 1) {
    if (clicked == NULL) return; // as opposed to a segmentation fault, for example...
     if (clicked->objtype() == INDI_tag) {
      view->setcurrent( clicked );
      //return;
    }
    return; // select-click on marriage does nothing yet
  }
  // so now have a right (3) or middle (2) click - currently treat both the same, but eventually decide
  // on the basis of a look-and-feel selector (RISCOS vs Windows/Linux) which will do menu

  // given a menu-requesting-click, we need to decide, on the basis of where we were clicked,
  // which menu to open, view->indimenu or view->fammenu

  if (clicked == NULL) { printf("popup_cb - no object\n"); return;} // eventually a generic popup to allow things like newperson, goto ...
/*  if (clicked->objtype() == FAM_tag) { printf("popup_cb - FAM object\n"); return; } // not yet implemented
  if (clicked->objtype() != INDI_tag) { printf("popup_cb bad object type clicked\n"); return; } */

  if (clicked->objtype() == INDI_tag) {
    // printf("popup_cb - an INDI at %ld\n", (long)clicked);
    //sprintf(person_buffer,"%s x %d: y %d",clicked->subobject( NAME_tag )->value(),x,y);
    sprintf(person_buffer,"%s",clicked->subobject( NAME_tag )->value());

    view->whichtree()->setevent( clicked );

    // need cast to discard const-ness 
    picked=(Fl_Menu_Item*)((Fl_Menu_Item*)view->indimenu)->popup(x-40,y-10,person_buffer,(Fl_Menu_Item*)NULL,(Fl_Menu_*)NULL);
    if (picked==NULL) return;
    if (picked->callback_==NULL) return;
    picked->do_callback((Fl_Widget*)picked,view->whichtree());
    return;
  }
  if (clicked->objtype() == FAM_tag) {
    sprintf(person_buffer,"%s",clicked->getidname());
    view->whichtree()->setevent( clicked );
    // need cast to discard const-ness 
    picked=(Fl_Menu_Item*)((Fl_Menu_Item*)view->fammenu)->popup(x-40,y-10,person_buffer,(Fl_Menu_Item*)NULL,(Fl_Menu_*)NULL);
    if (picked==NULL) return;
    if (picked->callback_==NULL) return;
    picked->do_callback((Fl_Widget*)picked,view->whichtree());
    return;
  }
  // we already covered clicked==NULL, so there should be no way to get here
}

////////////////////////////////////////////////////////////////////////////////
//
// callback routines from mainUI menu to start help

extern void help_cb(Fl_Menu_ *, void* userdata ) {
  shell_for_help( userdata );
}

void shell_for_help( void* urlleaf ) {
  // urlleaf points to leafname of helpfile (which is const, as it happens)
  // now need to shell out to a browser ( kdehelp/khelpcenter/gnome-help-browser or $(BROWSER) )
  // with parameter file:/usr/share/xfamily/help/HTML/<language-code>/+urlleaf

  //char gethelp[]="gnome-help-browser file:/usr/share/xfamily/help/HTML/en/endpiece.html";

  // for proof-of-concept, we will assume we are under KDE2:
  // if we have kde2, then we can call khelpcenter, but only if we have done a link
  // ln -s /usr/share/xfamily/help/HTML/en /opt/kde2/share/doc/HTML/en/xfamily
  // so that we can use a help: url. Otherwise we'd have to use konqueror directly
  char gethelp[]="khelpcenter help:/xfamily/endpiece.html";
  unsigned int leafoffset=strlen(gethelp)-13;
  printf("initial gethelp string is: >%s<\n",gethelp);
  printf("string to replace >%s< is >%s< of length %d\n",&gethelp[leafoffset],(char*)urlleaf,strlen((char*)urlleaf));
  strcpy(&gethelp[leafoffset],(char*)urlleaf);
  printf("the one we will shell with >%s<\n",gethelp);
  if ( fork() == 0 ) {
    printf("I am the new process\n");
    const char* shell="/bin/sh";
    if (getenv("SHELL")) shell=getenv("SHELL");
    execl(shell,shell,"-c",(const char*)gethelp,0L);
    // if that returns, we are shafted:
    printf("The execl has returned - our new process will exit. Bye !\n");
    exit(1);
  }
  printf("I am the original process - still here\n");
}


////////////////////////////////////////////////////////////////////////////////
//
// callback routines for infoUI

void info_cb(Fl_Menu_ *, void *) {
  aboutbox->gpl_box->hide();
  aboutbox->info_group->show();
  aboutbox->info_panel->show(); // this should do "raise" but doesn't seem to
}

void okinfo_cb(Fl_Return_Button *, void *) {
  aboutbox->info_panel->hide();
}

void gplinfo_cb(Fl_Button *, void *) {
  aboutbox->info_group->hide();
  aboutbox->gpl_box->show();
  aboutbox->info_panel->show();
}

////////////////////////////////////////////////////////////////////////////////
//
// callback routines for indiUI

void edit_cb(Fl_Menu_ *menu, void *userdata) {
  indiUI *neweditbox;
  //menu->value(0);
  // but we do need to know the instanciation of the mainUI we are being
  // called on, so we can find out whom to edit !! (this should have been
  //mostly done by popup_cb, who found out where the mouse was clicked
  // and should have put a title on the menu to say who we would edit)
  treeinstance* tree = (treeinstance*)userdata;
  neweditbox = editUIs->open(tree, tree->geteventobject());
}

void okedit_cb(Fl_Return_Button *, void *userdata) {

indiUI* indibox = (indiUI*)userdata;
GEDCOM_object* edited = indibox->whois();
treeinstance* thisinstance = indibox->whichis();
if (edited==NULL) {
  // then we were creating a brand-new person, and we need to know
  // which window it related to. We can then create a new GEDCOM_object
  // and add it to the indilist chain at an appropriate point. We may also need
  // to add new objects for each parent - otherwise we need to match them to
  // existing objects. There is a potential gotcha here: if we have opened this
  // indibox by clicking "child" on a marriage, the parents will already be filled
  // in - we need to be sure that we don't get confused by other people with the
  // same names

  // no, no, no, this is going to be too hard, and will involve loads of extra code
  // just to save the "expense" of creating a GEDCOM_object we might not use.
  // What we need is for the "New" and "Child" callbacks to create empty
  // GEDCOM_objects, perhaps with parent pointers filled in. If we are adding a
  // child to a single parent, that might involve creating a FAM as well as an INDI.
  // so, lets stop writing comments here and go off to the relavant place to do that
  } else {
  // we were editing a specific person, but we still might need to
  // create new parents in the relevant tree. Much the same gotcha as above
  // applies here
  }
// then copy all that stuff into the relevant object
// editUIs is the (global) list of currently open indiUIs, so close is a method on
// class editlist which is defined in structure.h/.cxx
  editUIs->close( indibox );
}

void canceledit_cb(Fl_Button *, void *userdata) {
indiUI* indibox = (indiUI*)userdata;
GEDCOM_object* edited = indibox->whois();
// just bin all the changes
// but not like this ! that leaves someone with a pointer to a dead
// thing !!! delete indibox;
  editUIs->close( indibox );
}

void helpedit_cb(Fl_Button *, void *userdata) {
// we need to call our routine to shell out to a help browser
// with URL file:/usr/share/xfamily/help/HTML/<language-code>/indiUI.html

  shell_for_help( userdata );
}

void searchma_cb(Fl_Button *, void *userdata) {
}

void searchpa_cb(Fl_Button *, void *userdata) {
}

////////////////////////////////////////////////////////////////////////////////
//
// callback routines for famUI

void famed_cb(Fl_Menu_ *menu, void *userdata) {
  famUI *newfambox;
  //menu->value(0);
  // but we do need to know the instanciation of the mainUI we are being
  // called on, so we can find out whom to edit !! (this should have been
  //mostly done by popup_cb, who found out where the mouse was clicked
  // and should have put a title on the menu to say who we would edit
  treeinstance* tree = (treeinstance*)userdata;
  // we need to get a pointer to our particular view, and then use
  // the x and y position to see who the menu click was over ...
  // then fill in all the details of the person. indi_ui probably needs a
  // member of type GEDCOM_object* (or indeed, Indi*) to say who we are
  // editing (NULL for a new person yet to be created).

  newfambox = famUIs->open(tree, tree->geteventobject());
}

void okfam_cb(Fl_Return_Button *, void *userdata) {

famUI* fambox = (famUI*)userdata;
GEDCOM_object* edited = fambox->whatis();
treeinstance* thisinstance = fambox->whichis();
if (edited==NULL) {
  // then we were creating a brand-new family, and we need to know
  // which window it related to. We can then create a new GEDCOM_object
  // and add it to the famlist chain at an appropriate point.
  } else {
  // we were editing a specific family
  }
// then copy all that stuff into the relevant object
  famUIs->close( fambox );
}

void cancelfam_cb(Fl_Button *, void *userdata) {
famUI* fambox = (famUI*)userdata;
GEDCOM_object* edited = fambox->whatis();
// just bin all the changes
// but not like this ! that leaves someone with a pointer to a dead
// thing !!! delete indibox;
  famUIs->close( fambox );
}

void helpfam_cb(Fl_Button *, void *userdata) {
// we need to call our routine to shell out to a help browser
// with URL file:/usr/share/xfamily/help/HTML/<language-code>/famUI.html

  shell_for_help( userdata );
}

////////////////////////////////////////////////////////////////////////////////
//
// callback routines for statsUI

void stats_cb(Fl_Menu_ *, void *userdata) {
  treeinstance* instanciation = (treeinstance*)userdata;
  statsUI* statsbox = instanciation->statsui();
  if (statsbox->shown()) statsbox->hide();
  statsbox->setvalues();
  statsbox->show();
}

////////////////////////////////////////////////////////////////////////////////
//
// callback routines for findUI

void find_cb(Fl_Menu_*, void *userdata) {
  mainUI* view = (mainUI*)userdata;
//  printf("goto raised on instanciation %d\n",(int)instanciation);
//  probably don't actually need to know which instance, until we need name completion
  int x,y;
  x = view->window->x();
  y = view->window->y();

  gotobox->open( view, x+50, y+55 );
}

void okfind_cb(Fl_Return_Button*, void *userdata) {
  mainUI* view = (mainUI*)userdata;
//  printf("find OK raised on instanciation %d\n",(int)instanciation);
//  printf("We'll call Lookup_INDI on a tree at %d\n",(int)(instanciation->treeroot()));
  // need to cast parm to discard const-ness explicitly
  GEDCOM_object* newperson = view->whichtree()->Lookup_INDI( (char *)gotobox->find_input->value());
  if (newperson != NULL) {
    view->setcurrent(newperson);
    gotobox->findbox->hide();
    // make sure we didn't leave a completions window lying around
    completionsbox->finish();
  }
  // else: should raise a warning if we didn't find that person
}

void binnedfind_cb(Fl_Window*, void *userdata) {
  // make sure we didn't leave a completions window lying around
  completionsbox->finish();
  gotobox->finish();
}

void searchfind_cb(Fl_Button*, void *userdata) {
  mainUI* view = (mainUI*)userdata;
// that will give us the view on which the completions code will be trying to
// find matching entries for the partial name in ... what ?
// the text is in (char *)gotobox->find_input->value()
// and we need a fuzzy search equivalent of view->whichtree()->Lookup_INDI( char* )
// then we need to raise a completions window to browse all the possible
// matches in the current tree ... but ... one step at a time.
  short x,y;
  x = gotobox->findbox->x();
  y = gotobox->findbox->y();
  completionsbox->open( view->whichtree(),
                        (char*)gotobox->find_input->value(),
                        0,
                        (Fl_Callback*) chosenfind_cb,
                        (void*) view, x+65, y+55 );
}

void chosenfind_cb( Fl_Select_Browser* chooser, void *userdata ) {
// userdata is (void*) of the mainUI class ... which for this callback
// is always going to be a mainUI (ie. a particular view).
  mainUI* view = (mainUI*)userdata;
  int chosen = chooser->value();
  printf("completions selected item %d\n", chosen);
  GEDCOM_object* newperson = completionsbox->chosen_indi( chosen );
  completionsbox->finish();
  if (newperson != NULL) {
    view->setcurrent(newperson);
    gotobox->findbox->hide();
  }
}

void completionbinned_cb( Fl_Window*, void *userdata ) {
  printf("completions window closed with no choice made\n");
  completionsbox->finish();
}

void helpfind_cb(Fl_Button *, void *userdata) {
// we need to call our routine to shell out to a help browser
// with URL file:/usr/share/xfamily/help/HTML/<language-code>/findUI.html

  shell_for_help( userdata );
}

////////////////////////////////////////////////////////////////////////////////
//
// callback routines for prefUI

// currently, most of these are no more than place holders

void choices_cb(Fl_Menu_ *, void *) {
  choicebox->optsbox->show();
}

void okchoices_cb(Fl_Button*, void*) {
  choicebox->optsbox->hide();
}

void cancelchoices_cb(Fl_Button*, void*) {
  choicebox->optsbox->hide();
}

void applychoices_cb(Fl_Button*, void*) {
}

void helpchoices_cb(Fl_Button *, void *userdata) {
// we need to call our routine to shell out to a help browser
// with URL file:/usr/share/xfamily/help/HTML/<language-code>/prefUI.html

  shell_for_help( userdata );
}

////////////////////////////////////////////////////////////////////////////////
//
// callback routines to drive notesUI

// we call opennotes from one of the popup menus in a mainUI window
// eventually, the userdata is going to be a particular GEDCOM_object*
// as determined by the mainUI code based on the click position.

// this is slightly bad news - we can't get from a GEDCOM_object to the
// treeinstance it is part of, but we will need to know the treeinstance
// in order to reflect the tree filename in the window title, and to mark
// the tree as modified if we eventually save changed notes back to a tree.

// this probably means that we need to keep a pointer in each tree to the
// last GEDCOM_object on which we processed an event. The plot is thickening ...

void opennotes_cb(Fl_Menu_ *menu, void *userdata) {
notesUI* newui;
  treeinstance* tree = (treeinstance*)userdata;
  // the code which popped up the menu should have called setevent for
  // the relevant object, so all we will need to do is look at the value
  newui = noteUIs->open(tree, tree->geteventobject(), NOTE_tag);
// noteUI->open() should do this:  newui->window->show();
}

// for each callback, we have to work back to find the notesUI. That was
// why we set the menu item userdata to ((void*)(this)) in the constructor :-)

void changenotes_cb(Fl_Multiline_Input*, void*userdata) {
notesUI *ui;
  ui = (notesUI*)userdata;
  ui->changed(true);
}

void cancelnotes_cb(Fl_Menu_*, void*userdata) {
notesUI *ui;
  ui = (notesUI*)userdata;
  // if (ui->changed()) are you sure ?
  noteUIs->close(ui);
}

void quitnotes_cb(Fl_Window*, void*userdata) {
notesUI *ui;
  ui = (notesUI*)userdata;
  // if (ui->changed()) are you sure ?
  noteUIs->close(ui);
}

void clearnotes_cb(Fl_Menu_*, void*userdata) {
notesUI *ui;
  ui = (notesUI*)userdata;
  // if ui->size() > 0 are you sure ?
  ui->clear();
}

void restorenotes_cb(Fl_Menu_*, void*userdata) {
notesUI *ui;
  ui = (notesUI*)userdata;
  ui->restore();
}

void savenotes_cb(Fl_Menu_*, void*userdata) {
notesUI *ui;
  ui = (notesUI*)userdata;
  ui->update();
}

void oknotes_cb(Fl_Menu_*, void*userdata) {
notesUI *ui;
  ui = (notesUI*)userdata;
  ui->update();
  noteUIs->close(ui);
}


