// main entry point for XFamily

// get class declarations
#include "classes.h"
// get global #definitions and static storage declarations
#include "family.h"

#include "structure.h"
#include "trees.h"
#include "gui.h"
#include "objects.h"
#include "xlate.h"

int main(int argc, char* **argv ) {

  // here we should load our choices file to decide (among other
  // things) which language to use
  // here we should parse our command line arguments to see if we
  // want to override those choices
  // here we load the messages into our static strings:
  xlate_messages("en");
  // we always need a list of tags we recognise
  GEDCOM_base_taglist();
  // and the windows which only exist in single instanciations
  aboutbox   = new infoUI;
  choicebox  = new prefUI;
  gotobox    = new findUI;
  // and the linked lists of notes and edit windows, and loaded trees
  noteUIs    = new noteslist;
  editUIs    = new editlist;
  famUIs     = new famedlist;
  trees      = new instancelist;
  // we will always want to put something on the screen :-)
  Fl::visual(FL_DOUBLE|FL_INDEX);

  // create one instanciation:

  treeinstance *first = trees->newinstance( "/usr/local/src/xfamily/test/test.ged" );

  // now find the view of that tree

  mainUI *view = first->mainui();
  view->setcurrent( first->noted_current() );
  // view->settitle(); done by setcurrent anyway
  view->show();

  return Fl::run();

}

// there are still a few global things left:

infoUI       *aboutbox;
prefUI       *choicebox;
findUI       *gotobox;
editlist     *editUIs;
famedlist      *famUIs;
noteslist    *noteUIs;
instancelist *trees;

char person_buffer[256];

// pointers to linked lists of
// a) known GEDCOM tags - this list is global to the program

GEDCOM_tag *first_tag = NULL;
GEDCOM_tag *last_tag  = NULL;

int xMax[MAX_TREE_GENERATIONS];
