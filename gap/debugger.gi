

InstallGlobalFunction( "AddBreakpoint",
function(fileend, line, infunc...)
	local func, i, hitfiles, filelist;
	filelist := GET_FILENAME_CACHE();
	hitfiles := [];
	for i in [1..Length(filelist)] do
		if EndsWith(filelist[i], fileend) then
			Add(hitfiles, i);
		fi;
	od;

	if not IsString(fileend) then
		ErrorNoReturn("First argument must be a string");
	fi;
	
	if not IsInt(line) then
		ErrorNoReturn("Second argument must be an int");
	fi;

	if Length(infunc) = 0 then
		func := fail;
	else
		if Length(infunc) = 1 and IsFunction(infunc[1]) then
			func := infunc[1];
		else
			ErrorNoReturn("Usage: filename, line [, func])");
		fi;
	fi;

	if Length(hitfiles) = 0 then
		ErrorNoReturn("Filename not found");
	fi;

	for i in hitfiles do
		Print("Adding breakpoint to ", filelist[i], ":", line,"\n");
		if func = fail then
			ADD_BREAKPOINT(i, line, function()
										Error("Breakpoint ", filelist[i], ":", line);
									end);
		else
			ADD_BREAKPOINT(i, line, func);
		fi;
	od;
end);

InstallGlobalFunction( "ClearBreakpoint",
function(fileend, line)
	local func, i, hitfiles, filelist;
	filelist := GET_FILENAME_CACHE();
	hitfiles := [];
	for i in [1..Length(filelist)] do
		if EndsWith(filelist[i], fileend) then
			Add(hitfiles, i);
		fi;
	od;

	if Length(hitfiles) = 0 then
		ErrorNoReturn("Filename not found");
	fi;

	for i in hitfiles do
		if CLEAR_BREAKPOINT(i, line, func) then
			Print("Removing breakpoint from ", filelist[i], ":", line);
		fi;
	od;
end);

InstallGlobalFunction( "ClearAllBreakpoints",
	CLEAR_ALL_BREAKPOINTS);

InstallGlobalFunction( "ListBreakpoints",
       GET_BREAKPOINTS);

InstallGlobalFunction( "BreakEveryLine",
	SET_EVERY_STATEMENT_BREAKPOINT);

InstallGlobalFunction( "BreakNextLine",
	SET_NEXT_STATEMENT_BREAKPOINT);

InstallGlobalFunction( "BreakEveryEnterFunction",
	SET_EVERY_ENTER_FUNCTION_BREAKPOINT);

InstallGlobalFunction( "BreakNextEnterFunction",
	SET_NEXT_ENTER_FUNCTION_BREAKPOINT);

InstallGlobalFunction( "BreakEveryLeaveFunction",
	SET_EVERY_LEAVE_FUNCTION_BREAKPOINT);

InstallGlobalFunction( "BreakNextLeaveFunction",
	SET_NEXT_LEAVE_FUNCTION_BREAKPOINT);

# These functions are all accessed from the C level,
# and used as standard functions for breakpoints.

BREAKPOINT_DEFAULT_FILELINE := function(file, line)
	Error("Breakpoint ", GET_FILENAME_CACHE()[file], ":", line);
end;

BREAKPOINT_DEFAULT_FUNCTION := function(func)
	Error("Breakpoint ", NAME_FUNC(func), " ", LocationFunc(func));
end;

BREAKPOINT_NO_ARGS := function()
	Error("Breakpoint");
end;


InstallGlobalFunction( "ShowLocals", function()
    local lvars, argcount, variadic, value, i;
    lvars := ErrorLVars;
    if lvars = fail then
        ErrorNoReturn("ShowLocals must be run from break loop or a function");
    fi;
    lvars := ContentsLVars(lvars);
    if not IsRecord(lvars) then
        ErrorNoReturn("ShowLocals unable to read local variables");
    fi;
    argcount := NumberArgumentsFunction(lvars.func);
    variadic := false;
    if argcount < 0 then
        argcount := -argcount;
        variadic := true;
    fi;

    if variadic then
        PrintFormatted("Variadic function with at least {} arguments\n", argcount-1);
    else
        PrintFormatted("Function with {} arguments\n", argcount);
    fi;
    for i in [1..argcount] do
        if IsBound(lvars.values[i]) then
            value := lvars.values[i];
        else
            value := "<unbound>";
        fi;
        PrintFormatted(" {}: {}\n", lvars.names[i], value);
    od;
    Print("Local variables:\n");
    for i in [argcount+1..Length(lvars.names)] do
        if IsBound(lvars.values[i]) then
            value := lvars.values[i];
        else
            value := "<unbound>";
        fi;
        PrintFormatted(" {}: {}\n", lvars.names[i], value);
    od;
end);
