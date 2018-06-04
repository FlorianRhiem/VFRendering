#include "VFRendering/ParallelepipedRenderer.hxx"

#include <map>
#include <glm/glm.hpp>

namespace VFRendering {

static void setParallelepipedMeshOptions(GlyphRenderer& renderer, const Options& options);

ParallelepipedRenderer::ParallelepipedRenderer( const View& view, const VectorField& vf) 
    : GlyphRenderer(view, vf)
{
    setParallelepipedMeshOptions( *this, this->options() );
}

void ParallelepipedRenderer::optionsHaveChanged(const std::vector<int>& changed_options) {
    bool update_vertices = false;
    for (auto option_index : changed_options) {
        switch (option_index) {
            case Option::X_LENGTH:
                update_vertices = true;
                break;
            case Option::Y_LENGTH:
                update_vertices = true;
                break;
            case Option::Z_LENGTH:
                update_vertices = true;
                break;
        }
    }
    if (update_vertices) {
        setParallelepipedMeshOptions(*this, options());
    }
    GlyphRenderer::optionsHaveChanged(changed_options);
}

static void setParallelepipedMeshOptions(GlyphRenderer& renderer, const Options& options) {
    auto x_length = options.get<ParallelepipedRenderer::Option::X_LENGTH>();
    auto y_length = options.get<ParallelepipedRenderer::Option::Y_LENGTH>();
    auto z_length = options.get<ParallelepipedRenderer::Option::Z_LENGTH>();
    
    // Enforce valid range
    if (x_length < 0) x_length = 0;
    if (y_length < 0) y_length = 0;
    if (z_length < 0) z_length = 0;
    
    std::vector<std::uint16_t> cube_indices = 
        {0,3,1,1,3,2,
         4,5,7,5,7,6,
         8,10,9,8,11,10,
         12,14,13,12,15,14,
         16,19,17,17,19,18,
         20,21,22,22,23,20};

    std::vector<glm::vec3> cube_vertices = {
        {-1,-1,-1}, {-1,-1, 1}, { 1,-1, 1}, { 1,-1,-1},
        { 1,-1,-1}, { 1,-1, 1}, { 1, 1, 1}, { 1, 1,-1},
        { 1, 1,-1}, { 1, 1, 1}, {-1, 1, 1}, {-1, 1,-1},
        {-1, 1,-1}, {-1, 1, 1}, {-1,-1, 1}, {-1,-1,-1},
        {-1,-1,-1}, { 1,-1,-1}, { 1, 1,-1}, {-1, 1,-1},
        { 1,-1, 1}, { 1, 1, 1}, {-1, 1, 1}, {-1,-1, 1}
    };
    
    for (auto& vertex : cube_vertices) {
        vertex.x *= x_length;
        vertex.y *= y_length;
        vertex.z *= z_length;
    }

    std::vector<glm::vec3> normals = {
        {0,-1,0}, {0,-1,0}, {0,-1,0}, {0,-1,0}, 
        {1,0,0}, {1,0,0}, {1,0,0}, {1,0,0}, 
        {0,1,0}, {0,1,0}, {0,1,0}, {0,1,0}, 
        {-1,0,0}, {-1,0,0}, {-1,0,0}, {-1,0,0}, 
        {0,0,-1}, {0,0,-1}, {0,0,-1}, {0,0,-1},
        {0,0,1}, {0,0,1}, {0,0,1}, {0,0,1}
    };

    for (auto& normal : normals) {
        normal = glm::normalize(normal);
    }

    renderer.setGlyph(cube_vertices, normals, cube_indices);
}

}
