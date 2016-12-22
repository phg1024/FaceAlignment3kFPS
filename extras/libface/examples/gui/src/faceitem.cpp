/** ===========================================================
 * @file
 *
 * This file is a part of libface project
 * <a href="http://libface.sourceforge.net">http://libface.sourceforge.net</a>
 *
 * @date    2010-10-02
 * @brief   face item.
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

#include "faceitem.h"

FaceItem::FaceItem(QGraphicsItem* parent, QGraphicsScene* scene, 
                   double x, double y, double width, double height, QString name)
        : QGraphicsItem(parent)
{
    faceRect = new QGraphicsRectItem( 0, scene);

    faceRect->setRect(x, y, width, height );

    faceRect->setBrush(QBrush(QColor(QString("red"))));

    faceRect->setOpacity(0.5);

    //faceName = new QGraphicsTextItem(name, 0, scene);
    //faceName->setPos(x-10,y-10);
    //faceName->setFont(QFont("Helvetica"));
}

QRectF FaceItem::boundingRect() const
{
    qreal adjust = 0.5;
    return QRectF(-18 - adjust, -22 - adjust,
                  36 + adjust, 60 + adjust);
}

void FaceItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
}

void FaceItem::setText(QString newName)
{
    faceName->setPlainText(newName);

    //Update?
}
