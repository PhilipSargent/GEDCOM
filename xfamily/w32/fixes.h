// fixes.h

// #defines for conditional compilation

#ifndef fixes_h
#define fixes_h
#define WIN32 1

// generic debug on/off for writing new code
#define debugging

// code to log all destructor calls so we see if we really are releasing memory
//#define destructorlogs

// code to journal changes as they are made to try to save the day in the event
// of a data-loss crash
#define journal

#define fix0001
// assume fltk does not scroll an Fl-Scroll to prevent window displaying
// beyond the canvas area (used in gui.cxx) [AERW 2007-09-18 I don't think
// this fix works, if it is fixing what it sounds as if it was meant to fix]
// remove this code in v018 unless we continue to see the problem (in which
// case, do more experiments...) [Still here in v 0.20 I believe ]

#define fltk13
// position(x,y) means something different in fltk 1.3, use scroll_to instead

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

// fix0022 is inactive and we believe fix0024 (which I made permanent, so the
// #define is not useful) fixed the issue. But there is useful commentary under
// fix0022 which I don't want to throw away just yet... The bugs file seems to be
// saying that we fixed being able to open two editindi dboxes on the same INDI,
// but the fix didn't prevent opening two editfam dboxes on the same FAM which
// clearly we do need to prevent.

//#define fix0022
// add new callbacks for new-child-in-family and child-in-new-family
// in v 0.20 turn this off until we get a clean compile, then we can
// change our menus and start bringing the code back without the ephemeral objects...

// fixes 2022_001 and 2022_002 made permanent 2022-02-20 - I think we'd been a
// bit sloppy with the conditionals and turning them off would probably not compile.

#endif
