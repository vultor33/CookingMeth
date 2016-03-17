/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "edge.h"
#include "atom.h"
#include "graphwidget.h"
#include "atomstruct.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QString>
#include <QDebug>

Atom::Atom(GraphWidget *graphWidget, struct atomType atomIn)
    : graph(graphWidget)
{
    setFlag(QGraphicsItem::ItemIgnoresTransformations); // a luz nao pode rodar.
    setFlag(ItemSendsGeometryChanges); // quando o cara e movimentado voce manda um aviso
    setCacheMode(DeviceCoordinateCache);// otimiza renderizacao
    setZValue(-1);

    //characteristics
    vel.setX(0);
    vel.setY(0);
    xInitialDraw = -atomIn.r;
    yInitialDraw = xInitialDraw;
    horizSize = -2 * xInitialDraw;
    vertSize = -2 * xInitialDraw;
    adjustBoundingSize = 2;

    lightColor.setNamedColor(atomIn.lightColor);
    darkColor.setNamedColor(atomIn.darkColor);

    if(atomIn.atomName != "")
    {
        name = new QGraphicsSimpleTextItem(this);
        name->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        name->setText(atomIn.atomName);
        name->setPos((int)(xInitialDraw/2),(int)(yInitialDraw/2));
        name->setFont(QFont("Times", -xInitialDraw, QFont::Bold));
        name->hide();
    }

}

void Atom::addEdge(Edge *edge)
{
    edgeList << edge;
    edge->adjust();
}

QList<Edge *> Atom::edges() const
{
    return edgeList;
}
//! [1]

void Atom::setVel(QPointF newVel)
{
    vel = newVel;
}

void Atom::changeVel(QPointF addVel)
{
    vel += addVel;
}

void Atom::showHideLabels(bool showLabel)
{
    if(showLabel)
        name->show();
    else
        name->hide();
}

void Atom::invertVel(int bounceType)
{
    switch(bounceType)
    {
    case 1:
        setVel(QPointF(-vel.x(),vel.y()));
        break;
    case 2:
        setVel(QPointF(vel.x(),-vel.y()));
        break;
    case 3:
        setVel(QPointF(-vel.x(),-vel.y()));
        break;
    }
}


QPointF Atom::getVel()
{
    return vel;
}

qreal Atom::getRadius()
{
    return -xInitialDraw;
}

int Atom::checkBounce()
{
    QRectF sceneRect = scene()->sceneRect();
    int bounce = 0;
    if(
            ((sceneRect.left() + xInitialDraw + horizSize) > newPos.x())||
            ((sceneRect.right() - xInitialDraw - horizSize) < newPos.x()))
        bounce += 1;
    if(
            ((sceneRect.top() + yInitialDraw + vertSize) > newPos.y())||
            ((sceneRect.bottom() - yInitialDraw - vertSize) < newPos.y()))
        bounce += 2;

    return bounce;
}

void Atom::calculateForces()
{
    newPos = pos() + vel;
}



bool Atom::advance()
{
    if (newPos == pos())
        return false;

    setPos(newPos);
    return true;
}

QRectF Atom::boundingRect() const
{
    return QRectF(
                xInitialDraw - adjustBoundingSize ,
                yInitialDraw - adjustBoundingSize,
                horizSize + adjustBoundingSize + 3,
                vertSize + adjustBoundingSize + 3);
}

// SE NAO DEFINIR ISSO AQUI A FIGURA VIRA UM RETANGULO
// AI VOCE CLICA NO RETANGULO ELE ENTENDE QUE E NA BOLINHA.
QPainterPath Atom::shape() const
{
    QPainterPath path;
    path.addEllipse(xInitialDraw, yInitialDraw, horizSize, vertSize);
    return path;
}

void Atom::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkBlue);
    painter->drawEllipse(xInitialDraw +3, yInitialDraw +3, horizSize, vertSize);

    QRadialGradient gradient((int)(xInitialDraw/3), (int)(xInitialDraw/3), (int)(horizSize/2));
    gradient.setColorAt(0, lightColor);
    gradient.setColorAt(1, darkColor);

    painter->setBrush(gradient);
    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(xInitialDraw, xInitialDraw, horizSize, vertSize);
}

QVariant Atom::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionHasChanged:
        foreach (Edge *edge, edgeList)
            edge->adjust();
        graph->itemMoved();
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void Atom::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

void Atom::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

