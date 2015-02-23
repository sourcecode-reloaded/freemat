#ifndef __Debug_hpp__
#define __Debug_hpp__



namespace FM
{

  enum class BreakpointType {Unconditional, Conditional, Transient};
  
  struct Breakpoint
  {
    Object frame_name;
    int    line_number;
    BreakpointType bp_type;
  };

  
  class Frame;
  struct DebugState
  {
    Frame *_activeFrame;
    int _fp_when_called;
  };
};

#endif
