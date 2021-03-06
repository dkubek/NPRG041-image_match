# NSWI041 - image_match

This is a project for the NPRG041 - Programming in C++ class at Faculty of
Mathematics and Physics, Charles University Prague.


## About

``image_match`` is a simple utility for image comparison using the MPEG-7
defined Color Structure Descriptor (CSD). A simple command line user interface
is provided. The major advantage of CSD is that in comparison to other image
descriptors it encodes both color and structural information. The CSD descriptor
is ideal for indexing and quick retrieval. Another advantage is that the result
of the descriptor is deterministic as opposed to some commonly used color
descriptors based for example on K-means clusterization.

Currently supported image types are png, jpg and bmp.


## Installation

The project is compiled into a single executable. For installation you will
need ``cmake`` version at least ``3.11``.

To build the project, run the following commands from the root of the project
directory.
```
> cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
> cd build
> cmake --build .
```

The final executable is then found within the ``build/`` directory.


## Usage

Firstly a database of descriptors needs to be built. As a database, any
directory can be used. This directory will be scanned recursively for images.
A type of the CSD descriptor needs to be chosen as well. CSD provides 4
different descriptor types based on the size of the resulting histogram --- 32,
64, 128, 256.

To generate a database for the type ``128`` CSD run:
```
> image_match generate 128 /path/to/image/directory
```

To compare an image against the generated run:
```
> image_match match /path/to/image /path/to/image/directory
```

## Example

A small image dataset sampled from [Harvard Dataverse Flowers
Dataset](https://dataverse.harvard.edu/file.xhtml?fileId=4105627&version=8.0)
is provided in the ``example/`` directory. For a more complete illustration the
whole dataset can be downloaded.

```
> image_match generate 128 example/
```

We will compare randomly chosen image
``example/flowers/daisy/172882635_4cc7b86731_m.jpg`` against the database.
```
> image_match match example/flowers/daisy/172882635_4cc7b86731_m.jpg example/
```
Note that the image itself does not need to be from the database.

A list of matches will be output to the ordered by increasing similarity (most
similar last). Each match comes a number - *similarity index*. One
disadvantage of CSD is that the similarity index cannot be interpreted easily.
However it follows that the more similar image the smaller the similarity
index (0 for perfect match).
