Tiny3D
======

Tiny3D is one library to work with 2D/3D graphics.

It uses PSL1GHT and install the libraries in PSL1GHT/lib, C Header files
in PSL1GHT/include

see the 'docs' folder to know more

Credits
-------

    Hermes         - Author
    ElSemi         - Vertex Program Compiler and other useful sample code
    HACKERCHANNEL  - PSL1GHT

License
-------
    
    It use the same PSL1GHT license (it have one? XD)

Environment
-----------

    libtiny3d.a -> the library
    
    tiny3d.h    -> 2D/3D functions

    matrix.h    -> Math support for matrix


Building
--------

You need the environment variable $PSL1GHT defined

    cd tiny3d
    make
    
It makes and install libs and samples.

Remember you can descend later to the samples folder to compile ONLY the samples:

    cd samples
    make
    make pkg

Current Status
--------------

Added libfont.a with support for fonts based in bitmaps arrays of characters

It works with four samples: spheres3D, sprites2D, surfaces and fonts
