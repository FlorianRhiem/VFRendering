#include "VFRendering/CombinedRenderer.hxx"

namespace VFRendering {
CombinedRenderer::CombinedRenderer(const View& view, const std::vector<std::shared_ptr<RendererBase>>& renderers) : RendererBase(view), m_renderers(renderers) {}

CombinedRenderer::~CombinedRenderer() {}

void CombinedRenderer::update(bool keep_geometry) {
    for (auto renderer : m_renderers) {
        renderer->update(keep_geometry);
    }
}

void CombinedRenderer::draw(float aspect_ratio) {
    for (auto renderer : m_renderers) {
        renderer->draw(aspect_ratio);
    }
}
void CombinedRenderer::optionsHaveChanged(const std::vector<int>& changed_options) {
    for (auto renderer : m_renderers) {
        renderer->optionsHaveChanged(changed_options);
    }
}

void CombinedRenderer::updateOptions(const Options& options) {
    for (auto renderer : m_renderers) {
        renderer->updateOptions(options);
    }
}
}
