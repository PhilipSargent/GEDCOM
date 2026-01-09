// structure.cxx

#include "classes.h"
#include "family.h"
#include "structure.h"
#include "trees.h"
#include "gui.h"
#include "objects.h"

////////////////////////////////////////////////////////////////////////////////
// class instancelist

instancelist::instancelist() {
  firstinstance = NULL;
  lastinstance = NULL;
}

instancelist::~instancelist() {
  // do absolutely nothing (even if we actually called...)
  // or maybe we should check that we are empty ?
}

treeinstance * instancelist::newinstance() {

treeinstance *instantia_nova;

  instantia_nova = new treeinstance();
  instantia_nova->setnext( firstinstance );
  if (firstinstance == NULL)
    lastinstance = instantia_nova;
  else
    firstinstance->setprevious( instantia_nova );
  return firstinstance = instantia_nova;
}

treeinstance * instancelist::newinstance( const char * filename ) {

treeinstance *instantia_nova;

  instantia_nova = this->newinstance();
  instantia_nova->loadGEDCOMfile( filename );

  return instantia_nova;
}

void instancelist::dropinstance( treeinstance* instantia ) {

  if ((instantia->getprevious())==NULL) {
    if ((instantia->getnext())==NULL) {
      firstinstance = lastinstance = NULL;
    } else {
      firstinstance = instantia->getnext();
      firstinstance->setprevious(NULL);
    }
  } else {
    if ((instantia->getnext())==NULL) {
      lastinstance = instantia->getprevious();
      lastinstance->setnext(NULL);
    } else {
      instantia->getprevious()->setnext( instantia->getnext() );
      instantia->getnext()->setprevious( instantia->getprevious() );
    }
  }
  delete instantia;
}

void instancelist::reopen() {

treeinstance* thisone;
  thisone = firstinstance;
  while (thisone!=NULL) {
    thisone->reopen();
    thisone = thisone->getnext();
  }
} 

///////////////////////////////////////////////////////////////////////////////
// class editlist controls all the edit windows

editlist::editlist() {
  first_edit = NULL;
  last_edit = NULL;
}

indiUI* editlist::open( treeinstance* thistree, GEDCOM_object * editindi ) {

// first check through the list to see if there is already
// an edit window for this GEDCOM_object

indiUI* newedit;

  newedit = this->editbox( editindi );
  if (newedit!=NULL) {
    newedit->hide();
    newedit->show();
    return newedit;
  }

// OK, we're not editing this individual, so open a new edit box:

  newedit = new indiUI( thistree, editindi );
  if (first_edit != NULL)
    first_edit->setprevious( newedit );
  else
    last_edit = newedit;
  newedit->setnext( first_edit );
  first_edit = newedit;
  newedit->setprevious( NULL );
  if (editindi!=NULL) newedit->insert_details( editindi );
   else newedit->clear_details();
  newedit->settitle();
  newedit->show();
  return newedit;
}

void editlist::close( indiUI* oldindi ) {
  indiUI* tryindi;

  //printf("Want to close indiUI at %ld\n",(long)oldindi);
  tryindi = first_edit;
  while (tryindi != NULL) {
    //printf("checking if that was %ld\n",(long)tryindi);
    if (tryindi == oldindi) {
      if ((oldindi->getnext()) == NULL) {
        if ((oldindi->getprevious()) == NULL) {
           first_edit = last_edit = NULL;
        } else {
           last_edit = oldindi->getprevious();
           last_edit->setnext(NULL);
        }
      } else {
        if ((oldindi->getprevious()) == NULL) {
           first_edit = oldindi->getnext();
           first_edit->setprevious( NULL );
        } else {
           oldindi->getprevious()->setnext( oldindi->getnext());
           oldindi->getnext()->setprevious( oldindi->getprevious() );
        }
      }
      delete oldindi;
      //printf("back from destructor\n");
      return;
    } // end matching indiUI found
    tryindi = tryindi->getnext();
  } // endwhile
  // an error to get here
  printf("an error to get here !\n");
}

indiUI* editlist::editbox( GEDCOM_object* who ) {
  indiUI* trythis;
  trythis = first_edit;
  while (trythis != NULL) {
    if (who == trythis->whois()) return trythis;
    trythis = trythis->getnext();
  }
  // failed to find an edit box for that indi ...
  return NULL;
}

void editlist::retitle() {
  indiUI* thisedit = first_edit;
  while (thisedit != NULL) {
    // no need to test shown() - window doesn't exist if not shown
    //printf("about to hide edit window\n");
    thisedit->hide();
    //printf("About to set its title\n");
    thisedit->settitle();
    thisedit->show();
    thisedit = thisedit->getnext();
  }
}


///////////////////////////////////////////////////////////////////////////////
// class famedlist controls all the editfamily windows

famedlist::famedlist() {
  first_fam = NULL;
  last_fam = NULL;
}

famUI* famedlist::open( treeinstance* thistree, GEDCOM_object * editfam ) {

// first check through the list to see if there is already
// an edit-family window for this GEDCOM_object

famUI* newfam;

  newfam = this->fambox( editfam );
  if (newfam!=NULL) {
    newfam->hide();
    newfam->show();
    return newfam;
  }

// OK, we're not editing this family, so open a new edit-family box:

  newfam = new famUI( thistree, editfam );
  if (first_fam != NULL)
    first_fam->setprevious( newfam );
  else
    last_fam = newfam;
  newfam->setnext( first_fam );
  first_fam = newfam;
  newfam->setprevious( NULL );
  if (editfam!=NULL) newfam->insert_details( editfam );
   else newfam->clear_details();
  newfam->settitle();
  newfam->show();
  return newfam;
}

void famedlist::close( famUI* oldfam ) {
  famUI* tryfam;

  tryfam = first_fam;
  while (tryfam != NULL) {
    if (tryfam == oldfam) {
      if ((oldfam->getnext()) == NULL) {
        if ((oldfam->getprevious()) == NULL) {
           first_fam = last_fam = NULL;
        } else {
           last_fam = oldfam->getprevious();
           last_fam->setnext(NULL);
        }
      } else {
        if ((oldfam->getprevious()) == NULL) {
           first_fam = oldfam->getnext();
           first_fam->setprevious( NULL );
        } else {
           oldfam->getprevious()->setnext( oldfam->getnext());
           oldfam->getnext()->setprevious( oldfam->getprevious() );
        }
      }
      delete oldfam;
      return;
    }
    tryfam = tryfam->getnext();
  }
  // an error to get here
  printf("an error to get here !\n");
}

famUI* famedlist::fambox( GEDCOM_object* what ) {
  famUI* trythis;
  trythis = first_fam;
  while (trythis != NULL) {
    if (what == trythis->whatis()) return trythis;
    trythis = trythis->getnext();
  }
  // failed to find an edit box for that fam ...
  return NULL;
}

void famedlist::retitle() {
  famUI* thisfam = first_fam;
  while (thisfam != NULL) {
    // no need to test shown() - window doesn't exist if not shown
    //printf("about to hide edit window\n");
    thisfam->hide();
    //printf("About to set its title\n");
    thisfam->settitle();
    thisfam->show();
    thisfam = thisfam->getnext();
  }
}

////////////////////////////////////////////////////////////////////////////
//
// class noteslist controls all the notes windows
//
// we don't want to limit the number of sets of notes the user can open,
// view and potentially edit at once, so keep track of the windows in a
// linked list, implemented as a rather minimal class

noteslist::noteslist() {
// this is a pretty minimal class, and there's only one instance of it !
  first_notes = NULL;
  last_notes = NULL;
}

notesUI* noteslist::open( treeinstance* whichtree, GEDCOM_object* newobject, GEDCOM_tag* edittag ) {
notesUI* newui;

// newobject is the object to which the NOTE or TEXT object belongs, ie. typically
// an INDI, FAM or event

// first check through the list to see if there is already
// a notes window for this GEDCOM_object and tag

  newui = first_notes;
  while (newui!=NULL) {
    //printf("searching list of NOTEs windows from %d\n",(int)newui);
    if ((newui->object() == newobject) && (newui->tag() == edittag) ) {
      newui->hide();
      newui->show();
      return newui;
    }
    newui = newui->getnext();
  }

// OK, we're not reading these notes, so open a new editor box:
  //printf("Sure that we need a new notesUI on tree %d, for object %d\n",(int)whichtree,(int)newobject);

  newui = new notesUI( whichtree, newobject, edittag );

  //printf("back in noteslist::open with new notesUI %d\n",(int)newui);
// now add it in at the head of our linked list:

  newui->setnext( first_notes );
  if (first_notes==NULL) {
    last_notes = newui; }
  else {
    first_notes->setprevious( newui );
  }
  newui->setprevious(NULL);
  first_notes = newui;
  newui->show();
  return newui;
}

void noteslist::close( notesUI* ui ) {
  if ((ui->getprevious())==NULL) {
    if ((ui->getnext())==NULL) {
      first_notes = last_notes = NULL;
    } else {
      first_notes = ui->getnext();
      first_notes->setprevious(NULL);
    }
  } else {
    if ((ui->getnext())==NULL) {
      last_notes = ui->getprevious();
      last_notes->setnext(NULL);
    } else {
      ui->getprevious()->setnext( ui->getnext() );
      ui->getnext()->setprevious( ui->getprevious() );
    }
  }
  ui->window->hide();
  //printf("noteslist::close about to call destructor on ui %d\n",(int)ui);
  delete ui;
}

void noteslist::retitle() {

// rebuild the window title for every extant notes window
// called after a language change, for example

notesUI* ui;
  ui = first_notes;
  while (ui!=NULL) {
    ui->retitle();
    ui = ui->getnext();
  }
}

