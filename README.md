# Vector Field Rendering

**libvfrendering** is a C++ library for rendering vectorfields using OpenGL. Originally developed for [spirit](https://github.com/spirit-code/spirit) and based on [WegGLSpins.js](https://github.com/FlorianRhiem/WebGLSpins.js), it has an extendable architecture and currently offers renderer implementations for:
- glyph-based vector field representations as arrows
- colormapped surface and isosurface rendering
- mapping vector directions onto a sphere

The library is still very much a work-in-progress, so its API is not yet stable and there are still several features missing that will be added in later releases. If you miss a feature or have another idea on how to improve libvfrendering, please open an issue or pull request!

## Getting Started

To use **libvfrendering**, you need to perform the following steps:

1. Include \<VectorfieldRendering/View.hxx\>
2. Create a *VectorfieldRendering::Geometry*
3. Read or calculate the vector directions
4. Pass geometry and directions to a *VectorfieldRendering::View*
5. Draw the view in an existing OpenGL context

### 1. Include \<VectorfieldRendering/View.hxx\>

When using **libvfrendering**, you will mostly interact with View objects, so it should be enough to `#include <VectorfieldRendering/View.hxx>`.

### 2. Create a VectorfieldRendering::Geometry

The **geometry describes the positions** on which you evaluated the vector field and how they might form a grid (optional, e.g. for isosurface and surface rendering). You can pass the positions directly to the constructor or call one of the class' static methods.

As an example, this is how you could create a simple, cartesian 30x30x30 geometry, with coordinates between -1 and 1:

``` c++
    auto geometry = VectorfieldRendering::Geometry::cartesianGeometry(
        {30, 30, 30},
        {-1.0, -1.0, -1.0},
        {1.0, 1.0, 1.0}
    );
```

### 3. Read or calculate the vector directions

This step highly depends on your use case. The **directions are stored as a `std::vector<glm::vec3>`**, so they can be created in a simple loop:

```c++
    std::vector<glm::vec3> directions;
    for (int iz = 0; iz < 10; iz++) {
        for (int iy = 0; iy < 10; iy++) {
            for (int ix = 0; ix < 10; ix++) {
                // calculate direction for ix, iy, iz
                directions.push_back(glm::normalize({ix-4.5, iy-4.5, iz-4.5}));
            }
        }
    }
```

As shown here, the directions should be in **C order** when using the `VectorfieldRendering::Geometry` static methods. If you do not know [glm](http://glm.g-truc.net/), think of a `glm::vec3` as a struct containing three floats x, y and z.

### 4. Pass geometry and directions to a VectorfieldRendering::View

The view object is what you will interact most with. It provides an interface to the various renderers and includes functions for handling mouse input.

You can **create a new view** and then **pass the geometry and directions by calling the update method**:

```c++
    VectorfieldRendering::View view;
    view.update(geometry, directions);
```

If the directions changed but the geometry is the same, you can use the **updateVectors method**.

### 5. Draw the view in an existing OpenGL context

To actually see something, you need to create an OpenGL context using a toolkit of your choice, e.g. Qt or GLFW. After creating the context, pass the framebuffer size to the **setFramebufferSize method**. You can then call the **draw method** of the view to render the vector field, either in a loop or only when you update the data.

```c++
    view.draw();
```

For a complete example, including an interactive camera, see demo.cxx.

## Options

To modify the way the vector field is rendered, **libvfrendering** offers a variety of options. To set these, you can create an **VectorfieldRendering::Options** object.

As an example, to use an isosurface renderer with an isovalue of 0.5, you would do the following:

```c++
    VectorfieldRendering::Options options;
    options.set<VectorfieldRendering::View::Option::VISUALIZATION_MODE>(
        VectorfieldRendering::VisualizationMode::ISOSURFACE
    );
    options.set<VectorfieldRendering::IsosurfaceRenderer::Option::ISOVALUE>(
        0.5
    );
    view.updateOptions(options);
```

Whether this way of setting options should be replaced by getters/setters will be evaluated as the API becomes more stable.

## ToDo

- A **EGS plugin** for combining **libvfrendering** with existing **EGS** plugins.
- Methods for reading geometry and directions from data files
- Documentation

See the issues for further information and adding your own requests.
