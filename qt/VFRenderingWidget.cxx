#include "VFRenderingWidget.hxx"

#include <vector>
#include <QWheelEvent>

VFRenderingWidget::VFRenderingWidget(QWidget *parent) : QOpenGLWidget(parent) {}

VFRenderingWidget::~VFRenderingWidget() {}

void VFRenderingWidget::initializeGL() {
  setMouseTracking(true);
  m_view = std::unique_ptr<VFRendering::View>(new VFRendering::View());
  m_view->update(m_geometry, m_vectors);
  m_view->updateOptions(m_options);
}

void VFRenderingWidget::resizeGL(int width, int height) {
  if (m_view) {
    m_view->setFramebufferSize(width, height);
  }
}

void VFRenderingWidget::update(const VFRendering::Geometry& geometry, const std::vector<glm::vec3>& vectors) {
  if (m_view) {
    m_view->update(geometry, vectors);
  } else {
    m_geometry = geometry;
    m_vectors = vectors;
  }
}


void VFRenderingWidget::updateVectors(const std::vector<glm::vec3>& vectors) {
  if (m_view) {
    m_view->updateVectors(vectors);
  } else {
    m_vectors = vectors;
  }
}

void VFRenderingWidget::updateOptions(const VFRendering::Options& options) {
  if (m_view) {
    m_view->updateOptions(options);
  } else {
    m_options = options;
  }
}

void VFRenderingWidget::paintGL() {
  if (m_view) {
    m_view->draw();
  }
}

float VFRenderingWidget::getFramerate() const {
  if (m_view) {
    return m_view->getFramerate();
  } else {
    return 0;
  }
}

void VFRenderingWidget::wheelEvent(QWheelEvent *event) {
  float delta = event->angleDelta().y()*0.1;
  if (m_view) {
    m_view->mouseScroll(delta);
    ((QWidget *)this)->update();
  }
}

void VFRenderingWidget::mousePressEvent(QMouseEvent *event) {
  m_previous_mouse_position = event->pos();
}

void VFRenderingWidget::mouseMoveEvent(QMouseEvent *event) {
  glm::vec2 current_mouse_position(event->pos().x(), event->pos().y());
  glm::vec2 previous_mouse_position(m_previous_mouse_position.x(), m_previous_mouse_position.y());
  m_previous_mouse_position = event->pos();
  
  if (event->buttons() & Qt::LeftButton) {
    if (m_view) {
      auto movement_mode = VFRendering::CameraMovementModes::ROTATE;
      if (event->modifiers() & Qt::AltModifier) {
        movement_mode = VFRendering::CameraMovementModes::TRANSLATE;
      }
      m_view->mouseMove(previous_mouse_position, current_mouse_position, movement_mode);
      ((QWidget *)this)->update();
    }
  }
}
