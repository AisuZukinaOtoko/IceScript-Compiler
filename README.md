# IceScript-Compiler
A compiler for IceScript; a c-style language.

## Table of Contents
1. [Instructions](#instructions)
2. [Supported Data Types](#supported-data-types)
3. [Project Status](#project-status)

## Instructions
1. Create your program and save it to a file. The file extension does not matter.
2. Open the terminal and enter the following command: "./ipc [path to your program] [additional flags]"
3. Flags include "-r" and "-d".
4. The -r flag outputs the value of every variable at the end of execution.
5. The -d flag outputs the structure of the syntax tree created. Useful for debugging.

## Supported Data Types
1. Integer.

## Project Status
Currently, the project is in the late development stage. Only integers are supported.
Scopes are not a thing... yet.
Functions and classes are not supported. Neither are arrays.
For loops also not supported. They're just fancy while loops anyways... :)
