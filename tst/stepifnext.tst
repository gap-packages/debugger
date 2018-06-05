gap> LoadPackage("debugger", false);
true
gap> LoadPackage("io", false);
true
gap> IO_chdir(Filename(DirectoriesPackageLibrary("debugger", "tst")[1],""));
true
gap> ClearAllBreakpoints();
gap> Read("testif.g");
gap> func := function() Print("Next Line Break\n"); end;;
gap> BreakNextLine(func); f(1);
Next Line Break
gap> BreakNextLine(); f(1);
Error, Breakpoint

# Check we are still not breaking
gap> f(1);
