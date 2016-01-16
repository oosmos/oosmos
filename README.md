## OOSMOS

OOSMOS is the Object Oriented State Machine Operating System.

*Threadless Concurrency for C/C++*

### Installing

After cloning or pulling OOSMOS from Github, you'll need to run the 
`populate.py` Python program from the top level directory.  This 
will copy common source files from their master locations (where 
they are source code controlled) into the appropriate `Examples` 
directory locations.  Most files are mastered in the `Classes` 
and `Classes/Tests` directory but many of the `Linux` examples 
are mastered in the `Windows` examples directories.  Refer 
to `populate.py` for a roadmap.

If you'd rather, the `.zip` or `.tar.gz` available from 
the [Download Page](http://oosmos.com/download) have all the files
already pre-populated. 

### Contributing to OOSMOS
We'd love to have you contribute to the OOSMOS code base.  See the
[Contributing to OOSMOS](CONTRIBUTING.md) page.

### Top Level Directory Contents

| File/Directory | Purpose  |
|----------------|----------|
| `Classes` | A directory filled with reusable classes. |
| `clean.py` | A Python script that walks subdirectories cleaning up files that are easily recreated by running 'bld.py'.  |
| `Examples`| A directory filled with OOSMOS examples.  Note that there are more examples in here than are highlighted on the website. |
| `LICENSE.txt` | The GPL Version 2 license. |
| `oosmos.py` | A Python utility script used by other Python scripts in the directory tree.                                        |
| `populate.py` | The master source code for OOSMOS: `oosmos.h` and `oosmos.c`. |
| `UmletPalette`  | A directory that contains a custom, simplified, palette of statechart symbols. |
