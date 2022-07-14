/*
 * debugger: Debugging support for GAP
 */

extern "C" {
#include "src/compiled.h"          /* GAP headers                */
#include "src/hookintrprtr.h"
}

#include "gap_cpp_headers/gap_cpp_mapping.hpp"

#include <stdio.h>
#include <vector>
#include <utility>

// List of break points
std::vector<std::pair<Int, Int> > break_points;

// List of functions to call
// In a GAP object so they are not GCed accidentally.
Obj breakpoint_functions;

// Function to call whenever moving to a new statement.
Obj every_step_function;

// Function to call next time a new statement.
Obj next_step_function;

// Function to call whenever entering a function.
Obj every_enter_function;

// Function to call next time entering a function.
Obj next_enter_function;

// Function to call whenever leaving a function.
Obj every_leave_function;

// Function to call next time leaving a function.
Obj next_leave_function;


// The last location -- so we do not keep triggering on the same line.
static std::pair<Int, Int> prevlocation;

// Checks if we are currently inside a function called by the debugger,
// or inside the break loop, so we should not invoke any more debugging
// functions, to avoid infinite loops.
static Int disable_debugger;


// If GAP ever longjmps, let's re-enable the debugger. This isn't perfect,
// but stops the debugger apparently dying.
extern "C" {
void resetDebuggerOnThrow(int depth)
{ disable_debugger = 0; }

void resetDebuggerOnBreakLoop(Int i)
{ disable_debugger = i; }
}

Obj ACTIVATE_DEBUGGING(Obj self);
Obj DEACTIVATE_DEBUGGING(Obj self);

// Check if we should enable or disable hooks
// TODO: Improve, error checking
void ConsiderEnableDisableDebugging()
{
    bool breakpoint = (!break_points.empty() ||
                        every_step_function || next_step_function ||
                        every_enter_function || next_enter_function ||
                        every_leave_function || next_leave_function);
    if(breakpoint)
        ACTIVATE_DEBUGGING(0);
    else
        DEACTIVATE_DEBUGGING(0);
}

// Call a function, suspending debugging while it runs
static void callDebugFunction0(Obj funcobj)
{
    disable_debugger = 1;
    CALL_0ARGS(funcobj);
    disable_debugger = 0;
}

static void callDebugFunction1(Obj funcobj, Obj val)
{
    disable_debugger = 1;
    CALL_1ARGS(funcobj, val);
    disable_debugger = 0;
}

static void callDebugFunction2(Obj funcobj, Obj val1, Obj val2)
{
    disable_debugger = 1;
    CALL_2ARGS(funcobj, val1, val2);
    disable_debugger = 0;
}

void debugVisitStat(Stat stat)
{
    if(disable_debugger)
        return;

    Obj func = CURR_FUNC();
    Obj body = BODY_FUNC(func);
    Int file = GET_GAPNAMEID_BODY(body);

    Int line = LINE_STAT(stat);
    // skip if not valid
    if(file == 0 || line == 0)
        return;
    std::pair<Int, Int> location(file, line);
    // Check we have moved line
    if(prevlocation == location)
        return;
    if(next_step_function)
    {
        Obj store = next_step_function;
        next_step_function = 0;
        callDebugFunction0(store);
    }
    if(every_step_function)
        callDebugFunction2(every_step_function, INTOBJ_INT(file), INTOBJ_INT(line));
    prevlocation = location;

    bool hit = false;
    for(int i = 0; i < break_points.size(); ++i)
    {
        if(break_points[i] == location)
        {
            hit = true;
            callDebugFunction0(ELM_PLIST(breakpoint_functions, i+1));
        }
    }
}

void debugEnterFunction(Obj func)
{
    if(next_enter_function && !disable_debugger)
    {
        Obj store = next_enter_function;
        next_enter_function = 0;
        callDebugFunction1(next_enter_function, func);
    }
    if(every_enter_function && !disable_debugger)
        callDebugFunction1(every_enter_function, func);
}

void debugLeaveFunction(Obj func)
{
    if(next_leave_function && !disable_debugger)
    {
        Obj store = next_leave_function;
        next_leave_function = 0;
        callDebugFunction1(next_leave_function, func);
    }
    if(every_leave_function && !disable_debugger)
        callDebugFunction1(every_leave_function, func);
}


Obj ADD_BREAKPOINT(Obj self, Obj objfile, Obj objline, Obj func)
{
    Int intfile = INT_INTOBJ(objfile);
    Int intline = INT_INTOBJ(objline);
    break_points.push_back(std::pair<Int, Int>(intfile, intline));
    Int breaklen = break_points.size();
    GROW_PLIST(breakpoint_functions, breaklen);
    SET_LEN_PLIST(breakpoint_functions, breaklen);
    SET_ELM_PLIST(breakpoint_functions, breaklen, func);
    CHANGED_BAG(breakpoint_functions);
    ConsiderEnableDisableDebugging();
    return 0;
}

// We do this at the C level rather than GAP, as this function is used when
// people are enabling, or disabling breakpoints, so we want to run as little
// GAP as possible!
static Obj SetValue(Obj* value, Obj funclist, const char* defaultfunc)
{
    if(LEN_PLIST(funclist) == 0)
    {
        *value = VAL_GVAR(GVarName(defaultfunc));
    }
    else if(LEN_PLIST(funclist) == 1)
    {
        Obj func = ELM_PLIST(funclist, 1);
        if(func == Fail)
            *value = 0;
        else
        {
            if(!IS_FUNC(func))
            {
                ErrorMayQuit("Breakpoint must be a function (or fail)",0,0);
            }
            else
            {
                *value = func;
            }
        }
    }
    else
    {
        ErrorMayQuit("Error: Maximum of one argument",0,0);
    }
    ConsiderEnableDisableDebugging();
    return 0;
}

Obj SET_NEXT_STATEMENT_BREAKPOINT(Obj self, Obj func)
{ return SetValue(&next_step_function, func, "BREAKPOINT_NO_ARGS"); }

Obj SET_EVERY_STATEMENT_BREAKPOINT(Obj self, Obj func)
{ return SetValue(&every_step_function, func, "BREAKPOINT_DEFAULT_FILELINE"); }

Obj SET_NEXT_ENTER_FUNCTION_BREAKPOINT(Obj self, Obj func)
{ return SetValue(&next_enter_function, func, "BREAKPOINT_DEFAULT_FUNCTION"); }

Obj SET_EVERY_ENTER_FUNCTION_BREAKPOINT(Obj self, Obj func)
{ return SetValue(&every_enter_function, func, "BREAKPOINT_DEFAULT_FUNCTION"); }

Obj SET_NEXT_LEAVE_FUNCTION_BREAKPOINT(Obj self, Obj func)
{ return SetValue(&next_leave_function, func, "BREAKPOINT_DEFAULT_FUNCTION"); }

Obj SET_EVERY_LEAVE_FUNCTION_BREAKPOINT(Obj self, Obj func)
{ return SetValue(&every_leave_function, func, "BREAKPOINT_DEFAULT_FUNCTION"); }


Obj CLEAR_BREAKPOINT(Obj self, Obj objfile, Obj objline)
{
    Int intfile = INT_INTOBJ(objfile);
    Int intline = INT_INTOBJ(objline);
    std::pair<Int, Int> location(intfile, intline);

    Obj removed = False;

    for(int i = 0; i < break_points.size(); ++i)
    {
        if(break_points[i] == location)
        {
            removed = True;
            break_points.erase(break_points.begin() + i);
            for(int j = i+1; j < break_points.size(); ++j)
            {
                Obj val = ELM_PLIST(breakpoint_functions, j+1);
                SET_ELM_PLIST(breakpoint_functions, j, val);
            }
            SET_LEN_PLIST(breakpoint_functions, break_points.size());
            CHANGED_BAG(breakpoint_functions);
            i--;
        }
    }
    ConsiderEnableDisableDebugging();
    return removed;
}

Obj CLEAR_ALL_BREAKPOINTS(Obj self)
{
    breakpoint_functions = NEW_PLIST(T_PLIST, 0);
    break_points.clear();
    ConsiderEnableDisableDebugging();
    return 0;
}

Obj GET_BREAKPOINTS(Obj self)
{
    return GAP_make(break_points);
}

#if GAP_KERNEL_MAJOR_VERSION >= 6
struct InterpreterHooks debugHooks =
{
    debugVisitStat,
    0,
    debugEnterFunction,
    debugLeaveFunction,
    0,
    0,
    "debugger"
};
#else
struct InterpreterHooks debugHooks =
{
    debugVisitStat,
    debugEnterFunction,
    debugLeaveFunction,
    0,
    "debugger"
};
#endif
Obj ACTIVATE_DEBUGGING(Obj self)
{
    return ActivateHooks(&debugHooks) ? True : False;
}

Obj DEACTIVATE_DEBUGGING(Obj self)
{
    return DeactivateHooks(&debugHooks) ? True : False;
}

typedef Obj (* GVarFunc)(/*arguments*/);

#define GVAR_FUNC_TABLE_ENTRY(srcfile, name, nparam, params) \
  {#name, nparam, \
   params, \
   (GVarFunc)name, \
   srcfile ":Func" #name }

// Table of functions to export
static StructGVarFunc GVarFuncs [] = {
    GVAR_FUNC_TABLE_ENTRY("debugger.c", ACTIVATE_DEBUGGING, 0, ""),
    GVAR_FUNC_TABLE_ENTRY("debugger.c", DEACTIVATE_DEBUGGING, 0, ""),
    GVAR_FUNC_TABLE_ENTRY("debugger.c", GET_BREAKPOINTS, 0, ""),
    GVAR_FUNC_TABLE_ENTRY("debugger.c", ADD_BREAKPOINT, 3, "file, line, func"),
    GVAR_FUNC_TABLE_ENTRY("debugger.c", SET_EVERY_STATEMENT_BREAKPOINT, -1, "func"),
    GVAR_FUNC_TABLE_ENTRY("debugger.c", SET_NEXT_STATEMENT_BREAKPOINT, -1, "func"),
    GVAR_FUNC_TABLE_ENTRY("debugger.c", SET_EVERY_ENTER_FUNCTION_BREAKPOINT, -1, "func"),
    GVAR_FUNC_TABLE_ENTRY("debugger.c", SET_NEXT_ENTER_FUNCTION_BREAKPOINT, -1, "func"),
    GVAR_FUNC_TABLE_ENTRY("debugger.c", SET_EVERY_LEAVE_FUNCTION_BREAKPOINT, -1, "func"),
    GVAR_FUNC_TABLE_ENTRY("debugger.c", SET_NEXT_LEAVE_FUNCTION_BREAKPOINT, -1, "func"),
    GVAR_FUNC_TABLE_ENTRY("debugger.c", CLEAR_BREAKPOINT, 2, "file, line"),
	GVAR_FUNC_TABLE_ENTRY("debugger.c", CLEAR_ALL_BREAKPOINTS, 0, ""),
    { 0 } /* Finish with an empty entry */

};

/******************************************************************************
*F  InitKernel( <module> )  . . . . . . . . initialise kernel data structures
*/
static Int InitKernel( StructInitInfo *module )
{
    /* init filters and functions                                          */
    InitHdlrFuncsFromTable( GVarFuncs );

    InitGlobalBag(&breakpoint_functions, "src/debugger.cc:breakpoint_functions");
    InitGlobalBag(&next_step_function, "src/debugger.cc:next_step_function");
    InitGlobalBag(&every_step_function, "src/debugger.cc:every_step_function");
    InitGlobalBag(&next_enter_function, "src/debugger.cc:next_enter_function");
    InitGlobalBag(&every_enter_function, "src/debugger.cc:every_enter_function");
    InitGlobalBag(&next_leave_function, "src/debugger.cc:next_leave_function");
    InitGlobalBag(&every_leave_function, "src/debugger.cc:every_leave_function");

    /* return success                                                      */
    return 0;
}

/******************************************************************************
*F  InitLibrary( <module> ) . . . . . . .  initialise library data structures
*/
static Int InitLibrary( StructInitInfo *module )
{
    /* init filters and functions */
    InitGVarFuncsFromTable( GVarFuncs );

    breakpoint_functions = NEW_PLIST(T_PLIST, 0);

    RegisterThrowObserver(resetDebuggerOnThrow);
    RegisterBreakloopObserver(resetDebuggerOnBreakLoop);


    /* return success                                                      */
    return 0;
}

/******************************************************************************
*F  InitInfopl()  . . . . . . . . . . . . . . . . . table of init functions
*/
static StructInitInfo module = {
 /* type        = */ MODULE_DYNAMIC,
 /* name        = */ "debugger",
 /* revision_c  = */ 0,
 /* revision_h  = */ 0,
 /* version     = */ 0,
 /* crc         = */ 0,
 /* initKernel  = */ InitKernel,
 /* initLibrary = */ InitLibrary,
 /* checkInit   = */ 0,
 /* preSave     = */ 0,
 /* postSave    = */ 0,
 /* postRestore = */ 0
};

extern "C"
StructInitInfo *Init__Dynamic( void )
{
    return &module;
}
