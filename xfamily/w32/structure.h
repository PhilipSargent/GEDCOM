// structure.h

#ifndef structure_h
#define structure_h

// forward declarations

#include "classes.h"

class instancelist {
  treeinstance *firstinstance;
  treeinstance *lastinstance;
public:
  instancelist();
  ~instancelist(); // purely for form - we will never delete it.
  // create a new treeinstance with no loaded tree
  treeinstance * newinstance();
  // create a new treeinstance with a given filename
  treeinstance * newinstance( const char* );
  void dropinstance( treeinstance* );
  void reopen();
};

class editlist {
  indiUI* first_edit;
  indiUI* last_edit;
public:
  editlist();
  indiUI* checkopen( treeinstance*, GEDCOM_object* );
  indiUI* open( treeinstance*, GEDCOM_object* );
//  indiUI* new();                    // an empty edit window
//  indiUI* child( GEDCOM_object* );  // ditto, the object is a FAM
  void close( indiUI* );
  indiUI* editbox( GEDCOM_object* );  // the box which is editing this object
  void retitle();
};

class famedlist {
  famUI* first_fam;
  famUI* last_fam;
public:
  famedlist();
  famUI* checkopen( treeinstance*, GEDCOM_object* );
  famUI* open( treeinstance*, GEDCOM_object* );
//  famUI* new();                    // an empty fam window (shouldn't need)
  void close( famUI* );
  famUI* fambox( GEDCOM_object* );  // the box which is editing this family
  void retitle();
};

class noteslist {
  notesUI* first_notes;
  notesUI* last_notes;
public:
  noteslist();
  notesUI* checkopen( treeinstance*, GEDCOM_object*, GEDCOM_tag* );
  notesUI* open( treeinstance*, GEDCOM_object*, GEDCOM_tag* );
  void close( notesUI* );
  void retitle();
  notesUI* notesbox( GEDCOM_object* );
};





#endif

