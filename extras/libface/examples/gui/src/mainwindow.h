/** ===========================================================
 * @file
 *
 * This file is a part of libface project
 * <a href="http://libface.sourceforge.net">http://libface.sourceforge.net</a>
 *
 * @date    2010-10-02
 * @brief   main window.
 *
 * @author Copyright (C) 2010 by Alex Jironkin
 *         <a href="alexjironkin at gmail dot com">alexjironkin at gmail dot com</a>
 * @author Copyright (C) 2010 by Aditya Bhatt
 *         <a href="adityabhatt at gmail dot com">adityabhatt at gmail dot com</a>
 * @author Copyright (C) 2010 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QFileDialog>

#include "../../../src/LibFace.h"
#include "../../../src/Face.h"

#include "faceitem.h"

namespace Ui
{
    class MainWindow;
}

using namespace libface;
using namespace std;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

    void openImage();
    void openConfig();
    void detectFaces();
    void updateConfig();
    void recognise();
    void saveConfig();

protected:

    void changeEvent(QEvent* e);

private:

    void clearScene();

private:

    Ui::MainWindow*      ui;
    QGraphicsScene*      myScene;
    QGraphicsView*       myView;
    QGraphicsPixmapItem* lastPhotoItem;
    LibFace*             libFace;
    string               currentPhoto;
    double               scale;
    vector<Face>         currentFaces;
};

#endif // MAINWINDOW_H
