from PyQt5.QtGui import (
        QOpenGLVersionProfile,
        QOpenGLVertexArrayObject,
        QSurfaceFormat,
    )

from PyQt5.QtWidgets import (
        QApplication,
        QMainWindow,
        QOpenGLWidget
    )


class MainWindow(QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()

        self.widget = glWidget(self)
        self.setCentralWidget(self.widget)


class glWidget(QOpenGLWidget):

    def __init__(self, parent):
        QOpenGLWidget.__init__(self, parent)


    def initializeGL(self):
        import pyVFRendering as vfr
        
        # Geometry
        n_cells = [20, 20, 20]
        geometry = vfr.Geometry.cartesianGeometry(n_cells, [-20, -20, -20], [20, 20, 20])
        
        # Directions
        directions = []
        for iz in range(n_cells[2]):
            for iy in range(n_cells[1]):
                for ix in range(n_cells[0]):
                    directions.append( [0, 0, 1] )
        directions[0] = [1,0,0] # for colormap test

        # View
        self.view = vfr.View()
        self.view.update(geometry, directions)
        
        # ArrowRenderer
        renderer_arrows = vfr.ArrowRenderer(self.view)
        self.renderers = [ (renderer_arrows, [0.0, 0.0, 1.0, 1.0]) ]
        self.view.renderers(self.renderers)

        # Options
        self.options = vfr.Options()
        self.options.set_system_center((geometry.min() + geometry.max())*0.5)
        self.options.set_camera_position([0, -80, 20])
        self.options.set_center_position([0, 0, 0])
        self.options.set_up_vector([0, 0, 1])
        self.options.set_colormap_implementation(vfr.getColormapImplementation(vfr.Colormap.HSV))
        self.view.updateOptions(self.options)


    def paintGL(self):
        self.view.draw()


    def resizeGL(self, width, height):
        self.view.setFramebufferSize(width*self.window().devicePixelRatio(), height*self.window().devicePixelRatio())
        # QTimer::singleShot(1, this, SLOT(update()));
        # self.update()


if __name__ == '__main__':
    # Set default surface format for OpenGL context
    fmt = QSurfaceFormat()
    fmt.setVersion(4, 1)
    fmt.setProfile(QSurfaceFormat.CoreProfile)
    fmt.setSamples(4)
    QSurfaceFormat.setDefaultFormat(fmt)

    # Open the Application Window
    app = QApplication(['PyVFRendering Demo'])
    window = MainWindow()
    window.resize(640, 480)
    window.show()

    # Return
    app.exec_()