gap> LoadPackage("debugger", false);
true
gap> testdir:= DirectoriesPackageLibrary( "debugger", "tst" )[1];;
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