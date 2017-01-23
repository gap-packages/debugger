gap> LoadPackage("debugger", false);
true
gap> ClearAllBreakpoints();
gap> Read("testif.g");
gap> func := function(x,y) Print("::", y, ":", gvar, "\n"); end;;
gap> BreakEveryLine(func); f(1); BreakEveryLine(fail);
::5:mark
::8:start
::6:start
::7:start
::9:inif
gap> gvar := "mark";;
gap> BreakEveryLine(func); f(-1); BreakEveryLine(fail);
::5:mark
::8:start
::6:start
::9:start
