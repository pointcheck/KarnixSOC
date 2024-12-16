# Karnix SoC Hardware Abstraction Layer

This directory contains a set of HAL files to interact with Karnix SoC hardware.

Provided Makefile file serves as an example of how typical program can be build using this HAL.

Calling ```make``` from this directory will result in link-time error because no main() function is provided. It is your responsibolity to provide main function for your application. To do so, just copy this Makefile to a separate directory (with your project) and adjust to your needs, also provide main.c in ./src subdirectory.
