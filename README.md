# shoot02

![cover image][cover image]

Toy game engine. Loads Quake `.map` files and most 3D model file formats.

My apologies for the messy file structure.

## Building

The code uses Unix Makefiles to build. Required system libraries are
[GLFW][glfw3 link] and [Assimp][assimp link]. Both can be found on `pacman`.

```
make
./shoot02
```

```
pacman -S glfw assimp
```

May only build on Linux - have not tried Windows, but should work with mingw
given that `glfw3`, `assimp`, and GL bindings are installed.

[glfw3 link]: https://github.com/glfw/glfw
[assimp link]: https://github.com/assimp/assimp
[cover image]: https://github.com/rwilliaise/shoot02/raw/main/cover.png

