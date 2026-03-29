#include "MainWindow.h"
#include "GLWidget.h"

#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("3D Viewer");

    glWidget = new GLWidget(this);
    setCentralWidget(glWidget);

    QMenu *fileMenu = menuBar()->addMenu("&File");

    QAction *openAction = fileMenu->addAction("&Open...");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    fileMenu->addSeparator();

    QAction *exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    QToolBar *toolbar = addToolBar("Main");
    toolbar->addAction(openAction);

    QMenu *viewMenu = menuBar()->addMenu("&View");

    m_wireframeAction = viewMenu->addAction("&Wireframe");
    m_wireframeAction->setCheckable(true);
    m_wireframeAction->setShortcut(QKeySequence(Qt::Key_W));
    connect(m_wireframeAction, &QAction::toggled, this, [this](bool checked) {
        glWidget->setWireframe(checked);
    });

    statusBar()->showMessage("Ready - File > Open to load a 3D model (.obj, .stl, .3mf)");
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Open 3D Model",
        QString(),
        "3D Models (*.obj *.stl *.3mf *.fbx *.gltf *.glb);;OBJ (*.obj);;STL (*.stl);;3MF (*.3mf);;FBX (*.fbx);;glTF (*.gltf *.glb);;All Files (*)");

    if (!filePath.isEmpty()) {
        if (glWidget->loadModel(filePath.toStdString())) {
            QString info = QString("Loaded: %1 | Vertices: %2 | Triangles: %3")
                .arg(QFileInfo(filePath).fileName())
                .arg(glWidget->vertexCount())
                .arg(glWidget->triangleCount());
            statusBar()->showMessage(info);
        } else {
            QMessageBox::warning(this, "Error", "Failed to load: " + filePath);
            statusBar()->showMessage("Failed to load model");
        }
    }
}
