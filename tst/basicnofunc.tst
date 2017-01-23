gap> LoadPackage("debugger", false);
true
gap> ClearAllBreakpoints();
gap> Read("testcode1.g");
gap> AddBreakpoint("testcode1.g", 5);
Adding breakpoint to testcode1.g:5
gap> AddBreakpoint("testcode1.g", 6);
Adding breakpoint to testcode1.g:6
gap> AddBreakpoint("testcode1.g", 7);
Adding breakpoint to testcode1.g:7
gap> f();
Error, Breakpoint testcode1.g:5