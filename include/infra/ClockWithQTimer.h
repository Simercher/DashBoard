#pragma once

#include "core/IClock.h"
#include <QElapsedTimer>

class ClockWithQTimer : public IClock {
public:
    void start() override;
    void stop() override;
    qint64 elapsedMs() const override;

private:
    QElapsedTimer m_timer;
};
