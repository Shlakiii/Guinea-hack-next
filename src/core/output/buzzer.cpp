#include<Arduino.h>
#include<core/config/output.h>

namespace {
uint32_t beep_started_at = 0;
uint32_t beep_duration = 0;
bool buzzer_on = false;
}

void buzzerBegin() {
    pinMode(BUZZER_PIN, OUTPUT);
    buzzer_on = false;
    beep_duration = 0;
}

void beep(int ms) {
    if (ms <= 0) {
        digitalWrite(BUZZER_PIN, LOW);
        buzzer_on = false;
        beep_duration = 0;
        return;
    }

    beep_started_at = millis();
    beep_duration = static_cast<uint32_t>(ms);
    buzzer_on = true;
    digitalWrite(BUZZER_PIN, HIGH);
}

void buzzerUpdate() {
    if (buzzer_on &&
        static_cast<uint32_t>(millis() - beep_started_at) >= beep_duration) {
        digitalWrite(BUZZER_PIN, LOW);
        buzzer_on = false;
    }
}