#pragma once

#include <QMainWindow>

class GLWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void openFile();

private:
    GLWidget *glWidget;
    QAction *m_wireframeAction = nullptr;
};
