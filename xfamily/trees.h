// trees.h

#ifndef trees_h
#define trees_h

#include <stdio.h>

#include "classes.h"

class treeinstance {
  treeinstance *next;
  treeinstance *previous;
  mainUI       *firstview;
  statsUI      *statsbox;

  GEDCOM_object *rootobject;
  GEDCOM_object *headlist; // these six are theoretically accessible by
  GEDCOM_object *indilist; // GEDCOM_object methods on rootobject, but
  GEDCOM_object *famslist; // we will gain in both clarity and performance
  GEDCOM_object *sourlist; // by having direct access to them as class
  GEDCOM_object *repolist; // members.
  GEDCOM_object *trlrlist;
  GEDCOM_object *lastevent;// the object on which a mouse event was last
  // raised - this can be fairly transient, only needed by the callback
  // which deals with the event, so each event can overwrite it
  char* filename;
// each instance also needs its own list of @id@s
  GEDCOM_id *first_id;
  GEDCOM_id *last_id;
  int indicount; int maxINDIid;
  int famcount;  int maxFAMid;
  int sourcount; int maxSOURid;
  int repocount; int maxREPOid;
  bool modified;

public:
  treeinstance();
  ~treeinstance();
// methods to traverse/maintain linked list
  treeinstance * getnext() const;
  void setnext( treeinstance * );
  treeinstance * getprevious() const;
  void setprevious( treeinstance * );

// methods for loading and saving GEDCOM
  GEDCOM_object* getGEDCOMline( FILE*, GEDCOM_tag**, int*, const int);
  void loadGEDCOMfile( const char *);
  void setfilename( char* );
  void save() const;
  char* getfilename() const;

// methods for extracting stats
  int getindicount() const;
  int getfamcount() const;
  int getsourcount() const;
  int getrepocount() const;
  int getmaxindi() const;
  int getmaxfam() const;
  int getmaxsour() const;
  int getmaxrepo() const;

// methods for traversing/maintaining GEDCOM structures
  GEDCOM_id* add_id( char* );
  void drop_id( GEDCOM_id* );
  int GEDCOM_objectforref( char*, GEDCOM_object* );
  GEDCOM_id* GEDCOM_idfromref( char* );
  GEDCOM_object* GEDCOM_objectfromref( char * );
  GEDCOM_object* Lookup_INDI( char* ) const;

// !Family compatability
  GEDCOM_object* noted_current() const;
// no method yet defined to set the 0 NOTE person <name>
// since it is an ambiguous concept (one current person per view)

// access to windows dealing with this tree:
  mainUI* mainui() const;   // first view in list, then use mainUI::getnext()
  statsUI* statsui() const; // only one stats window
  void addview( mainUI* );
  void reopen();
  void setevent( GEDCOM_object* );
  GEDCOM_object* geteventobject() const;
};

#endif
