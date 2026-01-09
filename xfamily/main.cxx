// main entry point for XFamily

// get conditional compilation flags
#include "fixes.h"
// get class declarations
#include "classes.h"
// get global #definitions and static storage declarations
#include "family.h"

#include "structure.h"
#include "trees.h"
#include "gui.h"
#include "objects.h"
#include "xlate.h"
#include "tags.h"

int main(int argc, char* argv[] ) {

  GEDCOM_object* noted;
  GEDCOM_object* current;
  char id[MAX_TAG];
  char* notedid;
  // here we should load our choices file to decide (among other
  // things) which language to use
  // here we should parse our command line arguments to see if we
  // want to override those choices
  // here we load the messages into our static strings:
  xlate_messages(strdup("en"));
  // we always need a list of tags we recognise
  GEDCOM_base_taglist();
  // and the windows which only exist in single instanciations
  aboutbox   = new infoUI;
  choicebox  = new prefUI;
  gotobox    = new findUI;
  completionsbox = new completionsUI;
  // and the linked lists of notes and edit windows, and loaded trees
  noteUIs    = new noteslist;
  editUIs    = new editlist;
  famUIs     = new famedlist;
  trees      = new instancelist;
  // we will always want to put something on the screen :-)
  Fl::visual(FL_DOUBLE|FL_INDEX);

  // create one instanciation:

  // if we were called with command line args, that should tell us the file to load here
  // if not, then we should create an empty tree here, and let the user either add to it
  // or use the file load dialogue to get a tree. As neither of those are implemented, we
  // load a tree from a default place at the moment:

  char* initialtree = strdup( "/usr/local/src/xfamily/test/peel.ged" );

  // using strdup gets rid of the warning that converting a string constant to a char* is deprecated

#ifdef debugging
  printf("Initial value of initial tree'%s'",initialtree);
#endif

  if (argc > 1) {
     initialtree = argv[1];
  }
  GEDCOMcount = 0;
  treeinstance *first = trees->newinstance( initialtree );

  // now find the view of that tree

  mainUI *view = first->mainui();
  noted = first->noted_view( (GEDCOM_object*) NULL );
  if (noted != NULL) {
    // noted is a pointer to the 0 NOTE View object
    // noted->value() should be "View @I<num>@"
    // but what do we need to pass to GEDCOM_objectfromref ? @I<num>@ or just I<num> ?
    notedid=(noted->value())+6;
#ifdef debugging
      printf("noted  id is '%s'\n",notedid);
#endif
    strncpy( id, notedid, strlen(notedid)-1 );
    // that produces something not-null-terminated which fails to match later - a very opaque bug
    memcpy(id+strlen(notedid)-1,"\0",1);
#ifdef debugging
      printf("copied id is '%s'\n",notedid);
#endif
    current = first->GEDCOM_objectfromref( id );
    // defensive coding in case there was a duff 0 NOTE View like 0 NOTE View @S34@
    // or we got no valid INDI for some other reason (like broken code...)
    if ( (current!=NULL) && (current->objtype() == INDI_tag) ) {
      view->setcurrent( current );
    } else {
      view->setcurrent( first->noted_person() );
    }
    view->show();
    // we should now loop looking for additional 0 NOTE View objects
    // and opening a new view for each.
    noted = first->noted_view( noted );
    while ( noted!=NULL ) {
      notedid=(noted->value())+6;
#ifdef debugging
      printf("noted  id is '%s'\n",notedid);
#endif
      strncpy( id, notedid, strlen(notedid)-1 );
      memcpy(id+strlen(notedid)-1,"\0",1);
#ifdef debugging
      printf("copied id is '%s'\n",notedid);
#endif
      current = first->GEDCOM_objectfromref( id );
      view = new mainUI( first );
      first->addview( view );
      view->setcurrent( current );
      view->show();
      noted = first->noted_view( noted );
    }
  }
  else {    
    view->setcurrent( first->noted_person() );
    // if there is no 0 NOTE Person, that will return a suitable default
    // so it should always be safe to setcurrent and show the view
    view->show();
  }

  return Fl::run();

}

// there are still a few global things left:

infoUI       *aboutbox;
prefUI       *choicebox;
findUI       *gotobox;
completionsUI *completionsbox;
editlist     *editUIs;
famedlist    *famUIs;
noteslist    *noteUIs;
instancelist *trees;
int           GEDCOMcount;


char person_buffer[256];

// pointers to linked lists of
// a) known GEDCOM tags - this list is global to the program

GEDCOM_tag *first_tag = NULL;
GEDCOM_tag *last_tag  = NULL;

int xMax[MAX_TREE_GENERATIONS];
