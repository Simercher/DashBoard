#include "infra/ClockWithQTimer.h"

void ClockWithQTimer::start() {
    m_timer.start();
}
void ClockWithQTimer::stop() {
    // optional: 若有 stop 行為可實作
}
qint64 ClockWithQTimer::elapsedMs() const {
    return m_timer.elapsed();
}