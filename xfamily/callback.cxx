// callback.cxx

#include <FL/Fl.H>
#include <FL/Fl_Menu.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Help_Dialog.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Multiline_Input.H>

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "fixes.h"

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

static Fl_Help_Dialog *help_dialog = 0;

///////////////// callback routines for the GUI classes ///////////////

// callback routines to attach to buttons and menus that don't work yet

void deadbutton_cb( Fl_Button*, void *) {
}

void deadmenu_cb( Fl_Menu_*, void *) {
}

// callback routines from mainUI menu applying to whole program

void language_cb(Fl_Menu_ *, void *index) {
  /* 2006-12-23 hacked to get an int from what we create in language.cxx, which is (void*)(0) etc. */
  xlate_messages(languages[(int)*(int*)index]);
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
  //printf("incarnated new mainUI at %ld\n",(long)newview);
  tree->addview(newview);
  //printf("added it to tree\n");
  GEDCOM_object* tempfortest = view->getcurrent();
  //printf("getcurrent for existing view was %ld\n",(long)tempfortest);
  newview->setcurrent( tempfortest );
  //newview->setcurrent( view->getcurrent() );
  //printf("Set current\n");
  newview->show();
  //printf("and now you should see it !\n");
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

  if (clicked == NULL) { /*printf("popup_cb - no object\n");*/ return;} // eventually a generic popup to allow things like newperson, goto ...
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
  show_help( userdata );
}

void show_help( void* urlleaf ) {
  // urlleaf points to leafname of helpfile (which is const, as it happens)
  const char	*docdir;
  char		helpname[1024];
  
//  if (!help_dialog) help_dialog = new Fl_Help_View(0,0,600,400,"X!Family Help");
  if (!help_dialog) help_dialog = new Fl_Help_Dialog();
//  help_dialog->textfont(FL_HELVETICA);

  if ((docdir = getenv("XFAM_DOCDIR")) == NULL) {
    docdir = "/usr/local/src/xfamily/html015";
  }
  // we should have an extra /%s and add the language code
  // and when installed, our dir should be something like /usr/local/share/xfamily/help/html/<lang-code>
  snprintf(helpname, sizeof(helpname), "%s/%s", docdir, (char*)urlleaf);  

  help_dialog->load(helpname);
  help_dialog->show();
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

void commitedit_cb(Fl_Button *, void *userdata) {
// virtually the same as above (which we should therefore hive off into a
// separate routine), but without the close( indibox );
}

void restoreedit_cb(Fl_Button *, void *userdata) {
// basically do the same field-filling as on opening the indiUI.
// DON'T restore NOTE or SOUR stuff into any extant notesUI boxes ...
}

void canceledit_cb(Fl_Button *, void *userdata) {
indiUI* indibox = (indiUI*)userdata;
GEDCOM_object* edited = indibox->whois();
GEDCOM_object *event, *eventdata;
// just bin all the changes - which may include creation of ephemeral objects (fix0004)
// "changes" in this context also includes the creation of dummy events (within the
// main GEDCOM or ephemera) to hang NOTE objects from. We need to garbage collect any
// dead events (ie. ones with no subobjects). We need to be careful, in that there
// may be no BIRT, CHR, BAPM, DEAT, CREM or BURI objects at all, but there *may* be
// other subobjects for which we don't yet provide an interface....
// If there are still NOTE objects for any event (eg.
// if there are active notesUI dboxes open), they will persist.
  if ( (eventdata = (event=edited->subobject( BIRT_tag ))->subobject()) == NULL ) {
    // clearly, if it has no subobjects, we can remove it.
    //printf("BIRT event at %ld empty, deleting it\n", (long)event);
    edited->delete_subobject( event );
  }
  // there may still be a dummy NOTE object keeping the BIRT alive...
  if ( (eventdata = (event=edited->subobject( CHR_tag ))->subobject()) == NULL ) {
    // clearly, if it has no subobjects, we can remove it.
    //printf("CHR event at %ld empty, deleting it\n", (long)event);
    edited->delete_subobject( event );
  }
  if ( (eventdata = (event=edited->subobject( BAPM_tag ))->subobject()) == NULL ) {
    // clearly, if it has no subobjects, we can remove it.
    //printf("BAPM event at %ld empty, deleting it\n", (long)event);
    edited->delete_subobject( event );
  }
  if ( (eventdata = (event=edited->subobject( DEAT_tag ))->subobject()) == NULL ) {
    // clearly, if it has no subobjects, we can remove it.
    // potentially, we might find a "1 DEAT yes"
    //printf("DEAT event at %ld empty, deleting it\n", (long)event);
    edited->delete_subobject( event );
  }
  if ( (eventdata = (event=edited->subobject( CREM_tag ))->subobject()) == NULL ) {
    // clearly, if it has no subobjects, we can remove it.
    //printf("CREM event at %ld empty, deleting it\n", (long)event);
    edited->delete_subobject( event );
  }
  if ( (eventdata = (event=edited->subobject( BURI_tag ))->subobject()) == NULL ) {
    // clearly, if it has no subobjects, we can remove it.
    //printf("BURI event at %ld empty, deleting it\n", (long)event);
    edited->delete_subobject( event );
  }
  //printf("Survived deleting all the empty event objects\n");
  // can't just delete indibox, must remove it from the linked list:
  editUIs->close( indibox );
  //printf("Back from editUIs->close() in callback\n");
}

void helpedit_cb(Fl_Button *, void *userdata) {
// we need to call our routine to shell out to a help browser
// with URL file:/usr/share/xfamily/help/HTML/<language-code>/indiUI.html

  show_help( userdata );
}

void searchma_cb(Fl_Button *, void *userdata) {
// called to create a completions box, searching for a partially-typed mother's name
}

void searchpa_cb(Fl_Button *, void *userdata) {
// called to create a completions box, searching for a partially-typed father's name
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
GEDCOM_object* event;
GEDCOM_object* edited = fambox->whatis();
// just bin all the changes
  if ( (event = edited->subobject( ENGA_tag )) == NULL ) {
    // clearly, if it has no subobjects, we can remove it.
    edited->delete_subobject( event );
  }
  if ( (event = edited->subobject( MARR_tag )) == NULL ) {
    // clearly, if it has no subobjects, we can remove it.
    edited->delete_subobject( event );
  }
  if ( (event = edited->subobject( DIV_tag )) == NULL ) {
    // clearly, if it has no subobjects, we can remove it.
    edited->delete_subobject( event );
  }
// can't just delete fambox - must remove it from the linked list:
  famUIs->close( fambox );
}

void commitfam_cb(Fl_Button *, void *userdata) {
// virtually the same as above (which we should therefore hive off into a
// separate routine), but without the close( fambox );
}

void restorefam_cb(Fl_Button *, void *userdata) {
// basically do the same field-filling as on opening the famUI.
// DON'T restore NOTE or SOUR stuff into any extant notesUI boxes ...
}

void helpfam_cb(Fl_Button *, void *userdata) {
// we need to call our routine to shell out to a help browser
// with URL file:/usr/share/xfamily/help/HTML/<language-code>/famUI.html

  show_help( userdata );
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
  // currently just drop back - if this is because we typed some string that
  // doesn't match any real INDI, then the findUI window just stays open with
  // no feedback to say "not found".
}

void binnedfind_cb(Fl_Window*, void *userdata) {
  // make sure we didn't leave a completions window lying around
  completionsbox->finish();
  gotobox->finish();
}

void searchfind_cb(Fl_Button* button, void *userdata) {
  int strategy=0;
  findUI* searchfrom = (findUI*)userdata;
  short x,y;
  x = gotobox->findbox->x();
  y = gotobox->findbox->y();
  // this is inelegant - we aren't a method of findUI (although, logically, we should be,
  // but I don't think callbacks can be methods - or am I just igorrant ?? )
  if (button==searchfrom->find_fromright) strategy = 1;
  if (button==searchfrom->find_fuzzy)     strategy = 2;
  if (button==searchfrom->find_fromleft)  strategy = 3;
  completionsbox->open( searchfrom->getview()->whichtree(),
                        (char*)gotobox->find_input->value(),
                        strategy,
                        (Fl_Callback*) chosenfind_cb,
                        (void*) searchfrom->getview(), x+65, y+55 );
}

void chosenfind_cb( Fl_Select_Browser* chooser, void *userdata ) {
// userdata is (void*) of the mainUI class ... which for this callback
// is always going to be a mainUI (ie. a particular view). erm, see below...
  mainUI* view = (mainUI*)userdata;
  int chosen = chooser->value();
//  printf("completions selected item %d\n", chosen);
  GEDCOM_object* newperson = completionsbox->chosen_indi( chosen );
  completionsbox->finish();
  // anyone chosen in the completionsbox ought to be a real INDI, so we
  // should never get NULL here, but test anyway:-)
  // 2013-01-15: this is not going to work !! It's OK when the completions
  // window was opened in response to a View->GoTo menu item, but we also
  // use a completions window to fill in details of parents in an indiUI
  // so we need a way for the completions window to know what it should be
  // doing with newperson. But we can't just return the pointer - we are a
  // callback, so the caller is ... kinda weird ...
  if (newperson != NULL) {
    view->setcurrent(newperson);
    gotobox->findbox->hide();
  }
}

void completionbinned_cb( Fl_Window*, void *userdata ) {
//  printf("completions window closed with no choice made\n");
  completionsbox->finish();
}

void helpfind_cb(Fl_Button *, void *userdata) {
// we need to call our routine to shell out to a help browser
// with URL file:/usr/share/xfamily/help/HTML/<language-code>/findUI.html

  show_help( userdata );
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

  show_help( userdata );
}

////////////////////////////////////////////////////////////////////////////////
//
// callback routines to drive notesUI

// we call opennotes from one of the popup menus in a mainUI window
// eventually, the userdata is going to be a particular GEDCOM_object*
// as determined by the mainUI code based on the click position.
//
// are you sure ? The userdata is set up by the constructor of the menu,
// which is called by the constructor of the mainUI. So these menus can
// never know the GEDCOM_object on which they were raised. Unless you
// redefine the userdata for popup_data[1] each time we are popped up.
// It seems that setting the event object is actually cleaner :-(
//
// that was good news - because we couldn't get from a GEDCOM_object to the
// treeinstance it is part of, but we need to know the treeinstance
// in order to reflect the tree filename in the window title, and to mark
// the tree as modified if we eventually save changed notes back to a tree.

// this means that we need to keep a pointer in each tree to the
// last GEDCOM_object on which we processed an event. OK, that works.

// but we are going to start getting other callbacks from buttons in
// indiUI and famUI dboxes where the thing we pass is going to have
// to be a GEDCOM_object, so its lucky that we have now built code to
// enable us to get back to the treeinstance from there :-)

void menuopennotes_cb(Fl_Menu_ *menu, void *userdata) {
// opens a notesUI window for an object which gets a popup menu, ie. INDI or FAM
notesUI* newui;
  treeinstance* tree = (treeinstance*)userdata;
  // the code which popped up the menu should have called setevent for
  // the relevant object, so all we will need to do is look at the value
  newui = noteUIs->open(tree, tree->geteventobject(), NOTE_tag);
// noteUI->open() should do this:  newui->window->show();
}

void buttonopennotes_cb(Fl_Button *menu, void *userdata) {
// opens a notesUI window for any object for which we provide a "Notes" button
// in its editing dbox. This includes INDI and FAM, but also events, facts,
// sources and almost anything else you can edit.
notesUI* newui;
  // the dbox owning the button should set the userdata to void*(pointertoobject)
  // but the notes button on a indiUI for a <new person> will be NULL until saved
  if (userdata==NULL) return;
  // you might want to raise an Fl_Alert before doing that....
  // actually, we'd rather create a relevant GEDCOM_object to raise the
  // NOTE on, but at this point we are going to struggle, since without
  // an existing object, we can't tie back to the INDI or FAM on which to
  // create the event, nor know which event to create. So this problem needs
  // solving back in indiUI and famUI ...
  //printf("buttonopennotes_cb for %d\n",(int)userdata);
  treeinstance* tree = ((GEDCOM_object*)userdata)->root();
  //printf("notesUI to be raised on treeinstance %d\n",(int)tree);
  // the object we pass to the notes open is the INDI/FAM/event/etc. object
  newui = noteUIs->open(tree, (GEDCOM_object*)userdata, NOTE_tag);
// noteUI->open() should do this:  newui->window->show();
}

void buttonopentext_cb(Fl_Button *menu, void *userdata) {
// opens a notesUI window for a TEXT object for which we provide a "Text" button
// in its editing dbox. This mostly means SOUR, CENS, WILL, PROB and the like
notesUI* newui;
  if (userdata==NULL) return;
  treeinstance* tree = ((GEDCOM_object*)userdata)->root();
  newui = noteUIs->open(tree, (GEDCOM_object*)userdata, TEXT_tag);
// noteUI->open() should do this:  newui->window->show();
}

// for each callback, we have to work back to find the notesUI. That was
// why we set the menu item userdata to ((void*)(this)) in the constructor :-)
// all these methods apply equally to NOTE objects' and TEXT objects' notesUIs

void changenotes_cb(Fl_Multiline_Input*, void*userdata) {
notesUI *ui;
  ui = (notesUI*)userdata;
  ui->changed(true);
}

void cancelnotes_cb(Fl_Menu_*, void*userdata) {
notesUI *ui;
  ui = (notesUI*)userdata;
  // if (ui->changed()) are you sure ?
  //printf("cancelnotes_cb for notesUI at %ld\n",(long)userdata);
  noteUIs->close(ui);
}

void quitnotes_cb(Fl_Window*, void*userdata) {
notesUI *ui;
  ui = (notesUI*)userdata;
  //printf("quitnotes_cb for notesUI at %ld\n",(long)userdata);
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
  //printf("restorenotes_cb for notesUI at %ld\n",(long)userdata);
  ui->restore();
}

void savenotes_cb(Fl_Menu_*, void*userdata) {
notesUI *ui;
  ui = (notesUI*)userdata;
  //printf("savenotes_cb for notesUI at %ld\n",(long)userdata);
  ui->update();
  //printf("Done update from notesUI to GEDCOM\n");
}

void oknotes_cb(Fl_Menu_*, void*userdata) {
notesUI *ui;
  ui = (notesUI*)userdata;
  //printf("oknotes_cb for notesUI at %ld\n",(long)userdata);
  ui->update();
  //printf("Done update from notesUI to GEDCOM\n");
  noteUIs->close(ui);
}

////////////////////////////////////////////////////////////////////////////////
//
// callback routines for other Person menu clicks

void menu_newmarriage_cb(Fl_Menu_ *menu, void* userdata) {
// opens something like a famUI for a Person->Marry menu click

  treeinstance* tree = (treeinstance*)userdata;
  // the code which popped up the menu should have called setevent for
  // the relevant object, so all we will need to do is look at the value
  // something vaguely like:
  // newui = famUIs->open(tree, tree->geteventobject());
  // tree->getevenetobject should point to the relevant INDI, for which, as
  // yet, there should not be a relevant FAMS (well, there might be, but the
  // purpose of this code is to create a new one)
}

////////////////////////////////////////////////////////////////////////////////
//
// callback routines to drive sourceUI

// none of which is built yet, but we need dummy routines while we test the
// button layouts

void buttonopensourceui_cb( Fl_Button*, void*userdata) {
GEDCOM_object* caller;
  // printf("Arrived ok in buttonopensourceui_cb\n");
  caller = (GEDCOM_object*)userdata;
  // printf("Cast userdata to GEDCOM_object* OK\n");
  treeinstance* tree = ((caller = (GEDCOM_object*)userdata)->root());
  // then you'd need to open a sourceUI window ...
}
