// fixes.h

// #defines for conditional compilation

#ifndef fixes_h
#define fixes_h

// generic debug on/off for writing new code
#define debugging

// code to log all destructor calls so we see if we really are releasing memory
//#define destructorlogs

#define fix0001
// assume fltk does not scroll an Fl-Scroll to prevent window displaying
// beyond the canvas area (used in gui.cxx) [AERW 2007-09-18 I don't think
// this fix works, if it is fixing what it sounds as if it was meant to fix]
// remove this code in v018 unless we continue to see the problem (in which
// case, do more experiments...)

//#define fix0002 ALL CODE REMOVED IN v018 - fix0002 conditional code deleted
// fix0002 compiled three methods:
// displaytree::findindi(obj); indidisplay::findindi(obj) and famdisplay::findind(obj)
// which returns the indidisplay for a given GEDCOM INDI object, starting from the
// displaytree, indidisplay or famdisplay on which it was raised. We call it recursively
// and it scans along siblings, and down families.
// The ONLY place we call this is in gui.cxx as newcurrent = display->findindi( person )
// where we now have no need for elaborate code, since fix0003 provides us a method
// to get the indidisplay of the current person directly

#define fix0003
// In this fix (applied in display.h, display.cxx and gui.cxx) we keep a pointer to
// the current person's indisplay in the treedisplay, so as to fix the problem which
// inspired the ill-considered fix0002

//#define test0002 ALL OLD CODE REMOVED IN v018 test0002 conditional code now included
// indidisplay::whoisat() reduce code inefficiency - we think we were testing
// the same thing repeatedly and this fix should stop that

#define fltk13
// position(x,y) means something different in fltk 1.3, use scroll_to instead

#define fix0004
// whenever we open an indiUI, ensure that there is a GEDCOM object to refer to
// for the INDI, and each parent. If we have to create new ones, they will be
// temporary, and not linked into the main GEDCOM, unless or until the user
// hits the "OK" or "Commit/save" buttons. In that case, we will whinge if
// the main INDI has a compulsory field (like NAME) missing, and will make
// the parental objects permanent iff they have a NAME, and if the name does
// not match an existing INDI. However, if the new parent refers to an
// existing parent, we'll substitute the existing real *GEDCOM_object and
// garbage-collect the temporary one.
// files: gui.cxx

#define fix0005
// try to revise header and trailer stuff on output. Trailer code OK.

#define fix0006
// implement GEDCOM_object::insert_after() and insert_before() as alternatives
// to chain_object() and precede_object() as this may make the logic clearer
// now tested and the old code should be removed soon
// v018: old code now conditionally compiled out, so assuming this doesn't
// break anything (so check documentation to ensure you won't write any new
// calls to the purged code), we can make this new code unconditional

#define fix0007
// try to make notesUI have scrollbars. Initially by using Fl_Text_Display
// instead of Multiline_Input, but finally by using Fl_Text_Editor

// upcoming fixes:

//#define fix0008
// display.cxx knows rather a lot about GEDCOM structure - we should have methods,
// applicable to any event object, which return the date, place (and site, for 5.3)
// of the event. OK, that may be overkill - what inspired this is that we want a
// birth and death date for display in an indiUI (and a marriage date for famUI).
// We have a lot of parsing code visible where it is distracting, and for these
// event s specifically, we would like to have *something* to display even if it
// is not exactly what we'd like (though we'd then need to colour-flag it in some
// way, I suppose).
// The idea would be to look up birth date, and return it if we have it, otherwise
// return the earliest of CHR or BAPM, which we'd display in an alternate colour
// as BEF <date>. Similarly, ask for a date of death and if we haven't one, return
// date from CREM or BURI.
// For famUI, we ask for MARR->DATE, but may not have it. We (very rarely) may have
// an ENGA date which we can return, suitably qualified (MARR will be AFT <enga date>).
// Finally, we don't currently display any end date for a marriage (most end on the
// death of the first spouse) but we do have a number of divorces, and a few have
// dates, so we could quite usefully display a second date so that a duration of the
// marriage is shown, much like birth - death dates.

//#define fix0009
// in display.cxx, we have hard-coded some font sizes, because the displaytree
// class doesn't have a pointer to the ones configured for it. In fact, as a
// start (and since we only ever have one prefUI), we should implement a method
// on the preferences to return this - how does prefUI get these ? Does prefUI
// access a preferences structure or is there a whole lot of underlying structure
// missing ? Yes, missing. We should load initial values from a file, allow user
// to update values (and recalculate tree display if needed), and be able to save
// preferences back to a file. This assumes global preferences which I think is
// OK - it would get a lot harder trying to have per-view or per-tree prefs.

#define fix0010
// ad hoc code to add the ability to save the whole current displayed tree via
// a context menu raised on any blank part of the canvas, using fl_read_image()
// you get some sort of X bit map, which will do for a start...

#define fix0011
// extend the four whoisat() methods to return (by reference) one of an indidisplay
// or famdisplay (either or both may be returned NULL). This enables us to test
// whether certain popup menu items are meaningful (younger and later). Added
// convenience methods indidisplay::younger_valid() and famdisplay::later_valid()

#define fix0012
// rather than storing the GEDCOM_object* represented by the display item most
// recently clicked on a tree, store a void* pointer cast from either the
// indidisplay* or famdisplay*.

#endif
