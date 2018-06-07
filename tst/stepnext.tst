gap> LoadPackage("debugger", false);
true
gap> LoadPackage("io", false);
true
gap> IO_chdir(Filename(DirectoriesPackageLibrary("debugger", "tst")[1],""));
true
gap> ClearAllBreakpoints();
gap> Read("testcode2.g");
gap> func := function(x,y) Print("::", y, ":", gvar2, "\n"); end;;
gap> BreakEveryLine(func); f(); BreakEveryLine(fail);
::9:mark
::4:mark
::10:A
::4:A
::11:B
::4:B
gap> gvar2 := "startval";;
gap> func := function(x) Print("::", x, ":", gvar2, "\n"); end;;
gap> BreakEveryEnterFunction(func); f(); BreakEveryEnterFunction(fail);
::function (  )
    local x;
    g( "A" );
    g( "B" );
    g( "C" );
    return;
end:startval
::function ( a )
    gvar2 := a;
    return;
end:startval
::function ( a )
    gvar2 := a;
    return;
end:A
::function ( a )
    gvar2 := a;
    return;
end:B
gap> gvar2 := "startval";;
gap> func := function(x) Print("::", x, ":", gvar2, "\n"); end;;
gap> BreakEveryLeaveFunction(func); f(); BreakEveryLeaveFunction(fail);
::function ( a )
    gvar2 := a;
    return;
end:A
::function ( a )
    gvar2 := a;
    return;
end:B
::function ( a )
    gvar2 := a;
    return;
end:C
::function (  )
    local x;
    g( "A" );
    g( "B" );
    g( "C" );
    return;
end:C
