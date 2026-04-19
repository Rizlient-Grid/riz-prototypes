#include <Arduino.h>

constexpr uint8_t kPwmOutPin = 9;   // OC1A
constexpr uint8_t kPwmReadPin = 2;  // External interrupt pin
constexpr uint32_t kPwmFrequencyHz = 20000;
constexpr uint8_t kDutyPercent = 50;
constexpr uint16_t kTimerTop = 799;
constexpr uint16_t kTimerCompare = 7;

volatile uint32_t gRiseMicros = 0;
volatile uint32_t gHighTimeMicros = 0;
volatile uint32_t gPeriodMicros = 0;
volatile bool gHasSample = false;

/*
 * Self-test PWM:
 * 1. Generate 20kHz, 50% duty PWM on D9 using Timer1.
 * 2. Read it back on D2 using an interrupt.
 * 3. Use a jumper wire to connect D9 to D2.
 */

void handlePwmEdge() {
  static uint32_t lastRiseMicros = 0;
  const uint32_t now = micros();

  if (digitalRead(kPwmReadPin) == HIGH) {
    gRiseMicros = now;
    if (lastRiseMicros != 0) {
      gPeriodMicros = now - lastRiseMicros;
      gHasSample = true;
    }
    lastRiseMicros = now;
  } else {
    gHighTimeMicros = now - gRiseMicros;
  }
}

void setup() {
  pinMode(kPwmOutPin, OUTPUT);
  pinMode(kPwmReadPin, INPUT);
  Serial.begin(115200);

  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  ICR1 = kTimerTop;
  OCR1A = kTimerCompare;
  TCCR1A = _BV(COM1A1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);
  sei();

#if 0
  attachInterrupt(digitalPinToInterrupt(kPwmReadPin), handlePwmEdge, CHANGE);
  Serial.println("PWM self-test started");
  Serial.print("Connect D");
  Serial.print(kPwmOutPin);
  Serial.print(" -> D");
  Serial.println(kPwmReadPin);
#endif
}

void loop() {
#if 0
  static uint32_t lastPrintMs = 0;
  const uint32_t nowMs = millis();

  if (nowMs - lastPrintMs >= 1000) {
    lastPrintMs = nowMs;

    noInterrupts();
    const uint32_t periodMicros = gPeriodMicros;
    const uint32_t highTimeMicros = gHighTimeMicros;
    const bool hasSample = gHasSample;
    interrupts();

    Serial.print("Target: ");
    Serial.print(kPwmFrequencyHz);
    Serial.print(" Hz, ");
    Serial.print(kDutyPercent);
    Serial.println("%");

    if (hasSample && periodMicros > 0) {
      const float measuredFrequency = 1000000.0f / periodMicros;
      const float measuredDuty = (highTimeMicros * 100.0f) / periodMicros;

      Serial.print("Measured: ");
      Serial.print(measuredFrequency, 1);
      Serial.print(" Hz, duty: ");
      Serial.print(measuredDuty, 1);
      Serial.println("%");
    } else {
      Serial.println("Measured: no signal, check jumper wiring");
    }
  }
#endif
}
