// trees.h

#ifndef trees_h
#define trees_h

#include "fixes.h"

#include <stdio.h>

#include "classes.h"

class treeinstance {
  treeinstance *next;
  treeinstance *previous;
  mainUI       *firstview;
  int           UIcount;
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
  // hmm, but with multiple views, events can be raised on different
  // views of the same tree simultaneously - think this belongs in
  // the display code, not here.
  char* filename;
// each instance also needs its own list of @id@s
  GEDCOM_id *first_id;
  GEDCOM_id *last_id;
  int indicount; int maxINDIid;
  int famcount;  int maxFAMid;
  int sourcount; int maxSOURid;
  int repocount; int maxREPOid;
  int linecount;
  bool modified;

public:
  treeinstance();
  ~treeinstance();
// debug method
  void dump();
// methods to traverse/maintain linked list
  treeinstance * getnext() const;
  void setnext( treeinstance * );
  treeinstance * getprevious() const;
  void setprevious( treeinstance * );

// methods for creating and destroying GEDCOM
void freshindi( GEDCOM_object* );
void freshfam( GEDCOM_object* );
void remove_indi( GEDCOM_object* );
void remove_fam( GEDCOM_object* );

// methods for loading and saving GEDCOM
  GEDCOM_object* getGEDCOMline( FILE*, GEDCOM_tag**, int*, const int);
  void loadGEDCOMfile( const char *);
  bool integritycheck();
  // integritycheck was huge and monolithic and has now been split into comprehensible chunks
  // which could nominally be used independently, so they are also public (but really intended for internal use)
  // for a little extra overhead, we've also reduced boilerplate repetition.
  bool integritycheck_indi( GEDCOM_object*);
  bool integritycheck_parent( GEDCOM_tag*, GEDCOM_object*, GEDCOM_object*);
  bool integritycheck_famc( GEDCOM_object*, GEDCOM_object*);
  bool integritycheck_fam( GEDCOM_object*);
  bool integritycheck_spouse( GEDCOM_tag*, GEDCOM_object*, GEDCOM_object*);
  bool integritycheck_child( GEDCOM_object *, GEDCOM_object*);
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

// methods for assigning free ids
  int getnextindi();
  int getnextfam();
  int getnextsour();
  int getnextrepo();

// methods for traversing/maintaining GEDCOM structures
  GEDCOM_id* add_id( char* );    // it might help if this had a better name like create_id
                                 // but .cpp says we should never use it, so perhaps check and remove ?
  void add_id( GEDCOM_id* );
  void drop_id( GEDCOM_id* );
  int GEDCOM_objectforref( char*, GEDCOM_object* );
  GEDCOM_id* GEDCOM_idfromref( char* );
  GEDCOM_object* GEDCOM_objectfromref( char * );
  GEDCOM_object* Lookup_INDI( char* ) const;
  GEDCOM_object* Lookup_INDI( GEDCOM_object*, char* ) const;
  GEDCOM_object* INDI_fromleft( GEDCOM_object*, char* ) const;
  GEDCOM_object* INDI_fromright( GEDCOM_object*, char* ) const;
  GEDCOM_object* INDI_fuzzymatch( GEDCOM_object*, char* ) const;


// !Family compatability ( 0 NOTE Person <name> )
  GEDCOM_object* noted_person() const;
// xfamily idiom for same functionality ( 0 NOTE View <indi-id> )*
  GEDCOM_object* noted_view( GEDCOM_object* ) const;

// access to windows dealing with this tree:
  mainUI* mainui() const;   // first view in list, then use mainUI::getnext()
  statsUI* statsui() const; // only one stats window
  void addview( mainUI* );
  int viewcount();
  void removeview( mainUI*);
  void redraw();
  void reopen();
  void setevent( GEDCOM_object* );
  GEDCOM_object* geteventobject() const;
};

#endif
