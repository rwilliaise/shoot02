# shoot02

Toy game engine. Loads Quake `.map` files and most 3D model file formats.

My apologies for the messy file structure.

## Building

The code uses Unix Makefiles to build. Required system libraries are
[GLFW][glfw3_l] and [Assimp][assimp_l]. Both can be found on `pacman`.

```
make
./shoot02
```

```
pacman -S glfw assimp
```

May only build on Linux - have not tried Windows, but should work with mingw
given that `glfw3`, `assimp`, and GL bindings are installed.

    [glfw3_l]: https://github.com/glfw/glfw
    [assimp_l]: https://github.com/assimp/assimp

