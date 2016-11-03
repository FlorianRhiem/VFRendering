#ifndef GLWIDGET_H_INCLUDED
#define GLWIDGET_H_INCLUDED

#include <memory>
#include <QWidget>
#include <QOpenGLWidget>

#include <glm/vec3.hpp>
#include <VFRendering/View.hxx>
#include <VFRendering/Geometry.hxx>

namespace VFRendering {
  class View;
}

class VFRenderingWidget : public QOpenGLWidget {
Q_OBJECT

public:
    VFRenderingWidget(QWidget *parent=nullptr);
    virtual ~VFRenderingWidget();
    void update(const VFRendering::Geometry& geometry, const std::vector<glm::vec3>& vectors);
    void updateVectors(const std::vector<glm::vec3>& vectors);
    void updateOptions(const VFRendering::Options& options);
    float getFramerate() const;

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    std::unique_ptr<VFRendering::View> m_view;
    QPoint m_previous_mouse_position;
    VFRendering::Geometry m_geometry;
    std::vector<glm::vec3> m_vectors;
    VFRendering::Options m_options;
};

#endif
