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

#include "graphwidget.h"
#include "edge.h"
#include "atom.h"
#include "atomstruct.h"

#include <math.h>

#include <QKeyEvent>
#include <QDebug>
#include <vector>

GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), timerId(0)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-250, -250, 490, 490);
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    //scale(qreal(0.8), qreal(0.8));
    //setMinimumSize(500, 500);
    setWindowTitle(tr("Cooking Meth"));

    showLabel = true;
    atomType1 = defineAtom(1);
    atomType2 = defineAtom(9);
    atomType3 = defineAtom(17);
    atom1 = new Atom(this,atomType1);
    atom2 = new Atom(this,atomType3);
    atom3 = new Atom(this,atomType2);
    scene->addItem(atom1);
    scene->addItem(atom2);
    scene->addItem(atom3);
    atom1->setPos(100, 0);
    atom2->setPos(150, 0);
    atom3->setPos(20, 20);
    molecule1.resize(2);
    molecule1[0] = 0;
    molecule1[1] = 1;
    molecule2.resize(1);
    molecule2[0] = 2;

    // Quando o grupo e criado ele fica no zero zero.
    // mol1
    atom4 = new Atom(this,atomType1);
    atom5 = new Atom(this,atomType3);
    atom6 = new Atom(this,atomType2);
    atom4->setPos(-25, 0);
    atom5->setPos(25, 0);
    atom6->setPos(0,0);
    bond4to5 = new Edge(atom4,atom5);
    scene->addItem(atom4);
    scene->addItem(atom5);
    scene->addItem(bond4to5);
    scene->addItem(atom6);

    QList<QGraphicsItem *> mol5;
    mol5 << atom4 << atom5 << bond4to5;
    mol1 = scene->createItemGroup(mol5);
    QList<QGraphicsItem *> mol6;
    mol6 << atom6;
    mol2 = scene->createItemGroup(mol6);

    mol1->setPos(-125,0);
    mol2->setPos(100,-100);

    mol1->setRotation(120);
    //fim do mol1


    atom3->setVel(QPointF(1,-1));
    scene->addItem(new Edge(atom2, atom1));
}

void GraphWidget::itemMoved()
{
    if (!timerId)
        timerId = startTimer(1000 / 25);
}

void GraphWidget::changeMoleculeVelocity(std::vector<int> &mol, QPointF newVel)
{
    for(size_t i = 0; i < mol.size(); i++ )
    {
        nodes[mol[i]]->changeVel(newVel);
    }
}

void GraphWidget::angularVelocity(std::vector<int> &mol)
{
    std::vector<QPointF> allPoints(mol.size());
    qreal xmean = 0;
    qreal ymean = 0;
    for(size_t i = 0; i < mol.size(); i++ )
    {
        xmean += nodes[mol[i]]->pos().x();
        ymean += nodes[mol[i]]->pos().y();
    }
    xmean /= mol.size();
    ymean /= mol.size();

    // new = point - xmean.
    // rotate
    // xmean + new
    qreal angle = 3.14/180;
    for(size_t i = 0; i < mol.size(); i++ )
    {
        qreal vecx = nodes[mol[i]]->pos().x() - xmean;
        qreal vecy = nodes[mol[i]]->pos().y() - ymean;
        qreal newx = vecx * cos(angle) + vecy * sin(angle);
        qreal newy = -vecx * sin(angle) + vecy * cos(angle);
        nodes[mol[i]]->setPos(newx+xmean,newy+ymean);
    }
}

// define center of molecule -> mass center.
// pra rodar e so setar uma velocidade em relacao ao centro de massa.
// preciso definir uma velocidade angular do lado de fora e mover
// cada atomo de acordo com essa velocidade angular.
// como nao vai alterar o centro de massa, a ordem dos movimentos nao importa.



void GraphWidget::showHideLabels()
{
    if(showLabel)
        showLabel = false;
    else
        showLabel = true;

    foreach (Atom *node, nodes)
        node->showHideLabels(showLabel);
}


bool GraphWidget::checkIfMoleculeBounced(std::vector<int> &mol)
{
    bool bounced = false;
    int bounceType;
    for(size_t i = 0; i < mol.size(); i++ )
    {
        bounceType = nodes[mol[i]]->checkBounce();
        if(bounceType > 0)
        {
            bounced = true;
            break;
        }
    }
    if(!bounced)
        return bounced;

    for(size_t i = 0; i < mol.size(); i++ )
    {
        nodes[mol[i]]->invertVel(bounceType);
    }
    return bounced;
}


void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        changeMoleculeVelocity(molecule1,QPointF(0,-1));
        break;
    case Qt::Key_Down:
        changeMoleculeVelocity(molecule1,QPointF(0,1));
        break;
    case Qt::Key_Left:
        changeMoleculeVelocity(molecule1,QPointF(-1,0));
        break;
    case Qt::Key_Right:
        changeMoleculeVelocity(molecule1,QPointF(1,0));
        break;
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_L:
        showHideLabels();
        break;

    case Qt::Key_Space:
    case Qt::Key_Enter:
    default:
        QGraphicsView::keyPressEvent(event);
    }
}


void GraphWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    foreach (QGraphicsItem *item, scene()->items()) {
        if (Atom *node = qgraphicsitem_cast<Atom *>(item))
            nodes << node;
    }

    foreach (Atom *node, nodes)
        node->calculateForces();

    if(!(
                checkIfMoleculeBounced(molecule1)||
                checkIfMoleculeBounced(molecule2)))
    {
        foreach (Atom *node, nodes)
        {
            angularVelocity(molecule1);
            node->advance();
        }
    }
}

#ifndef QT_NO_WHEELEVENT
void GraphWidget::wheelEvent(QWheelEvent *event)
{
    //scaleView(pow((double)2, -event->delta() / 240.0));
}
#endif

void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    /*
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
        painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
        painter->fillRect(bottomShadow, Qt::darkGray);
    */

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::blue);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);

    // Text
    /*
    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
                    sceneRect.width() - 4, sceneRect.height() - 4);
    QString message(tr("Click and drag the nodes around, and zoom with the mouse "
                       "wheel or the '+' and '-' keys"));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(14);
    painter->setFont(font);
    painter->setPen(Qt::lightGray);
    painter->drawText(textRect.translated(2, 2), message);
    painter->setPen(Qt::black);
    painter->drawText(textRect, message);
    */

}


void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

void GraphWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void GraphWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}

struct atomType GraphWidget::defineAtom(int nAtomic)
{
    struct atomType atomOut;
    switch(nAtomic)
    {
    case 1:
        atomOut.r = 6;
        atomOut.lightColor = "#ffffff";
        atomOut.darkColor = "#a0a0a4";
        atomOut.atomName = "H";
        break;
    case 9:
        atomOut.r = 9;
        atomOut.lightColor = "#ff0000";
        atomOut.darkColor = "#800000";
        atomOut.atomName = "F";
        break;
    case 17:
        atomOut.r = 12;
        atomOut.lightColor = "#00ff00";
        atomOut.darkColor = "#008000";
        atomOut.atomName = "Cl";
        break;
    }

    if(!showLabel)
        atomOut.atomName = "";

    return atomOut;
}
