# Geometry Library Specification and Selection

Selecting a geometry library for the toolpather is tricky; it will form the foundation of our internal data model and will need to supply robust and efficient algorithms for certain geometric operations. Our current thinking is to select a library that is as simple to use as possible and then substitute in or construct a more advanced library when it becomes necessary to do so.

## Requirements

The geometry library should provide the following primitives:

* 3D triangle

  Represents a mesh element. Ideally, the model should provide a face normal or at least have a winding convention to indicate interior and exterior sides.
  
* 2D path

  Represents an outer or inner perimeter for a region. The ordering of points on the perimeter should indicate the sense of the perimeter.
  
* 2D region

  A composite of several paths indicating an enclosed region of a plane.

The geometry library should provide the following algorithms:

* 2D path/region intersection

  A nlg(n) algorithm for resolving self-intersection and intersecting paths with each other.
  
* 2D inset

  Inset or outset a path with nlg(n) complexity.

## Options

*   BGL

    A very primitive library built for Mandoline. Lacks any sophisticated algorithms.
  
*   Fab@Home

    A much closer match to our current needs. Has inefficient algorithms and uses templates heavily.
  
*   Boost Geometry Library

    A powerful template-based library that implements most algorithms efficiently. Robustness and stability are still unknown.
  
*   CGAL
   
    A well-tested library that provides all our needs. Most algorithms are licensed under the old trolltech license. Uses arbitrary-precision math and thus is slower than it could be.

## Current Decision

We're starting with BGL with the expectation of switching it out shortly.
