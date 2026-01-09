// classes.h : forward declarations

#ifndef classes_h
#define classes_h

// forward refs: the top level linked list classes (structure.h/cxx)

class instancelist;
class editlist;
class famedlist;
class noteslist;

// tree classes - one instanciation per loaded GEDCOM (trees.h/cxx)

class treeinstance;
// class tree; now subsumed

// window classes (gui.h/cxx)

class mainUI;
class infoUI;
class indiUI;
class famUI;
class statsUI;
class findUI;
class prefUI;
class notesUI;

// menu classes (gui.h/cxx)

class barmenu;
class indipopupmenu;
class fampopupmenu;
class notesmenu;

// component classes

//   display structures - one treedisplay per view or print job (trees.h/cxx)

class treedisplay; // an Fl_Button subclass used by mainUI
class displaytree; // display info for a particular displayed tree
class indidisplay; // one per individual displayed
class famdisplay;  // one per marriage/family displayed

//   GEDCOM structures (objects.h/cxx)

class GEDCOM_object;
class GEDCOM_tag;
class GEDCOM_object;
class GEDCOM_id;

//   general purpose string container

class GEDCOM_string;

#endif
