// fixes.h

// #defines for conditional compilation

#ifndef fixes_h
#define fixes_h

#define debugging

#define fix0001
// assume fltk does not scroll an Fl-Scroll to prevent window displaying
// beyond the canvas area (used in gui.cxx) [AERW 2007-09-18 I don't think
// this fix works, if it is fixing what it sounds as if it was meant to fix]

#define fix0002
// fix0002 compiles a method findindi which returns the indidisplay for a given
// GEDCOM INDI object. Seems like a lot of code to execute rather than maintaining
// a pointer from that INDI object ? Hmmm, but any one INDI may have multiple
// indidisplays - most obviously on different views, but also, potentially on
// the same view (lots of cousin marriages in this family...) so you can't just
// have a pointer to an indidisplay - it would have to be a linked list.

// obviously findindi returns just a single object, and not a linked list
// so we must have decided that a unique value was acceptable when we wrote
// the code, so the above argument doesn't seem to hold water (2011-09-09)

// fix0002 applies in display.h and display.cxx
// as well as gui.cxx The purpose seems to be to be able to find the indidisplay
// for the new current person in a view by laboriously scanning the treedisplay
// looking for the indidisplay that points to that INDI.
// AERW 2007-09-18 there must have been a good reason for doing it that way,
// but it seems like a huge amount of code execution to find one bit of state
// that we could have saved earlier when the current person (who is specific
// to a particular view) was set. What am I missing here ?
// OK that does provide a generic solution to a general problem - but it is a
// general problem we don't have. The only problem we seem to have is that
// we want to know the current person (as distinct from the tree top, who is
// often a parent of the current person) so we can centre him/her. It would
// seem far more sensible to store a pointer to the indidisplay of the current
// person in the treedisplay structure when it is created, and thus have
// immediate access to it without all this overhead.

// the most obvious way to proceed is to add the pointers we think we need,
// and aim to eliminate all calls to code included in fix0002, such that we
// can undefine it and the code compiles and works...

#define fix0003
// not coded up yet. In this fix (which will be applied in display.h, display.cxx
// and gui.cxx) we will keep a pointer to the current person in the treedisplay,
// so as to fix the problem which inspired fix0002
// but do we want a pointer to the indidisplay for the current person or to the
// INDI GEDCOM object ? Probably the former, as that has a pointer to the INDI,
// but the latter has no pointer the other way.

#define test0002
// indidisplay::whoisat() reduce code inefficiency - we think we were testing
// the same thing repeatedly and this fix should stop that
#define fltk13
// postion(x,y) means something different in fltk 1.3, use scroll_to instead

#endif
