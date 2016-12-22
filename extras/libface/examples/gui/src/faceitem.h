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

#ifndef FACEITEM_H
#define FACEITEM_H

#include <QGraphicsItem>
#include <QObject>
#include <QGraphicsRectItem>
#include <QBrush>
#include <QFont>

class FaceItem : public QObject, public QGraphicsItem

{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)


public:

    FaceItem(QGraphicsItem* parent = 0, QGraphicsScene* scene = 0, 
             double x = 0, double y = 0, double width = 0, double height = 0, QString name = QString("unknown"));

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget);
    void setText(QString newName);

Q_SIGNALS:

public Q_SLOTS:

private:

    QGraphicsRectItem* faceRect;
    QGraphicsTextItem* faceName;
};

#endif // FACEITEM_H
