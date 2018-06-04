#ifndef VFRENDERING_PARALLELEPIPED_RENDERER_HXX
#define VFRENDERING_PARALLELEPIPED_RENDERER_HXX

#include <VFRendering/GlyphRenderer.hxx>

namespace VFRendering {

class ParallelepipedRenderer : public GlyphRenderer {
public:
    enum Option {
        X_LENGTH = 100,
        Y_LENGTH,
        Z_LENGTH
    };

    ParallelepipedRenderer(const View& view, const VectorField& vf);
    virtual void optionsHaveChanged(const std::vector<int>& changed_options) override;
};

namespace Utilities {

template<>
struct Options::Option<ParallelepipedRenderer::Option::X_LENGTH> {
    float default_value = 1.f;
};

template<>
struct Options::Option<ParallelepipedRenderer::Option::Y_LENGTH> {
    float default_value = 1.f;
};

template<>
struct Options::Option<ParallelepipedRenderer::Option::Z_LENGTH> {
    float default_value = 1.f;
};

}

}

#endif
