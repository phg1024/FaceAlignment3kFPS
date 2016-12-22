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

#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace libface;

MainWindow::MainWindow(QWidget* parent)
          : QMainWindow(parent),
            ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->connect(ui->openImageBtn, SIGNAL(clicked()), this, SLOT(openImage()));
    this->connect(ui->openConfigBtn, SIGNAL(clicked()), this, SLOT(openConfig()));
    this->connect(ui->detectFacesBtn, SIGNAL(clicked()), this, SLOT(detectFaces()));
    this->connect(ui->recogniseBtn, SIGNAL(clicked()), this, SLOT(recognise()));
    this->connect(ui->updateDatabaseBtn, SIGNAL(clicked()), this, SLOT(updateConfig()));
    this->connect(ui->saveConfigBtn, SIGNAL(clicked()), this, SLOT(saveConfig()));

    myScene = new QGraphicsScene();

    QHBoxLayout* layout = new QHBoxLayout;
    myView              = new QGraphicsView(myScene);
    layout->addWidget(myView);

    ui->widget->setLayout(layout);

    myView->setRenderHints(QPainter::Antialiasing);
    myView->show();


    libFace = new LibFace(libface::ALL,QDir::currentPath().toStdString());

    ui->configLocation->setText(QDir::currentPath());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent* e)
{
    QMainWindow::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void MainWindow::openImage()
{
    QString file = QFileDialog::getOpenFileName(this,
            tr("Open Image"), QDir::currentPath(), tr("Image Files (*.png *.jpg *.bmp)"));

    clearScene();

    currentPhoto   = string(file.toAscii().data());
    QPixmap* photo = new QPixmap(file);
    lastPhotoItem  = new QGraphicsPixmapItem(*photo);

    if(1.*ui->widget->width()/photo->width() < 1.*ui->widget->height()/photo->height())
    {

        scale = 1.*ui->widget->width()/photo->width();
    }
    else
    {
        scale = 1.*ui->widget->height()/photo->height();
    }

    lastPhotoItem->setScale(scale);

    myScene->addItem(lastPhotoItem);
}

void MainWindow::openConfig()
{
    QString directory = QFileDialog::getExistingDirectory(this,tr("Select Config Directory"),QDir::currentPath());

    ui->configLocation->setText(directory);

    libFace = new LibFace(ALL,directory.toStdString());
}

void MainWindow::detectFaces()
{
    int i;
    currentFaces = libFace->detectFaces(currentPhoto);
    int size     = currentFaces.size();

    for (i=0 ; i<size ; i++)
    {
        Face face          = currentFaces.at(i);
        FaceItem* faceItem = new FaceItem(0, 
                                          myScene,face.getX1()*scale, 
                                          face.getY1()*scale,
                                          (face.getX2()-face.getX1())*scale, 
                                          (face.getY2()-face.getY1())*scale);

        //cout << "Face:\t(" << face.getX1()*scale << ","<<face.getY1()*scale <<")" <<endl;
    }
}

void MainWindow::updateConfig()
{
    libFace->update(&currentFaces);
}

void MainWindow::clearScene()
{
    QList<QGraphicsItem*> list = myScene->items();

    int i;

    for(i=0;i<list.size();i++)
    {
        myScene->removeItem(list.at(i));
    }
}

void MainWindow::recognise()
{
    libFace->recognise(&currentFaces,1);

    int i;
    for(i=0;i<currentFaces.size();i++)
    	printf("Face ID: %d\n",currentFaces.at(i).getId());

    //TODO: create mapping to items.
}

void MainWindow::saveConfig()
{
    //libFace->loadConfig(libFace->getConfig());
    libFace->saveConfig(QDir::currentPath().toStdString());
}
