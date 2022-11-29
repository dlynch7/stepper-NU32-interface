# stepper-NU32-interface
Basic interface between NU32 devboard and stepper motor driver

## Description
This project implements a simple interface with a stepper motor driver (e.g., the STR8 from Applied Motion Products) using the following logic-level digital commands:
* ENABLE
* STEP
* DIRECTION

Many drivers also expose a FAULT signal.
This project currently does not monitor the FAULT signal, but adding this feature is straightforward.

This project provides a text-based user interface consisting of various options which the user can select via a serial console (e.g., screen, PuTTY).
This menu includes options to enable/disable the motor, enable/disable trapezoidal velocity profiles, and an option to move a user-specified number of steps.

## Project Structure
This project contains the following C and header files:
* `main.c`: the program entry point; implements user interface via switch statement inside an infinite while loop
* `stepper.{c,h}`: contains functions to interact with the stepper motor driver
* `util.{c,h}`: contains helpful functions whose use can extend beyond any single library.
* `NU32.{c,h}`: the NU32 library (see [http://hades.mech.northwestern.edu/index.php/NU32_Software](http://hades.mech.northwestern.edu/index.php/NU32_Software)).

## Notes
This project was developed around the [STR8 driver from Applied Motion Products](https://www.applied-motion.com/products/stepper-drives/str8).
The STR8 accepts any voltage from 5V to 24 V as digital high, but the NU32 outputs 3.3 V as digital high.
To circumvent this mismatch, I have configured the relevant NU32 digital output pins (E4, E5, and E6) as open-drain outputs; external pull-up resistors (R > 500 Ohms) are required to pull each output up to 5 V.
