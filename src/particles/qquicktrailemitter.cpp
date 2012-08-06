/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtQuick module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qquicktrailemitter_p.h"
#include <private/qqmlengine_p.h>
#include <private/qqmlglobal_p.h>
#include <cmath>
QT_BEGIN_NAMESPACE

/*!
    \qmltype TrailEmitter
    \instantiates QQuickTrailEmitter
    \inqmlmodule QtQuick.Particles 2
    \inherits QQuickParticleEmitter
    \brief Emits logical particles from other logical particles
    \ingroup qtquick-particles

    This element emits logical particles into the ParticleSystem, with the
    starting positions based on those of other logical particles.
*/
QQuickTrailEmitter::QQuickTrailEmitter(QQuickItem *parent) :
    QQuickParticleEmitter(parent)
  , m_particlesPerParticlePerSecond(0)
  , m_lastTimeStamp(0)
  , m_emitterXVariation(0)
  , m_emitterYVariation(0)
  , m_followCount(0)
  , m_emissionExtruder(0)
  , m_defaultEmissionExtruder(new QQuickParticleExtruder(this))
{
    //TODO: If followed increased their size
    connect(this, SIGNAL(followChanged(QString)),
            this, SLOT(recalcParticlesPerSecond()));
    connect(this, SIGNAL(particleDurationChanged(int)),
            this, SLOT(recalcParticlesPerSecond()));
    connect(this, SIGNAL(particlesPerParticlePerSecondChanged(int)),
            this, SLOT(recalcParticlesPerSecond()));
}

/*!
    \qmlproperty string QtQuick.Particles2::TrailEmitter::follow

    The type of logical particle which this is emitting from.
*/
/*!
    \qmlproperty qreal QtQuick.Particles2::TrailEmitter::velocityFromMovement

    If this value is non-zero, then any movement of the emitter will provide additional
    starting velocity to the particles based on the movement. The additional vector will be the
    same angle as the emitter's movement, with a magnitude that is the magnitude of the emitters
    movement multiplied by velocityFromMovement.

    Default value is 0.
*/
/*!
    \qmlproperty Shape QtQuick.Particles2::TrailEmitter::emitShape

    As the area of a TrailEmitter is the area it follows, a separate shape can be provided
    to be the shape it emits out of. This shape has width and height specified by emitWidth
    and emitHeight, and is centered on the followed particle's position.

    The default shape is a filled Rectangle.
*/
/*!
    \qmlproperty real QtQuick.Particles2::TrailEmitter::emitWidth

    The width in pixels the emitShape is scaled to. If set to TrailEmitter.ParticleSize,
    the width will be the current size of the particle being followed.

    Default is 0.
*/
/*!
    \qmlproperty real QtQuick.Particles2::TrailEmitter::emitHeight

    The height in pixels the emitShape is scaled to. If set to TrailEmitter.ParticleSize,
    the height will be the current size of the particle being followed.

    Default is 0.
*/
/*!
    \qmlproperty real QtQuick.Particles2::TrailEmitter::emitRatePerParticle
*/
/*!
    \qmlsignal QtQuick.Particles2::TrailEmitter::onEmitFollowParticles(Array particles, Particle followed)

    This handler is called when particles are emitted from the \a followed particle. \a particles contains an array of particle objects which can be directly manipulated.

    If you use this signal handler, emitParticles will not be emitted.

*/

bool QQuickTrailEmitter::isEmitFollowConnected()
{
    IS_SIGNAL_CONNECTED(this, QQuickTrailEmitter, emitFollowParticles, (QQmlV8Handle,QQmlV8Handle));
}

void QQuickTrailEmitter::recalcParticlesPerSecond(){
    if (!m_system)
        return;
    m_followCount = m_system->groupData[m_system->groupIds[m_follow]]->size();
    if (!m_followCount){
        setParticlesPerSecond(1);//XXX: Fix this horrendous hack, needed so they aren't turned off from start (causes crashes - test that when gone you don't crash with 0 PPPS)
    }else{
        setParticlesPerSecond(m_particlesPerParticlePerSecond * m_followCount);
        m_lastEmission.resize(m_followCount);
        m_lastEmission.fill(m_lastTimeStamp);
    }
}

void QQuickTrailEmitter::reset()
{
    m_followCount = 0;
}

void QQuickTrailEmitter::emitWindow(int timeStamp)
{
    if (m_system == 0)
        return;
    if (!m_enabled && !m_pulseLeft && m_burstQueue.isEmpty())
        return;
    if (m_followCount != m_system->groupData[m_system->groupIds[m_follow]]->size()){
        qreal oldPPS = m_particlesPerSecond;
        recalcParticlesPerSecond();
        if (m_particlesPerSecond != oldPPS)
            return;//system may need to update
    }

    if (m_pulseLeft){
        m_pulseLeft -= timeStamp - m_lastTimeStamp * 1000.;
        if (m_pulseLeft < 0){
            timeStamp += m_pulseLeft;
            m_pulseLeft = 0;
        }
    }

    //TODO: Implement startTime and velocityFromMovement
    qreal time = timeStamp / 1000.;
    qreal particleRatio = 1. / m_particlesPerParticlePerSecond;
    qreal pt;
    qreal maxLife = (m_particleDuration + m_particleDurationVariation)/1000.0;

    //Have to map it into this system, because particlesystem automaps it back
    QPointF offset = m_system->mapFromItem(this, QPointF(0, 0));
    qreal sizeAtEnd = m_particleEndSize >= 0 ? m_particleEndSize : m_particleSize;

    int gId = m_system->groupIds[m_follow];
    int gId2 = m_system->groupIds[m_group];
    for (int i=0; i<m_system->groupData[gId]->data.count(); i++) {
        QQuickParticleData *d = m_system->groupData[gId]->data[i];
        if (!d->stillAlive()){
            m_lastEmission[i] = time; //Should only start emitting when it returns to life
            continue;
        }
        pt = m_lastEmission[i];
        if (pt < d->t)
            pt = d->t;
        if (pt + maxLife < time)//We missed so much, that we should skip emiting particles that are dead by now
            pt = time - maxLife;

        if ((width() || height()) && !effectiveExtruder()->contains(QRectF(offset.x(), offset.y(), width(), height()),QPointF(d->curX(), d->curY()))){
            m_lastEmission[d->index] = time;//jump over this time period without emitting, because it's outside
            continue;
        }

        QList<QQuickParticleData*> toEmit;

        while (pt < time || !m_burstQueue.isEmpty()){
            QQuickParticleData* datum = m_system->newDatum(gId2, !m_overwrite);
            if (datum){//else, skip this emission
                datum->e = this;//###useful?

                // Particle timestamp
                datum->t = pt;
                datum->lifeSpan =
                        (m_particleDuration
                         + ((rand() % ((m_particleDurationVariation*2) + 1)) - m_particleDurationVariation))
                        / 1000.0;

                // Particle position
                // Note that burst location doesn't get used for follow emitter
                qreal followT =  pt - d->t;
                qreal followT2 = followT * followT * 0.5;
                qreal eW = m_emitterXVariation < 0 ? d->curSize() : m_emitterXVariation;
                qreal eH = m_emitterYVariation < 0 ? d->curSize() : m_emitterYVariation;
                //Subtract offset, because PS expects this in emitter coordinates
                QRectF boundsRect(d->x - offset.x() + d->vx * followT + d->ax * followT2 - eW/2,
                                  d->y - offset.y() + d->vy * followT + d->ay * followT2 - eH/2,
                                  eW, eH);

                QQuickParticleExtruder* effectiveEmissionExtruder = m_emissionExtruder ? m_emissionExtruder : m_defaultEmissionExtruder;
                const QPointF &newPos = effectiveEmissionExtruder->extrude(boundsRect);
                datum->x = newPos.x();
                datum->y = newPos.y();

                // Particle velocity
                const QPointF &velocity = m_velocity->sample(newPos);
                datum->vx = velocity.x()
                    + m_velocity_from_movement * d->vx;
                datum->vy = velocity.y()
                    + m_velocity_from_movement * d->vy;

                // Particle acceleration
                const QPointF &accel = m_acceleration->sample(newPos);
                datum->ax = accel.x();
                datum->ay = accel.y();

                // Particle size
                float sizeVariation = -m_particleSizeVariation
                        + rand() / float(RAND_MAX) * m_particleSizeVariation * 2;

                float size = qMax((qreal)0.0, m_particleSize + sizeVariation);
                float endSize = qMax((qreal)0.0, sizeAtEnd + sizeVariation);

                datum->size = size * float(m_enabled);
                datum->endSize = endSize * float(m_enabled);

                toEmit << datum;

                m_system->emitParticle(datum);
            }
            if (!m_burstQueue.isEmpty()){
                m_burstQueue.first().first--;
                if (m_burstQueue.first().first <= 0)
                    m_burstQueue.pop_front();
            }else{
                pt += particleRatio;
            }
        }

        foreach (QQuickParticleData* d, toEmit)
            m_system->emitParticle(d);

        if (isEmitConnected() || isEmitFollowConnected()) {
            v8::HandleScope handle_scope;
            v8::Context::Scope scope(QQmlEnginePrivate::getV8Engine(qmlEngine(this))->context());
            v8::Handle<v8::Array> array = v8::Array::New(toEmit.size());
            for (int i=0; i<toEmit.size(); i++)
                array->Set(i, toEmit[i]->v8Value().toHandle());

            if (isEmitFollowConnected())
                emitFollowParticles(QQmlV8Handle::fromHandle(array), d->v8Value());//A chance for many arbitrary JS changes
            else if (isEmitConnected())
                emitParticles(QQmlV8Handle::fromHandle(array));//A chance for arbitrary JS changes
        }
        m_lastEmission[d->index] = pt;
    }

    m_lastTimeStamp = time;
}
QT_END_NAMESPACE
