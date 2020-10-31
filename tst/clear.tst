gap> LoadPackage("debugger", false);
true
gap> LoadPackage("io", false);
true
gap> IO_chdir(Filename(DirectoriesPackageLibrary("debugger", "tst")[1],""));
true
gap> ClearAllBreakpoints();
gap> Read("testcode1.g");
gap> AddBreakpoint("testcode1.g", 5, function() Print("5:",gvar,"\n"); end);
Adding breakpoint to testcode1.g:5
gap> AddBreakpoint("testcode1.g", 6, function() Print("6:",gvar,"\n"); end);
Adding breakpoint to testcode1.g:6
gap> AddBreakpoint("testcode1.g", 7, function() Print("7:",gvar,"\n"); end);
Adding breakpoint to testcode1.g:7
gap> f();
5:mark
6:mark
7:change
gap> f();
5:change
6:change
7:change
gap> ClearBreakpoint("testcode1.g", 6);
Removing breakpoint from testcode1.g:6
gap> f();
5:change
6:change
gap> ClearBreakpoint("testcode1.g", 6);
gap> f();
5:change
6:change
gap> ClearBreakpoint("testcode1.g", 5);
Removing breakpoint from testcode1.g:5
gap> f();
5:change
gap> ClearBreakpoint("qqqqwwwwnotafilenameg", 5);
Error, Filename not found
gap> f();
5:change
gap> ClearAllBreakpoints();
gap> f();
