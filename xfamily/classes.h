// classes.h : forward declarations

#ifndef classes_h
#define classes_h

// forward refs: the top level linked list classes (structure.h/cpp)

class instancelist;
class editlist;
class famedlist;
class noteslist;

// tree classes - one instanciation per loaded GEDCOM (trees.h/cpp)

class treeinstance;
// class tree; now subsumed

// window classes (gui.h/cpp)

class mainUI;
class infoUI;
class indiUI;
class famUI;
class statsUI;
class indifindUI;
class indicompletionsUI;
class famfindUI;
class famcompletionsUI;
class prefUI;
class notesUI;

// menu classes (gui.h/cpp)

class barmenu;
class indipopupmenu;
class fampopupmenu;
class notesmenu;

// component classes

//   display structures - one treedisplay per view or print job (display.h/cpp)

class treedisplay; // an Fl_Button subclass used by mainUI
class displaytree; // display info for a particular displayed tree
class indidisplay; // one per individual displayed
class famdisplay;  // one per marriage/family displayed

//   GEDCOM structures (objects.h/cpp)

class GEDCOM_object;
class GEDCOM_tag;
class GEDCOM_id;

//   other structures (objects.h/cpp)

class indicompletion_item;
class famcompletion_item;

//   general purpose string container

class GEDCOM_string;

#endif
