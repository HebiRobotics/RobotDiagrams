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
invisible_base
invisible_base <specific width>
```

Link
```
link <length>
link <length> <label>
invisible_link <length>
```

Frames - x/y axis
```
frames
```

Rotary Joint
```
rjoint <theta>
invisible_rjoint <theta>
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

* scaling factor
* text labels
