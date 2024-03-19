# README

## Summary

`hello_vbo' is a demonstration program that renders a colored tetrahedron that rotates around the X, Y, and Z axis to make it appear as though it is wobbling. It is drawn twice. First it is drawn as a colored triangle strip and then drawn as a collection of black lines.

Building

To compile the program, run `make' on OS X or Ubuntu. The Makefile is compatible with GNU Make and not BSD Make. The list of make targets are:
 . all         build all (default)
 . clean       remove intermediate files, core files, backup files
 . spotless    clean + remove all dependency files

If using Ubuntu and you need to install the required libraries, the command below should be all you need:

```
sudo apt install -y libglm-dev libglew-dev libglfw3-dev libtinyobjloader-dev libsdl2-mixer-dev
```

If you don't have a compiler installed, the following command ought to give you the basics:

```
sudo apt install -y --no-install-recommends build-essential clang clang-format clang-tidy python3-pexpect libgmock-dev libgtest-dev git ca-certificates
```

Execution

To run the program, run `hello_glfw'. It does not require any command line options.

The program has the following user interface:
 . esc: quit
It does not respond to any other keyboard or mouse events.

