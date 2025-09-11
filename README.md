[![CI](https://github.com/gap-packages/debugger/actions/workflows/CI.yml/badge.svg)](https://github.com/gap-packages/debugger/actions/workflows/CI.yml)
[![Code Coverage](https://codecov.io/github/gap-packages/debugger/coverage.svg?branch=master&token=)](https://codecov.io/gh/gap-packages/debugger)

The GAP 4 package `debugger'
==============================

This package provides some common features from debuggers.

At present, there are two main features:

* Breakpoints
 - The function AddBreakpoint(filename, line) will force GAP to break
   when is reaches lines 'line' in file 'filename.

* Controlling when to enter the break loop.
 - Once in the break loop, function BreakNextLine will make GAP break
   on the next line. Users can also break on:
     - BreakNextEnterFunction, BreakEveryEnterFunction
     - BreakNextLeaveFunction, BreakEveryLeaveFunction

* Pretty print the state of variables
 - ShowLocals prints the variables of the current function