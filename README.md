# RobotDiagrams
A header file and associated examples for generating 2-D textbook diagrams of simple robots.

This uses an open source SVG drawing library -- simple svg. Check the license here before using it in your own project!
(NOTE: it has been modified; 'transparent' has been changed to 'none' to ensure the generated SVG file properly handles
transparancy)

The example programs are as follows:
* draw_rr_robot.cpp - draws a simple RR robot.
* generate_robots.cpp - converts all file arguments with an extension of '.robot' to '.svg' format.

# Config file
For generate_robots.cpp, the text format for the .robot files is a series of lines, each which is one of the following.

Base (fixed attachment to the world):
```
base
base <specific width>
```

Link
```
link <length> <theta>
link <length> <theta> <label>
```

Link with base and output frame shown, and theta from previous joint:
```
link_frames <length> <theta>
link_frames <length> <theta> <label>
```

Rotary Joint
```
rjoint
```

Prismatic Joint
```
pjoint
```

End Effector
```
effector
```

# Building

To compile and run the example programs, type
```
g++ <program name> -o <executable name>
```

# TODOS

* prismatic joint
* end effector
* text parser
* frames
* scaling factor
* text labels
