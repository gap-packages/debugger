#
# debugger: Debugging support for GAP
#
# Declarations
#

#! @Arguments file, line [ , function]
#! @Description
#!   Adds a breakpoint to all loaded files whose name
#!   ends <A>file</A>, at line <A>line</A>.
#!   Optionally a <A>function</A> to call can be passed
#!   in, or by default the break loop will be entered.
DeclareGlobalFunction( "AddBreakpoint" );

#! @Arguments file, line
#! @Description
#!   Remove a breakpoints in loaded files whose name
#!   ends <A>file</A>, at line <A>line</A>.
DeclareGlobalFunction( "ClearBreakpoint" );

#! @Arguments
#! @Description
#!   Remove all breakpoints
DeclareGlobalFunction( "ClearAllBreakpoints" );

#! @Arguments
#! @Description
#!   List all breakpoints. Returned as a list of
#!   triples, consisting of file, line, and the function
#!   called.
DeclareGlobalFunction( "ListBreakpoints" );
