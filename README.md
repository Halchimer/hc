# HClib

## About

HC lib is my utility library for C99 to C23.
It includes things such as dynamic arrays, an arena allocator with a leak detecction,
queues and linked lists, a hashmap, custom random functions and soon more.

I don't think it works on other compilers that GCC yet because of some of the macros
using GCC specific extensions. I'll try to fix that soon though.

## Goals

I don't have clear goals regarding this library yet, i'll just add to it whatever i consider having
its place in it, this means all the features that i am gonna reuse everywhere.

I juste know i'll implement a hashset, other allocators and finish my string implementation for now.

## Use

This is a single header library, so to use is just copy the `hclib.h` file in your project.
for the library's implementation you just have to define the H_DEFINITION macro before including the header. 
(I suggest you do that in a separate c file so the implementation is available everywhere and not inline.)

The library is organized into modules, all included using a macro definition, which are specified at the top of the header.

I'll probably document this library better later, but since it is mostly a personnal tool it is not a priority. 