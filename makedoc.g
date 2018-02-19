#
# debugger: Debugging support for GAP
#
# This file is a script which compiles the package manual.
#
if fail = LoadPackage("AutoDoc", "2016.02.16") then
    Error("AutoDoc version 2016.02.16 or newer is required.");
fi;

AutoDoc(rec( 
    autodoc := true,
    scaffold := rec(
        includes := [ "Tutorials.xml" ]
    )
    
));

PrintTo("VERSION", PackageInfo("debugger")[1].Version);

