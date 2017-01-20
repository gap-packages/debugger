/*
 * debugger: Debugging support for GAP
 */

// Include gmp.h *before* switching to C mode, because GMP detects when
// compiled from C++ and then does some things differently, which would
// cause an error if called from within extern "C". But libsing.h
// (indirectly) includes gmp.h ...
#include <gmp.h>

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

// The last location -- so we do not keep triggering on the same line.
static std::pair<Int, Int> prevlocation;

// Checks if we are currently inside a function called by the debugger,
// or inside the break loop, so we should not invoke any more debugging
// functions, to avoid infinite loops.
static Int disable_debugger;

// If GAP ever longjmps, let's re-enable the debugger. This isn't perfect,
// but stops the debugger apparently dying.
extern "C" {
void resetDebuggerOnLongjmp()
{ disable_debugger = 0; }

void resetDebuggerOnBreakLoop(Int i)
{ disable_debugger = i; }
}

void debugVisitStat(Stat stat)
{
    if(disable_debugger)
        return;
    Int file = FILENAMEID_STAT(stat);
    Int line = LINE_STAT(stat);
    std::pair<Int, Int> location(file, line);
    //fprintf(stderr,"XX%d,%d,%d,%d\n", file, line, prevlocation.first, prevlocation.second);
    // skip if not valid
    if(location == std::pair<Int,Int>(0,0))
        return;
    // Check we have moved line
    if(prevlocation == location)
        return;
    prevlocation = location;

    //fprintf(stderr,"Y\n");
    bool hit = false;
    for(int i = 0; i < break_points.size(); ++i)
    {
        if(break_points[i] == location)
        {
            //fprintf(stderr,"!!%d,%d,%d\n", file, line,i);
            hit = true;
            disable_debugger = 1;
            Obj funcobj = ELM_PLIST(breakpoint_functions, i+1);
            typedef Obj(*FuncPtr)(Obj);
            ObjFunc hdlrfunc = HDLR_FUNC(funcobj,0);
            FuncPtr funcptr = reinterpret_cast<FuncPtr>(hdlrfunc);
            (*funcptr)(funcobj);
            disable_debugger = 0;
        }
    }
    //if(hit) fprintf(stderr,"!!!\n");
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

    return 0;
}

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
    return removed;
}

Obj CLEAR_ALL_BREAKPOINTS(Obj self)
{
    breakpoint_functions = NEW_PLIST(T_PLIST, 0);
    return 0;
}

Obj GET_BREAKPOINTS(Obj self)
{
    return GAP_make(break_points);
}

struct InterpreterHooks debugHooks =
{
    debugVisitStat,
    0,
    0,
    0,
    "debugger"
};

Obj ACTIVATE_DEBUGGING(Obj self)
{
    activateHooks(&debugHooks);
    return True;
}

Obj DEACTIVATE_DEBUGGING(Obj self)
{
    deactivateHooks();
    return True;
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
    GVAR_FUNC_TABLE_ENTRY("debugger.c", DEACTIVATE_DEBUGGING, 0, "param, param2"),
    GVAR_FUNC_TABLE_ENTRY("debugger.c", GET_BREAKPOINTS, 0, ""),
    GVAR_FUNC_TABLE_ENTRY("debugger.c", ADD_BREAKPOINT, 3, "file, line, func"),
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

    SignalSyLongjmp(resetDebuggerOnLongjmp);
    SignalBreakloop(resetDebuggerOnBreakLoop);
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
