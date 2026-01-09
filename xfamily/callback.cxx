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
#include <Fl/Fl_File_Chooser.H>

#include "fixes.h"

#include <FL/Fl_Image.H>

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "menucodes.h"
#ifdef fix0021
#include "flags.h"
#endif

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
#include "write_png.h"

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

  // eventually we won't set the filename here, as we'll use what it's already set to
  tree->setfilename("/var/test.ged");
  tree->save();

}

#ifdef fix0013
void saveas_cb( Fl_Menu_ *, void *userdata) {

  printf("Callback saveas_cb\n");
  mainUI *view = (mainUI*)userdata;
  treeinstance *tree = view->whichtree();

  // need to make the dialogue title translatable
  Fl_File_Chooser chooser(".","*.ged",Fl_File_Chooser::CREATE,"Save tree as");
  chooser.preview(0);
  chooser.show();
  while (chooser.shown())
    { Fl::wait(); }
  if (chooser.count()!=1) return;
  printf("Filename (full) %s\n",chooser.value());
  tree->setfilename((char*)chooser.value());
  tree->save();

}
#endif

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
indidisplay *indid = NULL;
famdisplay  *famd = NULL;

Fl_Menu_Item *picked;
  x = Fl::event_x();
  y = Fl::event_y();
  button = Fl::event_button();
  //printf("button clicked was %d.\n", button);
  // view->main is the treedisplay, so whoisat is implemented in display.cxx
  clicked = view->main->whoisat(x,y,indid,famd);
  //printf("popup_cb getting clicked pointer %ld\n",(long)clicked);
  // what we get back there is the actual INDI or FAM object, not the indidisplay
  // or famdisplay

  if (button == 1) {
    if (clicked == NULL) return; // as opposed to a segmentation fault, for example...
     if (clicked->objtype() == INDI_tag) {
      view->setcurrent( clicked );
      //return;
    }
    return;
  }
  // so now have a right (3) or middle (2) click - currently treat both the same, but eventually decide
  // on the basis of a look-and-feel selector (RISCOS vs Windows/Linux) which will do menu

  // given a menu-requesting-click, we need to decide, on the basis of where we were clicked,
  // which menu to open, view->indimenu or view->fammenu or view->genmenu

  if (clicked == NULL) {
  // a generic popup to allow things like newperson, goto ... save screen
    view->whichtree()->setevent( clicked );
    // FIXME suspect we don't actually need that, which is a fucntion we are deprecating
    // and we might want something more meaningful for this:
    sprintf(person_buffer,"Menu");
    picked=(Fl_Menu_Item*)((Fl_Menu_Item*)view->genmenu)->popup(x-40,y-10,person_buffer,(Fl_Menu_Item*)NULL,(Fl_Menu_*)NULL);
    if (picked==NULL) return;
    if (picked->callback_==NULL) return;
    // for this one we need to know which view, specifically
    picked->do_callback((Fl_Widget*)picked,view);
    return;
  }

  if (clicked->objtype() == INDI_tag) {
    // printf("popup_cb - an INDI at %ld\n", (long)clicked);
    //sprintf(person_buffer,"%s x %d: y %d",clicked->subobject( NAME_tag )->value(),x,y);
    sprintf(person_buffer,"%s",clicked->subobject( NAME_tag )->value());

    view->whichtree()->setevent( clicked );
    view->whattodraw()->setevent( (void*)indid);

    // need to (a) find some things out about the particular INDIdisplay
    // so we can diagnose whether to grey out some menu options (eg. 'younger')
    // then need some vague idea of how to grey them out !

    // printf("popup_cb believes we clicked on an INDI, and the indidisplay is %ld, %s\n",(long)indid,indid->name());
    if (indid->younger_valid()) {
      printf("younger is OK\n");
      view->indimenu->black(imenuyounger);
    } else {
      printf("younger greyed\n");
      view->indimenu->grey(imenuyounger);
    }
#ifdef fix0015
    // if indi has more than one spouse shown, we want to grey out "child"
    if (indid->child_valid()) {
      printf("child OK\n");
      view->indimenu->black(imenuchild);
    } else {
      printf("child greyed\n");
      view->indimenu->grey(imenuchild);
    }
    if (indid->newfam_valid()) {
      printf("new family OK\n");
      view->indimenu->black(imenunewfam);
    } else {
      printf("new family greyed\n");
      view->indimenu->grey(imenunewfam);
    }
#endif

    // need cast to discard const-ness 
    picked=(Fl_Menu_Item*)((Fl_Menu_Item*)view->indimenu)->popup(x-40,y-10,person_buffer,(Fl_Menu_Item*)NULL,(Fl_Menu_*)NULL);
    if (picked==NULL) return;
    if (picked->callback_==NULL) return;
    picked->do_callback((Fl_Widget*)picked,view);
    return;
  }
  if (clicked->objtype() == FAM_tag) {
    sprintf(person_buffer,"%s",clicked->getidname());
    view->whichtree()->setevent( clicked );

    // need to (a) find some things out about the particular FAMdisplay
    // so we can diagnose whether to grey out some menu options (eg. 'later')
    // then need some vague idea of how to grey them out !
    // printf("popup_cb believes we clicked on a FAM, and the famdisplay is %ld, %s\n",(long)famd,famd->marrlabel());
    if (famd->later_valid()) {
      // printf("later is OK\n");
      view->fammenu->black(fmenulater);
    } else {
      // printf("later greyed\n");
      view->fammenu->grey(fmenulater);
    }
    view->whattodraw()->setevent( (void*)famd);

    // need cast to discard const-ness 
    picked=(Fl_Menu_Item*)((Fl_Menu_Item*)view->fammenu)->popup(x-40,y-10,person_buffer,(Fl_Menu_Item*)NULL,(Fl_Menu_*)NULL);
    if (picked==NULL) return;
    if (picked->callback_==NULL) return;
    picked->do_callback((Fl_Widget*)picked,view);
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
#ifdef fix0022
  GEDCOM_id *xref;
#endif
  //menu->value(0);
  // but we do need to know the instanciation of the mainUI we are being
  // called on, so we can find out whom to edit !! (this should have been
  //mostly done by popup_cb, who found out where the mouse was clicked
  // and should have put a title on the menu to say who we would edit)
  mainUI* view = (mainUI*)userdata;
  treeinstance* tree = view->whichtree();
  displaytree* clickedtree = view->whattodraw();
  indidisplay* eventindi = (indidisplay*)clickedtree->getevent();
#ifdef fix0022
  neweditbox = editUIs->checkopen(tree, eventindi->getperson());
  if (neweditbox!=NULL) return;
  // create all ephemerals needed
  GEDCOM_object *ephem;
  GEDCOM_object *person=eventindi->getperson();
  GEDCOM_object *fam=person->parental_family();
  if (fam==NULL) {
    // create a temporary FAM object on this treeinstance:
    fam = new GEDCOM_object( xref = new GEDCOM_id("E",tree->ephemnextref()), FAM_tag );
    xref->GEDCOM_setobjectforid( fam );
 #ifdef fix0021
    fam->setflags(ged_ephemeral,0);
 #endif
    tree->add_ephemera( fam );
    printf("Added ephemeral FAM object\n");
 #ifdef fix0021
    person->add_subobject( ephem = new GEDCOM_object( GEDCOM_tagfromname( "FAMC" ), xref ));
    ephem->setflags( ged_ephemeral,0 );
    fam->add_subobject( ephem = new GEDCOM_object( GEDCOM_tagfromname( "CHIL" ), person->getid() ));
    ephem->setflags( ged_ephemeral,0 );
  }
 #else
    person->add_subobject( new GEDCOM_object( GEDCOM_tagfromname( "FAMC" ), xref ));
    fam->add_subobject( new GEDCOM_object( GEDCOM_tagfromname( "CHIL" ), person->getid() ));
  }
 #endif
  GEDCOM_object *parent =  fam->thewife();
  if (parent==NULL) {
    // create a temporary INDI object on this treeinstance as our mother
    parent = new GEDCOM_object( xref = new GEDCOM_id("E",tree->ephemnextref()), INDI_tag );
    xref->GEDCOM_setobjectforid( parent );
 #ifdef fix0021
    parent->setflags( ged_ephemeral,0 );
    parent->add_subobject( ephem = new GEDCOM_object( FAMS_tag, fam->getid()));
    ephem->setflags( ged_ephemeral,0 );
 #else
    parent->add_subobject( new GEDCOM_object( FAMS_tag, fam->getid()));
 #endif
    printf("%s\n",xref->GEDCOM_idname());
    tree->add_ephemera( parent );
    // create a WIFE subobject of the fam to point to it
 #ifdef fix0021
    fam->add_subobject( ephem = new GEDCOM_object( GEDCOM_tagfromname( "WIFE" ), xref ));
    ephem->setflags( ged_ephemeral,0 );
 #else
    fam->add_subobject( new GEDCOM_object( GEDCOM_tagfromname( "WIFE" ), xref ));
 #endif
  }
  parent = fam->thehusband();
  if (parent==NULL) {
    // create a temporary INDI object on this treeinstance as our father
    parent = new GEDCOM_object( xref = new GEDCOM_id("E",tree->ephemnextref()), INDI_tag );
    xref->GEDCOM_setobjectforid( parent );
 #ifdef fix0021
    parent->setflags( ged_ephemeral,0 );
    parent->add_subobject( ephem = new GEDCOM_object( FAMS_tag, fam->getid()));
    ephem->setflags( ged_ephemeral,0 );
 #else
    parent->add_subobject( new GEDCOM_object( FAMS_tag, fam->getid()));
 #endif
    printf("%s\n",xref->GEDCOM_idname());
    // create a HUSB subobject of the fam to point to it
 #ifdef fix0021
    fam->add_subobject( ephem = new GEDCOM_object( GEDCOM_tagfromname( "HUSB" ), xref ));
    ephem->setflags( ged_ephemeral,0 );
 #else
    fam->add_subobject( new GEDCOM_object( GEDCOM_tagfromname( "HUSB" ), xref ));
 #endif
  }
  
  neweditbox = editUIs->open(tree, person);
#else
  neweditbox = editUIs->open(tree, eventindi->getperson());
#endif
}

#ifdef fix0022
void childnew_cb(Fl_Menu_ *menu, void *userdata) {
// called to create a child in a new family of which eventindi->getperson() is one spouse
  GEDCOM_id *xref;
  GEDCOM_object *parent;
  GEDCOM_object *spouse;
  GEDCOM_object *check;
  char *val;
  indiUI *neweditbox;
  mainUI* view = (mainUI*)userdata;
  treeinstance* tree = view->whichtree();
  displaytree* clickedtree = view->whattodraw();
  indidisplay* eventindi = (indidisplay*)clickedtree->getevent();
  // need to create ephemeral indi object for new child, and an ephemeral fam object
  // with the eventindi->getperson() as the gender-correct spouse. The other spouse
  // will need to be a second ephemeral indi, of the opposite gender
  // create all ephemerals needed
  GEDCOM_object *ephem;
  GEDCOM_object *person = new GEDCOM_object( xref = new GEDCOM_id("E",tree->ephemnextref()), INDI_tag );
  xref->GEDCOM_setobjectforid( person );
#ifdef fix0021
  person->setflags(ged_ephemeral,0);
#endif
  tree->add_ephemera( person );
  printf("Added ephemeral INDI object\n");
  GEDCOM_object *fam = new GEDCOM_object( xref = new GEDCOM_id("E",tree->ephemnextref()), FAM_tag );
      xref->GEDCOM_setobjectforid( fam );
#ifdef fix0021
  fam->setflags(ged_ephemeral,0);
#endif
  tree->add_ephemera( fam );
  printf("Added ephemeral FAM object\n");
  person->add_subobject( new GEDCOM_object( FAMC_tag, fam->getid()));
  fam->add_subobject( ephem = new GEDCOM_object( CHIL_tag, person->getid()));
  ephem->setflags( ged_ephemeral,0 );
  parent = eventindi->getperson();
  parent->add_subobject( ephem = new GEDCOM_object( FAMS_tag, fam->getid()));
  ephem->setflags( ged_ephemeral,0 );
  // we can only raise this menu item on a gendered person, so this will always work
  val = parent->subobject( SEX_tag )->value();
  printf("Testing gender of single parent: %s\n",val);
  printf("Check name of single parent: %s\n",parent->subobject( NAME_tag )->value());
  switch (*val) {
    case 'F': {
      fam->add_subobject( ephem = new GEDCOM_object( WIFE_tag, parent->getid()));
      spouse = new GEDCOM_object( xref = new GEDCOM_id("E",tree->ephemnextref()), INDI_tag );
      xref->GEDCOM_setobjectforid( spouse );
#ifdef fix0021
      spouse->setflags(ged_ephemeral,0);
#endif
      fam->add_subobject( new GEDCOM_object( HUSB_tag, spouse->getid()));
      spouse->add_subobject( new GEDCOM_object( FAMS_tag, fam->getid()));
      spouse->add_subobject( new GEDCOM_object( SEX_tag, "M" ));
      fam->print(0);
      person->print(0);
      parent->print(0);
      spouse->print(0);
      break;
    }
    case 'M': {
      fam->add_subobject( ephem = new GEDCOM_object( HUSB_tag, parent->getid()));
      spouse = new GEDCOM_object( xref = new GEDCOM_id("E",tree->ephemnextref()), INDI_tag );
      xref->GEDCOM_setobjectforid( spouse );
#ifdef fix0021
      spouse->setflags(ged_ephemeral,0);
#endif
      fam->add_subobject( new GEDCOM_object( WIFE_tag, spouse->getid()));
      spouse->add_subobject( new GEDCOM_object( FAMS_tag, fam->getid()));
      spouse->add_subobject( new GEDCOM_object( SEX_tag, "F" ));
      fam->print(0);
      person->print(0);
      parent->print(0);
      spouse->print(0);
      break;
    }
  }
  neweditbox = editUIs->open(tree, person);
}
#endif

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
    // potentially, we might find a "1 DEAT yes" (although we have no ui to create that)
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
  mainUI* view = (mainUI*)userdata;
  treeinstance* tree = view->whichtree();
  displaytree* clickedtree = view->whattodraw();
  famdisplay* eventfam = (famdisplay*)clickedtree->getevent();
  // we need to get a pointer to our particular view, and then use
  // the x and y position to see who the menu click was over ...
  // then fill in all the details of the person. indi_ui probably needs a
  // member of type GEDCOM_object* (or indeed, Indi*) to say who we are
  // editing (NULL for a new person yet to be created).
  newfambox = famUIs->open(tree, eventfam->getfamily());
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
// callback routines for genpopupmenu

extern uchar* flocal_read_image( uchar*, int, int, int, int, int );

void savescr_cb(Fl_Menu_ *menu, void *userdata) {
  uchar *data_p = NULL;
  mainUI* view = (mainUI*)userdata;
  // we apparently need to force the window to be current in order to be
  // able to create an Fl_Offscreen which X understands
  view->main->window()->make_current();
  int os_w = view->canvas_w();
  int os_h = view->canvas_h();
  data_p = (uchar*)malloc(4*((os_w+3)&0xfffffffc)*((os_h+3)&0xfffffffc));
#ifdef debugging
  printf("Expecting to save an area %d x %d, into %ld bytes malloc'ed at %ld\n",os_w,os_h,4*((os_w+3)&0xfffffffc)*((os_h+3)&0xfffffffc),(long)data_p);
#endif
  if (data_p==NULL) {
    printf("Can't allocate memory to save window as graphic\n");
    return;
  }

// We can't just save from our window/canvas as it's not double-buffered and
// the bit map we'd want simply doesn't exist. We have to create an offscreen
// buffer of the canvas size, make it the target of drawing, call the code that
// draws our tree, with the clip rectangle set to the full size, and only then
// call fl_read_image. Then untarget drawing to the offscreen and delete it.

  Fl_Offscreen saver=fl_create_offscreen(os_w,os_h);
  printf("Created offscreen buffer %ld\n",(long)saver);
  fl_begin_offscreen(saver);
  view->main->drawoffscreen();

  fl_read_image(data_p, 0, 0, os_w, os_h, 0);
//  uchar* data_p_returned;
//  data_p_returned = flocal_read_image(data_p, 50, 50, 50, 50, 0);
#ifdef debugging
//  printf("Got an image at %ld\n",(long)data_p_returned);
#endif

#ifdef fix0013
  Fl_File_Chooser chooser(".","*.png",Fl_File_Chooser::CREATE,"Save tree as");
  chooser.preview(0);
  chooser.show();
  while (chooser.shown())
    { Fl::wait(); }
  if (chooser.count()==1) {
    write_png(chooser.value(), data_p, os_w, os_h, 3);
  }
#else
  write_png("Test1.png", data_p, os_w, os_h, 3);
#endif
  fl_end_offscreen();
  fl_delete_offscreen( saver );

#ifdef debugging
  printf("Wrote the image as a .png\n");
#endif
  free(data_p);
}

////////////////////////////////////////////////////////////////////////////////
//
// callback routines for statsUI

void stats_cb(Fl_Menu_ *, void *userdata) {
  mainUI*  view = (mainUI*)userdata;
  treeinstance* instanciation = view->whichtree();
//  treeinstance* instanciation = (treeinstance*)userdata;
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
  mainUI* view = (mainUI*)userdata;
  treeinstance* tree = view->whichtree();
  // the code which popped up the menu should have called setevent for
  // the relevant object, so all we will need to do is look at the value

// in order to eliminate geteventobject() on the treeinstance, we want to
// use event=view->whattodraw()->getevent() and then one of event->getfamily()
// or event->getperson(). However we can't tell what sort of pointer we get
// back from getevent(), as our classes aren't properly polymorphic (we just
// cast to void* and back...) FIXME

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

void menu_younger_cb(Fl_Menu_ *menu, void* userdata) {

  // the idiom is such that we shouldn't really allow this operation unless
  // the whole FAM is in the displaytree - this has already been checked
  // and the menu item 'younger' greyed out if it's not a valid operation

  mainUI*  view = (mainUI*)userdata;
  displaytree* clickedtree = view->whattodraw();
  treeinstance* tree = view->whichtree();
  GEDCOM_object *sibling = tree->geteventobject();
  sibling->print( 0 );
  // find a FAMC subobject of sibling - there can only ever be one
  //  if there isn't one, then we can't be made older - whinge/exit
  GEDCOM_object *thefamc = sibling->subobject( FAMC_tag );
  // should not be possible - menu item is greyed out for this case:
  if (thefamc == NULL ) {
    printf("Can't find a FAMC for that INDI\n");
    return; // whinge we are not a child in any family
  }
  thefamc->print( 0 );
  GEDCOM_object *thefam = thefamc->followxref();
  if (thefam == NULL) {
    printf("Can't follow FAMC ref to a FAM\n");
    return; // we are very broken if this happens
  }
  thefam->print( 0 );
  // indirect to the relevant FAM object. find the first CHIL subobject.
  GEDCOM_object *chil1 = thefam->subobject( CHIL_tag );
  GEDCOM_object *chil2;
  while ((chil1->followxref())!=sibling) {
    chil1 = chil1->next_object (CHIL_tag);
    if (chil1==NULL) {
      printf("None of CHIL tags in FAM points back to starting INDI\n");
      return;
    }
  }
  chil2 = chil1->next_object( CHIL_tag );
  // this should not be possible - the menu item is greyed out if we are
  // already youngest but bombproof error-checking does not real harm ;-)
  if (chil2 == NULL) {
    printf("That's already the youngest child\n");
    return; // whinge that we are already the youngest
  }
  if (!thefam->swap_subobject( chil1, chil2 )) {
    printf("swap_subobject returned false - something broke !\n");
  }
  printf("Should now be swapped\n");
  thefam->print( 0 );

  // at this point we need to rebuild the displaytree and redraw.
  view->newdisplay();
  view->window->redraw();
  while ( (view=(view->getnext()))!=NULL ) {
    view->newdisplay();
    view->window->redraw();
  }
  // that recalculates display, and forces a redraw. Often, that is enough, but
  // sometimes the redrawn tree will be such that the person we just made younger
  // is no longer visible. We ought to check for that (if they are visible in the
  // window at all, we shouldn't scroll, but if they are now entirely out of the
  // window, we should centre them).
  
}

void menu_later_cb(Fl_Menu_ *menu, void* userdata) {

  mainUI*  view = (mainUI*)userdata;
  displaytree* clickedtree = view->whattodraw();
  treeinstance* tree = view->whichtree();
  // so we need to get back to the INDI that has this family, and swap two
  // FAMS objects. Of course, there are two INDIs, and we want the one that
  // is uppermost in the displayed tree
  famdisplay* eventfam = (famdisplay*)clickedtree->getevent();
  GEDCOM_object* family = eventfam->getfamily();
  printf("eventfam->getfamily() returns %ld\n",(long)family);
  GEDCOM_object* person = eventfam->getindi();
  printf("Trying to swap FAMS objects for \n");
  person->print(0);
  GEDCOM_object *fams1=person->subobject( FAMS_tag );
  GEDCOM_object *fams2;
  while ((fams1->followxref())!=family) {
    fams1 = fams1->next_object (FAMS_tag);
    if (fams1==NULL) {
      printf("None of FAMS tags in INDI points back to starting FAM\n");
      return;
    }
  }
  fams2 = fams1->next_object( FAMS_tag );
  // this should not be possible - the menu item is greyed out if we are
  // already latest but bombproof error-checking does not real harm ;-)
  if (fams2 == NULL) {
    printf("That's already the latest marriage\n");
    return; // whinge that we are already the latest
  }
  if (!person->swap_subobject( fams1, fams2 )) {
    printf("swap_subobject returned false - something broke !\n");
  }
  printf("Should now be swapped\n");
  person->print( 0 );
  view->newdisplay();
  view->window->redraw();
  while ( (view=(view->getnext()))!=NULL ) {
    view->newdisplay();
    view->window->redraw();
  }
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
