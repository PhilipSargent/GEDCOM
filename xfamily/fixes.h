// fixes.h

// #defines for conditional compilation

#ifndef fixes_h
#define fixes_h

// generic debug on/off for writing new code
//#define debugging

// code to log all destructor calls so we see if we really are releasing memory
//#define destructorlogs

#define fix0001
// assume fltk does not scroll an Fl-Scroll to prevent window displaying
// beyond the canvas area (used in gui.cxx) [AERW 2007-09-18 I don't think
// this fix works, if it is fixing what it sounds as if it was meant to fix]
// remove this code in v018 unless we continue to see the problem (in which
// case, do more experiments...)

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
/***************** fix0004 is being backed out of gui.cxx - the idea is OK,
   but the code was being implemented in the wrong place, as became clear
   when we considered equivalents of edit_cb for creating new persons */
// currently fix0023 seems to be inside fix0004 in some places so that backing
// out fix0004 breaks the code. Probably some things inside fix0004 are actually
// useful without the ephemeral objects stuff. Otherwise, move fix0023 outside
// and test that it doesn't still reply n stuff inside fix0004 ...

#define fix0005
// try to revise header and trailer stuff on output. Trailer code OK
// (and removed from conditional compilation). Header code still to do.

// upcoming fixes:

//#define fix0008
// display.cxx knows rather a lot about GEDCOM structure - we should have methods,
// applicable to any event object, which return the date, place (and site, for 5.3)
// of the event. OK, that may be overkill - what inspired this is that we want a
// birth and death date for display in an indiUI (and a marriage date for famUI).
// We have a lot of parsing code visible where it is distracting, and for these
// events specifically, we would like to have *something* to display even if it
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

//#define fix0012
// read Eliminate-treeinstance-geteventobject

#define fix0013
// file chooser to save file under "save as" or "save graphic as"

#define fix0014
// integrity check on loaded GEDCOM (and before saving if debugging is defined)

#define fix0015
// if indi pop-up menu is raised on upper person in a family with two or more
// spouses shown, we want to grey out the "child" menu item, as it would be
// ambiguous. It's a much cleaner UI to block the ambiguity rather than trying
// to resolve it.

#define fix0016
// add GEDCOM id value to a new column in the completions window. on right. gui.cxx

#define fix0017
// revise indiUI to show GEDCOM ids of indi and parents.
// revise famUI similarly for both spouse's ids

//#define fix0018 not needed - code now removed fully
// stop supporting LVG tag

#define fix0019
// rearrange Cancel/OK/Apply/Revert/Help buttons (cosmetic) both indiUI and famUI

#define fix0020
// show age at death in death tab in indiUI from 1 DEAT / 2 AGE <text>

#define fix0021
// implement a flags bitfield in GEDCOM_object

#define fix0022
// add new callbacks for new-child-in-family and child-in-new-family

#define fix0023
// replace the fixed char[16] with a GEDCOM_string to avoid arbitrary limit in GEDCOM_id
// probably counts as a security fix, since it would have allowed a buffer overrun by crafting
// a long id string...

#endif
