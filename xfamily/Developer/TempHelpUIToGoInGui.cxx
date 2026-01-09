// I think this musing got superseded when fltk provided a help browser class,
// although it has to be said that fltk's help browser won't let you increase the
// font size beyond a certain point, and that point is still barely legible on
// a 1920x1440 screen ...

///////////////////////////////////////////////////////////////////////////////
// class helpUI constructor: build help window - only one instanciation
//  this needs a callback routine to hand it a filename to use as the html file
//  to be displayed.

helpUI::helpUI() {
  Fl_Window* w = help_panel = new Fl_Window( 600, 400, msg_help_title );
  w -> labeltype(FL_NORMAL_LABEL);
  w->user_data((void*)(this));
  { Fl_Help_View* o = help_view = new Fl_Help_View( 0, 0, 600, 400, msg_help_default );
    o->labeltype(FL_NO_LABEL);
  }
  w->set_non_modal();
  w->end();
}

