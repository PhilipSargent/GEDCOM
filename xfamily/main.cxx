// main entry point for XFamily

// get class declarations
#include "classes.h"
// get global #definitions and static storage declarations
#include "family.h"
// get conditional compilation flags
#include "fixes.h"

#include "structure.h"
#include "trees.h"
#include "gui.h"
#include "objects.h"
#include "xlate.h"

int main(int argc, char** argv ) {
// that used to be char* **argv but now that's a no no it seems

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

  char* initialtree;
  {
     char it[] = "/usr/local/src/xfamily/test/test2.ged";
     initialtree = it;
     // this is always being reported as (null). char* initialtree = "..."; always used to work, how to do it now ??
  }
  if (argc>0) {
     initialtree = argv[1];
  }
#ifdef debugging
  printf("about to create a treeinstance on initialtree at %s\n",initialtree);
#endif
  treeinstance *first = trees->newinstance( initialtree );
#ifdef debugging
  printf("created first treeinstance at %ld, next determine view of tree\n",(long)first);
#endif

  // now find the view of that tree

  mainUI *view = first->mainui();
#ifdef debugging
  printf("initial mainUI view at %ld, about to set current person\n",(long)view);
#endif
  view->setcurrent( first->noted_current() );
#ifdef debugging
  printf("current person and view title now set, about to show the view and enter the fltk event loop\n");
#endif
  // view->settitle(); done by setcurrent anyway
  view->show();

  return Fl::run();

}

// there are still a few global things left:

infoUI       *aboutbox;
prefUI       *choicebox;
findUI       *gotobox;
completionsUI *completionsbox;
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
