// callback.cxx

#include "fixes.h"

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
#include <FL/fl_ask.H>

#include <FL/Fl_Image.H>

#include <string.h>
//#include <unistd.h>
#include <stdlib.h>

#include "menucodes.h"

#include "classes.h"
#include "family.h"
#include "structure.h"
#include "gui.h"
#include "display.h"
#include "trees.h"
#include "objects.h"
#include "callback.h"
#include "tags.h"

static Fl_Help_Dialog *help_dialog = 0;

///////////////// callback routines for the GUI classes ///////////////

// callback routines to attach to buttons and menus that don't work yet

void deadbutton_cb( Fl_Button*, void *) {
}

void deadmenu_cb( Fl_Menu_*, void *) {
}

// callback routines from mainUI menu applying to whole program

// since we removed the ability to change language on the fly - there are none

////////////////////////////////////////////////////////////////////////////////
//
// callback routines from mainUI menu applying to whole tree

void save_cb( Fl_Menu_ *, void *userdata) {

  mainUI *view = (mainUI*)userdata;
  treeinstance *tree = view->whichtree();
  char file[] = "test.ged";

  // eventually we won't set the filename here, as we'll use what it's already set to
  tree->setfilename(file);
  tree->save();

}

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
  treeinstance* tree=view->whichtree();
  if (fl_choice("This will close all open views - you may want to save first! Go ahead and quit?","OK","*No, panic, keep going",0)) {
    return;
  }
  exit(37);
}

// Shortly, destroying a single view will not exit as it does now, it will just
// remove the view. If it was the last view open (on all trees), it should pop
// up the "master control" UI

// we should also rename this to reflect which window is being killed, since
// we need to provide callbacks for every window ...

void killed_cb(Fl_Window*, void*userdata) {
mainUI *view = (mainUI*)userdata;
  // essentially identical to the above, except for the type of the first
  // parameter - this one is from the callback we get when a mainUI window
  // is closed by its close icon - until we do save/cancel/discard, its sole
  // function is to stop other windows being left open
  treeinstance* tree=view->whichtree();
  if ((tree->viewcount())==1) {
    if (fl_choice("Closing the final open view will quit the program - you may want to save first! Go ahead and quit?","OK","*No, panic, keep going",0)) return;
    exit(0);
  }
  tree->removeview(view);
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
  // what we get back in clicked is the actual INDI or FAM object,
  // but the indidisplay or famdisplay come back in indid or famid which are passed by reference

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
    // FIXME suspect we don't actually need that, which is a function we are deprecating
    // and we might want something more meaningful for this:
    sprintf_s(person_buffer, 256, "Menu");
    picked=(Fl_Menu_Item*)((Fl_Menu_Item*)view->genmenu)->popup(x-40,y-10,person_buffer,(Fl_Menu_Item*)NULL,(Fl_Menu_*)NULL);
    if (picked==NULL) return;
    if (picked->callback_==NULL) return;
    // for this one we need to know which view, specifically
    picked->do_callback((Fl_Widget*)picked,view);
    return;
  }

  if (clicked->objtype() == INDI_tag) {
    printf("popup_cb - an INDI at %ld\n", (long)clicked);
    //sprintf(person_buffer,"%s x %d: y %d",clicked->subobject( NAME_tag )->value(),x,y);
    GEDCOM_object* clickedname;
    if ((clickedname=(clicked->subobject( NAME_tag )))!=NULL) {
      sprintf_s(person_buffer,256,"%s",clickedname->value());
    } else {
      person_buffer[0] = '\0';
    }
    printf("person_buffer set to'%s'\n",person_buffer);
    view->whichtree()->setevent( clicked );
    view->whattodraw()->setevent( (void*)indid);

    // need to (a) find some things out about the particular INDIdisplay
    // so we can diagnose whether to grey out some menu options (eg. 'younger')
    // then need some vague idea of how to grey them out !

    printf("popup_cb believes we clicked on an INDI, and the indidisplay is %ld, %s\n",(long)indid,indid->name());
    if (indid->younger_valid()) {
      view->indimenu->black(imenuyounger);
    } else {
      view->indimenu->grey(imenuyounger);
    }
    if (indid->older_valid()) {
      view->indimenu->black(imenuolder);
    } else {
      view->indimenu->grey(imenuolder);
    }
    // this is a temporary kludge - an indidisplay doesn't know its treedisplay
    // so cannot tell if it is the top person - we have to test that ourselves
    if ((clicked==view->gettop())||(indid->newfam_valid())) {
      view->indimenu->black(imenunewfam);
    } else {
      view->indimenu->grey(imenunewfam);
    }

    // need cast to discard const-ness 
    picked=(Fl_Menu_Item*)((Fl_Menu_Item*)view->indimenu)->popup(x-40,y-10,person_buffer,(Fl_Menu_Item*)NULL,(Fl_Menu_*)NULL);
    if (picked==NULL) return;
    if (picked->callback_==NULL) return;
    picked->do_callback((Fl_Widget*)picked,view);
    return;
  }
  if (clicked->objtype() == FAM_tag) {
    sprintf_s(person_buffer,256,"%s",clicked->getidname());
    view->whichtree()->setevent( clicked );

    // need to (a) find some things out about the particular FAMdisplay
    // so we can diagnose whether to grey out some menu options (eg. 'later')
    // printf("popup_cb believes we clicked on a FAM, and the famdisplay is %ld, %s\n",(long)famd,famd->marrlabel());
    if (famd->later_valid()) {
      view->fammenu->black(fmenulater);
    } else {
      view->fammenu->grey(fmenulater);
    }
    if (famd->earlier_valid()) {
      view->fammenu->black(fmenuearlier);
    } else {
      view->fammenu->grey(fmenuearlier);
    }
    if (famd->unmarry_valid()) {
      view->fammenu->black(fmenuunmarry);
    } else {
      view->fammenu->grey(fmenuunmarry);
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

// this routine is called by any of the callbacks that respond to a help button.
// the userdata of each help button shuld be set to a leafname for a relevant URL
// which typically means ensuring that there is a suitable html file...
// Since there is a separate callback for each help button, we are wasting a
// load of boilerplate. If we set userdata for the button, then we should only
// need a single callback for all help buttons,and an extra call to this routine
// is not really adding any value...

void show_help( void* urlleaf ) {
  // urlleaf points to leafname of helpfile (which is const, as it happens)
  const char	*docdir;
  char		helpname[1024];
  
//  if (!help_dialog) help_dialog = new Fl_Help_View(0,0,600,400,"X!Family Help");
//  if (!help_dialog) help_dialog = new Fl_Help_Dialog();
//  help_dialog->textfont(FL_HELVETICA);

 // if ((docdir = getenv("XFAM_DOCDIR")) == NULL) {
 //   docdir = "/usr/local/src/xfamily/html015";
 // }
  // we should have an extra /%s and add the language code
  // and when installed, our dir should be something like /usr/local/share/xfamily/help/html/<lang-code>
//  snprintf(helpname, sizeof(helpname), "%s/%s", docdir, (char*)urlleaf);  

//  help_dialog->load(helpname);
//  help_dialog->show();
  fl_alert("The help subsystem is not available in the Windows port, but the documentation can be read just as easily in a browser");
}
/*  char file[] = "test.ged";
  tree->setfilename(file); */


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
  mainUI* view = (mainUI*)userdata;
  treeinstance* tree = view->whichtree();
  displaytree* clickedtree = view->whattodraw();
  indidisplay* eventindi = (indidisplay*)clickedtree->getevent();
  GEDCOM_object *person=eventindi->getperson();
  neweditbox = editUIs->checkopen(tree, person);
  if (neweditbox!=NULL) return;
//  GEDCOM_object *fam=person->parental_family();
  neweditbox = editUIs->open(tree, person);
#ifdef debugging
  printf("edit_cb neweditbox %ld, displaytree %ld, person %ld, indidisplay %ld, treeinstance %ld, view %ld\n",
    (long)neweditbox, (long)clickedtree, (long)person, (long)eventindi, (long)tree, (long)view );
#endif
}

void okedit_cb(Fl_Return_Button *, void *userdata) {

  indiUI* indibox = (indiUI*)userdata;
  printf("OKedit on indibox %ld\n",(long)indibox);
  GEDCOM_object* edited = indibox->whois();
  treeinstance* thisinstance = indibox->whichis();
  if (edited==NULL) {
    fl_alert("Called okedit_cb with a NULL edited person - broken !!\n");
    editUIs->close( indibox );
    return;
  }
  GEDCOM_object* test;
  GEDCOM_object* test2;
  indibox->update();
// if the ui was open to create a new spouse in a famUI, we need to update
// the contents of the famUI - indeed we update the contents of the spouse
// fields of any famUI open on any of the FAM objects which we have a FAMS for
  test = edited->subobject(FAMS_tag);
  while (test!=NULL) {
    printf("okedit test %ld\n",(long)test);
    famUI* fambox;
    if ((fambox = (famUIs->fambox(test->followxref())))!=NULL) {
      printf("okedit fambox %ld\n",(long)fambox);
      fambox->refresh_spouse();
    }
// similarly, if we were opened to create a new parent, we need to check for indi UI
// edit dboxes open on any INDI for which a FAMS points to a FAM with a CHIL pointer
// which points to an INDI with an open edit dbox
    indiUI* indibox;
    GEDCOM_object* FAMobject;
    FAMobject = test->followxref();
    FAMobject->output( stdout, 1 );
    test2 = FAMobject->subobject(CHIL_tag);
    while (test2!=NULL) {
      printf("okedit test2 %ld\n",(long)test2);
      if ((indibox = editUIs->editbox(test2->followxref()))!=NULL) {
        printf("Calling refresh_parents()\n");
        indibox->refresh_parents();
      }
      test2=test2->next_object(CHIL_tag);
    }
    test=test->next_object(FAMS_tag);
  }
  
// editUIs is the (global) list of currently open indiUIs, so close is a method on
// class editlist which is defined in structure.h/.cxx. All it does is remove this
// indibox from the linked list - nothing changes the GEDCOM structure here
  editUIs->close( indibox );
  // need to redraw every view which is open on this treeinstance
  printf("okedit_cb Redrawing every view which is open on this treeinstance %ld\n",(long)thisinstance);
  thisinstance->redraw();
  printf("okedit_cb Redrawn every view which is open on this treeinstance %ld\n",(long)thisinstance);
}

// **** need to refactor this to avoid boilerplate. okedit_cb should just run the code
// which is almost all of commitedit_cb, and then close the indibox, whilst commitedit_cb
// should do that common code and then do the indibox->whichis()->redraw();

void commitedit_cb(Fl_Button *, void *userdata) {
  indiUI* indibox = (indiUI*)userdata;
  GEDCOM_object* edited = indibox->whois();
  treeinstance* thisinstance = indibox->whichis();
  GEDCOM_object* test;
  GEDCOM_object* test2;
  test = edited->subobject();
  indibox->update();                    // does this do anything useful ? I think yes - we are not updating the indibox, we are updating
                                        // changed data in the indibox into the actual INDI object - which is the whole point of editing !
  test = edited->subobject(FAMS_tag);
  while (test!=NULL) {
    famUI* fambox;
    if ((fambox = (famUIs->fambox(test->followxref())))!=NULL) {
      printf("Calling refresh_parents()\n");
      fambox->refresh_spouse();
    }
// similarly, if we were opened to create a new parent, we need to check for indi UI
// edit dboxes open on any INDI for which a FAMS points to a FAM with a CHIL pointer
// which points to an INDI with an open edit dbox
    indiUI* indibox;
    GEDCOM_object* FAMobject;
    FAMobject = test->followxref();
    FAMobject->output( stdout, 1 );
    test2 = FAMobject->subobject(CHIL_tag);
    while (test2!=NULL) {
      printf("okedit test2 %ld\n",(long)test2);
      if ((indibox = editUIs->editbox(test2->followxref()))!=NULL) {
        printf("Calling refresh_parents()\n");
        indibox->refresh_parents();
      }
      test2=test2->next_object(CHIL_tag);
    }
    test=test->next_object(FAMS_tag);
  }
  // need to redraw every view which is open on this treeinstance
  printf("commitedit_cb Redrawing every view which is open on this treeinstance %ld\n",(long)thisinstance);
  thisinstance->redraw();
  printf("commitedit_cb Redrawn every view which is open on this treeinstance %ld\n",(long)thisinstance);
}

// refactor common code in okedit and commitedit void saveedit(indiUI *indibox) {
// but wait - first check if we can identify which button was pressed from the Fl_Button * we are being
// passed. If so, we could just have a single callback routine, save the data, and check whether to
// close the indibox on the basis of whether we got Apply or OK...

/* deprecate the Restore concept void restoreedit_cb(Fl_Button *, void *userdata) {
// basically do the same field-filling as on opening the indiUI.
// DON'T restore NOTE or SOUR stuff into any extant notesUI boxes ...
} */

void canceledit_cb(Fl_Button *, void *userdata) {
indiUI* indibox = (indiUI*)userdata;
GEDCOM_object* edited = indibox->whois();
GEDCOM_object *event, *eventdata, *object;
// just bin all the changes - although we won't back out the creation of other objects
// which may have been created with the "Add" parent button(s).
// "changes" in this context also includes the creation of dummy events (within the
// main GEDCOM or ephemera) to hang NOTE objects from. We need to garbage collect any
// dead events (ie. ones with no subobjects). We need to be careful, in that there
// may be no BIRT, CHR, BAPM, DEAT, CREM or BURI objects at all, but there *may* be
// other subobjects for which we don't yet provide an interface....
// If there are still NOTE objects for any event (eg.
// if there are active notesUI dboxes open), they will persist.
  event=edited->subobject( BIRT_tag );
  printf("BIRT subobject returned as %ld\n",event);
  if (event!=NULL) {
    if ( (eventdata = event->subobject()) == NULL ) {
      // clearly, if it has no subobjects, we can remove it.
      printf("BIRT event at %ld empty, deleting it\n", (long)event);
      edited->delete_subobject( event );
    }
  }
  // there may still be a dummy NOTE object keeping the BIRT alive...
  event=edited->subobject( CHR_tag );
  printf("CHR subobject returned as %ld\n",event);
  if (event!=NULL) {
    if ( (eventdata = event->subobject()) == NULL ) {
      // clearly, if it has no subobjects, we can remove it.
      printf("CHR event at %ld empty, deleting it\n", (long)event);
      edited->delete_subobject( event );
    }
  }
  event=edited->subobject( BAPM_tag );
  printf("BAPM subobject returned as %ld\n",event);
  if (event!=NULL) {
    if ( (eventdata = event->subobject()) == NULL ) {
      // clearly, if it has no subobjects, we can remove it.
      printf("BAPM event at %ld empty, deleting it\n", (long)event);
      edited->delete_subobject( event );
    }
  }
  event=edited->subobject( DEAT_tag );
  printf("DEAT subobject returned as %ld\n",event);
  if (event!=NULL) {
    if ( (eventdata = event->subobject()) == NULL ) {
      // clearly, if it has no subobjects, we can remove it.
      // potentially, we might find a "1 DEAT yes" (although we have no ui to create that)
      printf("DEAT event at %ld empty, deleting it\n", (long)event);
      edited->delete_subobject( event );
    }
  }
  event=edited->subobject( CREM_tag );
  printf("CREM subobject returned as %ld\n",event);
  if (event!=NULL) {
    if ( (eventdata = event->subobject()) == NULL ) {
      // clearly, if it has no subobjects, we can remove it.
      printf("CREM event at %ld empty, deleting it\n", (long)event);
      edited->delete_subobject( event );
    }
  }
  event=edited->subobject( BURI_tag );
  printf("BURI subobject returned as %ld\n",event);
  if (event!=NULL) {
    if ( (eventdata = event->subobject()) == NULL ) {
      // clearly, if it has no subobjects, we can remove it.
      printf("BURI event at %ld empty, deleting it\n", (long)event);
      edited->delete_subobject( event );
    }
  }
  printf("Survived deleting all the empty event objects\n");

  // can't just delete indibox, must remove it from the linked list:
  editUIs->close( indibox );
  GEDCOM_object *test=edited->subobject();
  int status = 0;
  int count = 0;
  GEDCOM_tag* tag;

  printf("Back from editUIs->close() in canceledit_cb\n");
}

/* void helpedit_cb(Fl_Button *, void *userdata) {
// we need to call our routine to shell out to a help browser
// with URL file:/usr/share/xfamily/help/HTML/<language-code>/indiUI.html

  show_help( userdata );
} */

void buttonaddindi_parent( char *gender, void *userdata) {
// 0 unknown (should never occur), 1 male, 2 female
GEDCOM_object* fam;
GEDCOM_id* fam_id;
// called to create a new INDI which will be pointed to by the WIFE/HUSB tag of
// a (possibly newly created) FAM object whose CHIL tag will point to the INDI
// we are editing. We will open an edit dbox for the new INDI.
  indiUI* indibox = (indiUI*)userdata;                   // this is the dbox in which user clicked "Add"
  treeinstance* tree = indibox->whichis();               // this is the tree in which that INDI resides
  //tree->dump();
  GEDCOM_object* indi = indibox->whois();                // this gets the INDI object
  //printf("buttonaddindi_parent entry context\n");
  //indi->print( 0 );
  GEDCOM_id* indi_id = indi->getid();                    // and that gets the ID (which we need for FAMS of the new INDI)
  indiUI *neweditbox;                                    // now open a new edit dbox for the parent we are creating
  int iid=tree->getnextindi();                           // and get the next available ID number
  char i[] = "I";
  GEDCOM_id* parent_id = new GEDCOM_id(i,iid);         // which we use to generate an "I<nnn>" ID string
  tree->add_id(parent_id);                               // this adds to a linked list of IDs ? check...
  GEDCOM_object* parent = new GEDCOM_object(parent_id,INDI_tag);    // this actually creates the new INDI object. It is not yet linked
  parent_id->GEDCOM_setobjectforid( parent );            // this adds the new ID to the INDI tag, so it is valid. Why did the constructor not do this ?
  parent->add_subobject(new GEDCOM_object(SEX_tag,gender)); // we can add a SEX tag, because we know which parent we are creating
  tree->freshindi( parent );                             // this adds the indi into the tree's linked list of INDI objects
  printf("buttonaddindi_parent inserted new parent into tree\n");
  // at this point we have created a new INDI but he/she has no context. If we let that
  // situation continue, then when user adds details, and saves from the edit dbox,
  // the new INDI will be fine, but won't be the parent we wanted.

  // if this is the second parent, there will already be a FAMC linking to a FAM, and that's
  // the FAM we need to use, not a new one, so check that first, otherwise create a new FAM
  GEDCOM_object* famc = indi->subobject(FAMC_tag);
  if (famc==NULL) {
    int fid=tree->getnextfam();                            // get the next available ID number
    printf("buttonaddindi_parent established context and will try FAM ID I%d\n",fid);
    char f[] = "F";
    fam_id = new GEDCOM_id(f,fid);                       // which we use to generate an "F<nnn>" ID string
    fam = new GEDCOM_object(fam_id,FAM_tag);               // create the new FAM
                                                           // we don't do an equivalent of tree->add_id(parent_id); for the fam id. Is this a bug ?
                                                           // I believe it is a bug, but don't think we actually use the linked list of IDs for anything
    tree->freshfam( fam );                                 // this adds the family into the tree's linked list of FAM objects
    fam_id->GEDCOM_setobjectforid( fam );       // as above, why did this not happen in the constructor anyway ?
    GEDCOM_object* famc = new GEDCOM_object(FAMC_tag,fam_id);
    indi->add_subobject(famc);
    // only need to add CHIL if its a new FAM
    GEDCOM_object* chil = new GEDCOM_object(CHIL_tag,indi_id);
    fam->add_subobject(chil);
  } else {
    //famc->print( 0 );
    fam_id = famc->getxref(); // this was our point of failure - needed getxref, not getid. 
    fam = famc->followxref();
  }
  // now we have both INDI and FAM objects, we need to link them via WIFE or HUSB tag in the FAM, a FAMS in the new parent INDI
  GEDCOM_object* spouse = new GEDCOM_object((*gender=='F')?WIFE_tag:HUSB_tag,parent_id);
  //printf("buttonaddindi_parent can we add the spouse ?\n");
  //spouse->print( 1 );
  //fam->print( 0 );
  fam->add_subobject(spouse);
  //printf("buttonaddindi_parent can we create FAMS ?\n");
  GEDCOM_object* fams = new GEDCOM_object(FAMS_tag,fam_id);
  //printf("buttonaddindi_parent can we add FAMS ?\n");
  //fams->print( 1 );
  //fam->print( 0 );
  //parent->print(0);
  parent->add_subobject(fams);
  // at this point the changes to the tree should be complete, but the edit indi dbox
  // will not reflect that and there will be no data in the new edit dbox for the parent.
  // Indeed, so far we haven't opened that dbox. Until the user saves the new edit dbox
  // for the parent, the original indi dbox won't have a name ...
  //printf("buttonaddindi_ma_cb pre-redraw context\n");
  //indi->print( 0 );
  //fam->print( 0 );
  //tree->dump();
  tree->redraw();
  neweditbox = editUIs->open(tree, parent);
  // now we have the edit open for the new parent, we need to update the original indi dbox:
  indibox->refresh_parents();
}

void buttonaddindi_ma_cb(Fl_Button*, void *userdata) {
  char f[] = "F";
  buttonaddindi_parent( f, userdata );
}

void buttonaddindi_pa_cb(Fl_Button*, void *userdata) {
  char m[] = "M";
  buttonaddindi_parent( m, userdata );
}

void buttonfindindi_fam_cb(Fl_Button*, void *userdata) {
// called to create a completions box, searching for a partially-typed parent's name
// so we can list matching FAM objects and attach that INDI to the one chosen.
// This is essentially the same functionality as the "Attach" menu item in the INDI
// context menu, but more visual and probably easier to understand (for the user).
  indiUI* indibox = (indiUI*)userdata;
  treeinstance* tree = indibox->whichis();
  int x = indibox->indi_dbox->x();
  int y = indibox->indi_dbox->y();
  famgotobox->open( userdata, x+50, y+50, 16 );
}

void famnew_cb(Fl_Menu_ *menu, void *userdata) {
// called to create a new FAM attached to an existing INDI as a FAMS
  famUI *newfambox;
  mainUI* view = (mainUI*)userdata;
  treeinstance* tree = view->whichtree();
  displaytree* clickedtree = view->whattodraw();
  indidisplay* eventindi = (indidisplay*)clickedtree->getevent();
  GEDCOM_object *person=eventindi->getperson();
  GEDCOM_id* indi_id = person->getid();
  // we will need to add a HUSB or WIFE tag to the FAM depending on the
  // gender of person and currnetly won't create a FAM if gender is unknown
  GEDCOM_tag* spouse_tag;
  int sex=eventindi->getgender();
  spouse_tag = NULL;
  switch (sex) {
    case SEX_MALE: spouse_tag=HUSB_tag; break;
    case SEX_FEMALE: spouse_tag=WIFE_tag; break;
    case SEX_UNKNOWN: fl_alert("Cannot start a family if spouse gender is unknown\n"); return;
  }
  int id=tree->getnextfam();
  char f[] = "F";
  GEDCOM_id* fam_id = new GEDCOM_id(f,id);
  GEDCOM_object* fam = new GEDCOM_object(fam_id,FAM_tag);
  tree->freshfam( fam );
  fam_id->GEDCOM_setobjectforid( fam );
  GEDCOM_object* fams = new GEDCOM_object(FAMS_tag,fam_id);
  person->add_subobject(fams);
  GEDCOM_object* spouse = new GEDCOM_object(spouse_tag,indi_id);
  fam->add_subobject(spouse);
  tree->redraw();
  // only after the redraw will there be a famdisplay for the new marriage
  // but the redraw will have rendered our indisdisplay invalid. However, the
  // famUI dbox is raised on the treeinstance, not the specific view
  newfambox = famUIs->open(tree, fam);
}

void deleteindi_cb(Fl_Menu_ *menu, void *userdata) {
// called to delete an INDI from the tree

// if this indi has FAMS tag(s) we need to follow to the FAM
// and remove the HUSB/WIFE tag, then garbage collect the FAM if there
// is neither HUSB nor WIFE left.
// if this INDI has a FAMC, we similarly need to follow to the FAM and
// remove the CHIL.
  bool neednewtop;
  mainUI* view = (mainUI*)userdata;
  treeinstance* tree = view->whichtree();
  displaytree* clickedtree = view->whattodraw();
  indidisplay* eventindi = (indidisplay*)clickedtree->getevent();
  GEDCOM_object *person=eventindi->getperson();
  if (person==(view->getcurrent())) {
    fl_alert("We advise clicking to choose a new current person first !\n");
    // this is not helpful if this is an isolated individual - there's no mechanism to delete someone you
    // can't see in a tree, so we need a mechanism to choose a new current person in this case.
    return;
  }
  neednewtop = (person==(view->gettop()));
  tree->remove_indi(person);
  if (neednewtop) {
    view->setcurrent(view->getcurrent());
  }
  tree->redraw();
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
  // in the 2017 move to implement read/write functionality it became
  // apparent that we should have creation and editing as separate operations
  // and NEVER call the famUI without a FAM object in the tree. So
  // getting here would now be an error.
    fl_alert("Called okfam_cb with a NULL edited person - broken !!\n");
    famUIs->close( fambox );
    return;
  }
  // then copy all that stuff into the relevant object
  fambox->update();
  printf("Done the update\n");
  famUIs->close( fambox );
  printf("About to call redraw on treeinstance %ld\n",(long)thisinstance);
  thisinstance->redraw();
}

void commitfam_cb(Fl_Button *, void *userdata) {

  famUI* fambox = (famUI*)userdata;
  GEDCOM_object* edited = fambox->whatis();
  fambox->update();
  fambox->whichis()->redraw();
  
}

void cancelfam_cb(Fl_Button *, void *userdata) {
famUI* fambox = (famUI*)userdata;
GEDCOM_object* event;
GEDCOM_object* edited = fambox->whatis();
// just bin all the changes
  event = edited->subobject( ENGA_tag );
  if (event!=NULL) {
    if ( event->subobject() == NULL ) {
      // clearly, if it has no subobjects, we can remove it.
      edited->delete_subobject( event );
    }
  }
  event = edited->subobject( MARR_tag );
  if (event!=NULL) {
    if ( event->subobject() == NULL ) {
      // clearly, if it has no subobjects, we can remove it.
      edited->delete_subobject( event );
    }
  }
  event = edited->subobject( DIV_tag );
  if (event!=NULL) {
    if ( event->subobject() == NULL ) {
      // clearly, if it has no subobjects, we can remove it.
      edited->delete_subobject( event );
    }
  }
// can't just delete fambox - must remove it from the linked list:
  famUIs->close( fambox );
}

/* deprecate the Restore concept void restorefam_cb(Fl_Button *, void *userdata) {
// basically do the same field-filling as on opening the famUI.
// DON'T restore NOTE or SOUR stuff into any extant notesUI boxes ...
} */

/* void helpfam_cb(Fl_Button *, void *userdata) {
// we need to call our routine to shell out to a help browser
// with URL file:/usr/share/xfamily/help/HTML/<language-code>/famUI.html

  show_help( userdata );
} */

void buttonaddhusb_cb(Fl_Button *, void *userdata) {
// called to create a new INDI attached to an existing FAM as HUSB
  famUI* fambox = (famUI*)userdata;
  GEDCOM_object* event;
  treeinstance* tree = fambox->whichis();
  GEDCOM_object* fam = fambox->whatis();
//  displaytree* clickedtree = view->whattodraw();
  indiUI *neweditbox;
  int id=tree->getnextindi();
  printf("buttonaddhusb_cb established context and will try ID I%d\n",id);
  char i[] = "I";
  GEDCOM_id* indi_id = new GEDCOM_id(i,id);
  tree->add_id(indi_id);
  GEDCOM_object* indi = new GEDCOM_object(indi_id,INDI_tag);
  indi_id->GEDCOM_setobjectforid( indi );
  tree->freshindi( indi );
  printf("buttonaddhusb_cb inserted new indi into tree\n");
  GEDCOM_object* husb = new GEDCOM_object(HUSB_tag,indi_id);
  fam->add_subobject(husb);
  printf("buttonaddhusb_cb inserted HUSB into fam\n");
  indi->add_subobject(new GEDCOM_object(FAMS_tag,fam->getid()));
  char m[] = "M";
  indi->add_subobject(new GEDCOM_object(SEX_tag,m));
  printf("buttonaddhusb_cb added FAMS and SEX tags to indi\n");
  tree->redraw();
  neweditbox = editUIs->open(tree, indi);
}

void buttonfindhusb_cb(Fl_Button *, void *userdata) {
// called to find an existing INDI to attach to an existing FAM as HUSB
  famUI* fambox = (famUI*)userdata;
  int x = fambox->fam_dbox->x();
  int y = fambox->fam_dbox->y();
  indigotobox->open( userdata, x+50, y+50, 4+16 );
}

void buttonremovehusb_cb(Fl_Button *, void *userdata) {
// called to detach an exissitng husband from a FAM
}

void buttonaddwife_cb(Fl_Button *, void *userdata) {
// called to create a new INDI attached to an existing FAM as WIFE
  famUI* fambox = (famUI*)userdata;
  GEDCOM_object* event;
  treeinstance* tree = fambox->whichis();
  GEDCOM_object* fam = fambox->whatis();
//  displaytree* clickedtree = view->whattodraw();
  indiUI *neweditbox;
  int id=tree->getnextindi();
  printf("buttonaddwife_cb established context and will try ID I%d\n",id);
  char i[] = "I";
  GEDCOM_id* indi_id = new GEDCOM_id(i,id);
  tree->add_id(indi_id);
  GEDCOM_object* indi = new GEDCOM_object(indi_id,INDI_tag);
  indi_id->GEDCOM_setobjectforid( indi );
  tree->freshindi( indi );
  printf("buttonaddwife_cb inserted new indi into tree\n");
  GEDCOM_object* wife = new GEDCOM_object(WIFE_tag,indi_id);
  fam->add_subobject(wife);
  printf("buttonaddwife_cb inserted WIFE into fam\n");
  indi->add_subobject(new GEDCOM_object(FAMS_tag,fam->getid()));
  char f[] = "F";
  indi->add_subobject(new GEDCOM_object(SEX_tag,f));
  printf("buttonaddhusb_cb added FAMS and SEX tags to indi\n");
  tree->redraw();
  neweditbox = editUIs->open(tree, indi);
}

void buttonfindwife_cb(Fl_Button *, void *userdata) {
// called to find an existing INDI to attach to an existing FAM as WIFE
  famUI* fambox = (famUI*)userdata;
  int x = fambox->fam_dbox->x();
  int y = fambox->fam_dbox->y();
  indigotobox->open( userdata, x+50, y+50, 8+16 );
}

void buttonremovewife_cb(Fl_Button *, void *userdata) {
// called to detach an exissitng wife from a FAM
}

void unmarry_cb(Fl_Menu_ *menu, void *userdata) {
// called to remove a FAM object - only permitted if there are no children
// 2022-02-19 that is no longer true - remove_fam did refuse to do that, but now it
// follows the CHIL tags and removes the FAMC tags from the children. That avoids
// breaking the GEDCOM, but is possibly overly destructive. One solution is to ask
// the user first (and allow remove_fam to be comprehensive).
// We need to think, what did the user think "Unmarry" was going to do ? Perhaps
// he thought it would remove one spouse ... We need an idiom for that.
// Now Unmarry is greyed except when a FAM has only a HUSB and a WIFE. We have
// added (but not yet implemented) an idiom to remove one spouse from a family
// with children
  mainUI* view = (mainUI*)userdata;
  treeinstance* tree = view->whichtree();
  displaytree* clickedtree = view->whattodraw();
  famdisplay* eventfam = (famdisplay*)clickedtree->getevent();
  GEDCOM_object* fam = eventfam->getfamily();
  GEDCOM_id* fam_id = fam->getid();
  tree->remove_fam( fam );
  tree->redraw();
}

void newchild_cb(Fl_Menu_ *menu, void *userdata) {
// called to create a new INDI attached to an existing FAM as a CHIL
  indiUI *neweditbox;
  mainUI* view = (mainUI*)userdata;
  treeinstance* tree = view->whichtree();
  displaytree* clickedtree = view->whattodraw();
  famdisplay* eventfam = (famdisplay*)clickedtree->getevent();
  GEDCOM_object* fam = eventfam->getfamily();
  GEDCOM_id* fam_id = fam->getid();
  int id=tree->getnextindi();
#ifdef debugging
  printf("newchild_cb displaytree %ld, famdisplay %ld, fam %ld, treeinstance %ld, view %ld, new id I%d\n",
    (long)clickedtree, (long)eventfam, (long)fam, (long)tree, (long)view, id );
#endif
  GEDCOM_object* parentforsurname;
  parentforsurname = fam->thehusband();
  if (parentforsurname==NULL) parentforsurname = fam->thewife();
  
  char i[] = "I";
  GEDCOM_id* indi_id = new GEDCOM_id(i,id);
  tree->add_id( indi_id);
  GEDCOM_object* indi = new GEDCOM_object(indi_id,INDI_tag);
  indi_id->GEDCOM_setobjectforid( indi );
  tree->freshindi( indi );
  GEDCOM_object* chil = new GEDCOM_object(CHIL_tag,indi_id);
#ifdef debugging
  printf("Adding CHIL %ld indi_id %ld @%s@\n", (long)chil,(long)indi_id,indi_id->GEDCOM_idname());
#endif
  fam->add_subobject(chil);
#ifdef debugging
  printf("Added CHIL %ld for fam %ld\n", (long)chil, (long)fam);
#endif
  GEDCOM_object* famc = new GEDCOM_object(FAMC_tag,fam_id);
#ifdef debugging
  printf("Adding FAMC %ld @%s@\n", (long)famc, fam->getidname());
#endif
  indi->add_subobject(famc);
#ifdef debugging
  printf("Added FAMC %ld for indi %ld\n", (long)famc, (long)indi);
#endif
  if (parentforsurname!=NULL) {
    GEDCOM_object* basename=parentforsurname->subobject(NAME_tag);
    if (basename!=NULL) {
      char* name=strdup(basename->value());
      char* surname=strtok(name,"/");
      surname = strtok(NULL,"/");
      if (surname!=NULL) {
        char slash[] = "/";
        GEDCOM_object* childname = new GEDCOM_object(NAME_tag,new GEDCOM_string(slash,surname,slash));
        indi->add_subobject(childname);
      }
    }
  }
  tree->redraw();
  // only after the redraw will there be an indisplay for the new child, but
  // the redraw will have rendered our famdisplay invalid. However, the indiUI dbox
  // is raised on the treeinstance, not the view, so we should not care about either
#ifdef debugging
  printf("Redrawn, now open indiUI on tree %ld, indi %ld\n",(long)tree, (long)indi);
#endif
  neweditbox = editUIs->open(tree, indi);
#ifdef debugging
  printf("newchild_cb neweditbox %ld, displaytree %ld, person %ld, treeinstance %ld, view %ld\n",
    (long)neweditbox, (long)clickedtree, (long)indi, (long)tree, (long)view );
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
// callback routines for genpopupmenu

extern uchar* flocal_read_image( uchar*, int, int, int, int, int );

void savescr_cb(Fl_Menu_ *menu, void *userdata) {
  uchar *data_p = NULL;
  mainUI* view = (mainUI*)userdata;
  fl_alert("screen image saving is not implemented in Windows, sorry");
  /*
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
    fl_alert("Can't allocate memory to save window as graphic\n");
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

  Fl_File_Chooser chooser(".","*.png",Fl_File_Chooser::CREATE,"Save tree as");
  chooser.preview(0);
  chooser.show();
  while (chooser.shown())
    { Fl::wait(); }
  if (chooser.count()==1) {
    write_png(chooser.value(), data_p, os_w, os_h, 3);
  }
  fl_end_offscreen();
  fl_delete_offscreen( saver );

#ifdef debugging
  printf("Wrote the image as a .png\n");
#endif
  free(data_p);
  */
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
// callback routines for indifindUI

void indifind_cb(Fl_Menu_*, void *userdata) {
  mainUI* view = (mainUI*)userdata;
  int x,y;
  x = view->window->x();
  y = view->window->y();

  // 12 is a bitfield indicating that strategy is to search for all genders (M+F+unknown)
  indigotobox->open( view, x+50, y+55, 12 );
}

void okindifind_cb(Fl_Return_Button*, void *userdata) {
  indifindUI* search = (indifindUI*)userdata;
  printf("Passing indifindUI OK click to search->OK()\n");
  search->OK();
}

void binnedindifind_cb(Fl_Window*, void *userdata) {
  // make sure we didn't leave a completions window lying around
  indicompletionsbox->finish();
  indigotobox->finish();
}

void searchindifind_cb(Fl_Button* button, void *userdata) {
  indifindUI* searchfrom = (indifindUI*)userdata;
  searchfrom->searchfind(button);
}

void chosenindifind_cb( Fl_Select_Browser* chooser, void *userdata ) {
  int chosen = chooser->value();
//  printf("completions selected item %d\n", chosen);
  GEDCOM_object* newperson = indicompletionsbox->chosen_indi( chosen );
  indicompletionsbox->finish();
  if (newperson != NULL) {
    indigotobox->setperson(newperson);
  }
}

void indicompletionbinned_cb( Fl_Window*, void *userdata ) {
//  printf("completions window closed with no choice made\n");
  indicompletionsbox->finish();
}

/* void helpindifind_cb(Fl_Button *, void *userdata) {
// we need to call our routine to shell out to a help browser
// with URL file:/usr/share/xfamily/help/HTML/<language-code>/indifindUI.html

  show_help( userdata );
} */

////////////////////////////////////////////////////////////////////////////////
//
// callback routines for famfindUI

void famfind_cb(Fl_Menu_*, void *userdata) {
  mainUI* view = (mainUI*)userdata;
  int x,y;
  x = view->window->x();
  y = view->window->y();

  // 12 is a bitfield indicating that strategy is to search for all genders (M+F+unknown)
  famgotobox->open( view, x+50, y+55, 12 );
}

void okfamfind_cb(Fl_Return_Button*, void *userdata) {
  // we think this userdata has not been set for the OK button, so this is a junk pointer 2022-03-04
  // but ->OK doesn't actually use it, because itknows there's just onefindfamUI which is global...
  famfindUI* search = (famfindUI*)userdata;
  printf("Passing famfindUI OK click to search->OK()\n");
  search->OK();
}

void binnedfamfind_cb(Fl_Window*, void *userdata) {
  // make sure we didn't leave a completions window lying around
  famcompletionsbox->finish();
  famgotobox->finish();
}

void searchfamfind_cb(Fl_Button* button, void *userdata) {
  famfindUI* searchfrom = (famfindUI*)userdata;
  searchfrom->searchfind(button);
}

void chosenfamfind_cb( Fl_Select_Browser* chooser, void *userdata ) {
  int chosen = chooser->value();
  printf("completions selected item %d\n", chosen);
// our first idea was that chosen_fam would return the pointer to the FAM object, but this
// doesn't define for us which spouse was the one matched. We *could* return the actual
// famcompletion_item*. That has the FAM pointer, and a string, but we could add a
// value which would tell us which spouse was matched. The completions box has a found_indi
  famcompletion_item* famchoice = famcompletionsbox->chosen_fam( chosen );
  //GEDCOM_object* newfam = famchoice->fam();
  char* gender = famchoice->gender();
  printf("chosenfamfind_cb has first spouse gender %s\n",gender);
  famgotobox->setfam(famchoice);
  famcompletionsbox->finish();
  printf("Thrown away completions window\n");
}

void famcompletionbinned_cb( Fl_Window*, void *userdata ) {
//  printf("completions window closed with no choice made\n");
  famcompletionsbox->finish();
}

/* void helpfamfind_cb(Fl_Button *, void *userdata) {
// we need to call our routine to shell out to a help browser
// with URL file:/usr/share/xfamily/help/HTML/<language-code>/famfindUI.html

  show_help( userdata );
} */

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

/*void helpchoices_cb(Fl_Button *, void *userdata) {
// we need to call our routine to shell out to a help browser
// with URL file:/usr/share/xfamily/help/HTML/<language-code>/prefUI.html

  show_help( userdata );
}*/

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

void changenotes_cb(Fl_Text_Editor*, void*userdata) {
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
  // we should now check if the NOTE we saved was empty, and if so delete it
  // completely, then call a process to check upwards...
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
  //sibling->print( 0 );
  // find a FAMC subobject of sibling - there can only ever be one
  //  if there isn't one, then we can't be made older - whinge/exit
  GEDCOM_object *thefamc = sibling->subobject( FAMC_tag );
  // should not be possible - menu item is greyed out for this case:
  if (thefamc == NULL ) {
    fl_alert("Can't find a FAMC for that INDI\n");
    return; // whinge we are not a child in any family
  }
  //thefamc->print( 0 );
  GEDCOM_object *thefam = thefamc->followxref();
  if (thefam == NULL) {
    fl_alert("Can't follow FAMC ref to a FAM\n");
    return; // we are very broken if this happens
  }
  //thefam->print( 0 );
  // indirect to the relevant FAM object. find the first CHIL subobject.
  GEDCOM_object *chil1 = thefam->subobject( CHIL_tag );
  GEDCOM_object *chil2;
  while ((chil1->followxref())!=sibling) {
    chil1 = chil1->next_object (CHIL_tag);
    if (chil1==NULL) {
      fl_alert("None of CHIL tags in FAM points back to starting INDI\n");
      return;
    }
  }
  chil2 = chil1->next_object( CHIL_tag );
  // this should not be possible - the menu item is greyed out if we are
  // already youngest but bombproof error-checking does not real harm ;-)
  if (chil2 == NULL) {
    fl_alert("That's already the youngest child\n");
    return; // whinge that we are already the youngest
  }
  if (!thefam->swap_subobject( chil1, chil2 )) {
    fl_alert("swap_subobject returned false - something broke !\n");
  }
  //thefam->print( 0 );

  // at this point we need to rebuild the displaytree and redraw.
  tree->redraw();
  // that recalculates display, and forces a redraw. Often, that is enough, but
  // sometimes the redrawn tree will be such that the person we just made younger
  // is no longer visible. We ought to check for that (if they are visible in the
  // window at all, we shouldn't scroll, but if they are now entirely out of the
  // window, we should centre them).
  
}

void menu_older_cb(Fl_Menu_ *menu, void* userdata) {

  // the idiom is as above

  mainUI*  view = (mainUI*)userdata;
  displaytree* clickedtree = view->whattodraw();
  treeinstance* tree = view->whichtree();
  GEDCOM_object *sibling = tree->geteventobject();
  //sibling->print( 0 );
  // find a FAMC subobject of sibling - there can only ever be one
  //  if there isn't one, then we can't be made older - whinge/exit
  GEDCOM_object *thefamc = sibling->subobject( FAMC_tag );
  // should not be possible - menu item is greyed out for this case:
  if (thefamc == NULL ) {
    fl_alert("Can't find a FAMC for that INDI\n");
    return; // whinge we are not a child in any family
  }
  //thefamc->print( 0 );
  GEDCOM_object *thefam = thefamc->followxref();
  if (thefam == NULL) {
    fl_alert("Can't follow FAMC ref to a FAM\n");
    return; // we are very broken if this happens
  }
  //thefam->print( 0 );
  // indirect to the relevant FAM object. find the first CHIL subobject.
  GEDCOM_object *chil1 = thefam->subobject( CHIL_tag );
  if (chil1 == sibling) {
    fl_alert("That's already the oldest child\n");
    return; // whinge that we are already the oldest
  }
  
  GEDCOM_object *chil2=NULL;
  while ((chil1->followxref())!=sibling) {
    chil2 = chil1;
    chil1 = chil1->next_object (CHIL_tag);
    if (chil1==NULL) {
      fl_alert("None of CHIL tags in FAM points back to starting INDI\n");
      return;
    }
  }
  if (!thefam->swap_subobject( chil2, chil1 )) {
    fl_alert("swap_subobject returned false - something broke !\n");
  }
  //thefam->print( 0 );

  // at this point we need to rebuild the displaytree and redraw.
  tree->redraw();
  // that recalculates display, and forces a redraw. Often, that is enough, but
  // sometimes the redrawn tree will be such that the person we just made older
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
  //person->print(0);
  GEDCOM_object *fams1=person->subobject( FAMS_tag );
  GEDCOM_object *fams2;
  while ((fams1->followxref())!=family) {
    fams1 = fams1->next_object (FAMS_tag);
    if (fams1==NULL) {
      fl_alert("None of FAMS tags in INDI points back to starting FAM\n");
      return;
    }
  }
  fams2 = fams1->next_object( FAMS_tag );
  // this should not be possible - the menu item is greyed out if we are
  // already latest but bombproof error-checking does not real harm ;-)
  if (fams2 == NULL) {
    fl_alert("That's already the latest marriage\n");
    return; // whinge that we are already the latest
  }
  if (!person->swap_subobject( fams1, fams2 )) {
    fl_alert("swap_subobject returned false - something broke !\n");
  }
  //person->print( 0 );
  tree->redraw();
}

void menu_earlier_cb(Fl_Menu_ *menu, void* userdata) {

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
  //person->print(0);
  GEDCOM_object *fams1=person->subobject( FAMS_tag );
  if (fams1==family) {
    fl_alert("That's already the earliest marriage\n");
    return; // whinge that we are already the earliest
  }
  GEDCOM_object *fams2=NULL;
  while ((fams1->followxref())!=family) {
    fams2 = fams1;
    fams1 = fams1->next_object (FAMS_tag);
    if (fams1==NULL) {
      fl_alert("None of FAMS tags in INDI points back to starting FAM\n");
      return;
    }
  }
  if (!person->swap_subobject( fams2, fams1 )) {
    fl_alert("swap_subobject returned false - something broke !\n");
  }
  //person->print( 0 );
  tree->redraw();
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
  if (caller==NULL) return;
  // printf("Cast userdata to GEDCOM_object* OK\n");
  treeinstance* tree = ((caller = (GEDCOM_object*)userdata)->root());
  // then you'd need to open a sourceUI window ...
}
