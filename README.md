# maze-solver
Maze solver using A* algorithm.

It expects a P3 PPM file without comments as input. **Black** pixels denote wall, **white** pixels are open space, a green pixel indicates the starting point, and a red pixel, the end point.

I recommend using [this program](https://github.com/mcavazotti/maze-maker) to generate the mazes.

## Usage:
```sh
make
./mazeMaker < inputFile.ppm > outputFile.ppm
```
