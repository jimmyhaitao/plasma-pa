/*
    Copyright 2014-2015 Harald Sitter <sitter@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SINK_H
#define SINK_H

#include "device.h"
#include <pulse/channelmap.h>
#include <canberra.h>

namespace QPulseAudio
{

class Sink : public Device
{
    Q_OBJECT
public:
    explicit Sink(QObject *parent);
    virtual ~Sink();

    void update(const pa_sink_info *info);
    void setVolume(qint64 volume) override;
    void setMuted(bool muted) override;
    void setActivePortIndex(quint32 port_index) override;
    void setChannelVolume(int channel, qint64 volume) override;

    bool isDefault() const override;
    void setDefault(bool enable) override;

    void switchStreams() override;

public Q_SLOTS:
    void testChannel(const QString &name);
};

} // QPulseAudio

#endif // SINK_H
