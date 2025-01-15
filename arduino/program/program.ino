struct Sensor {
  char trigger, echo;
};

struct Interval {
  float min, max;
};

constexpr int count = 3;
constexpr float timeToCm = 0.01723f;
constexpr int soundFrecquency = 440;
constexpr struct Interval limits = { 5.0f, 200.0f };
struct Interval distance = { 10.0f, 50.0f };
struct Interval frecquency = { 50.0f, 200.0f };
constexpr Sensor sensors[count] = { { 2, 3 }, { 4, 5 }, { 6, 7 } };
constexpr char outPin = 8;
constexpr float lerpFactor = 0.5f;
constexpr int defaultDelay = 50;
float lastDistances[count] = { 0, 0, 0 };

#define MIN(a, b) ((a) < (b) ? (a) : (b))

long readUltrasonicTime(struct Sensor sensor) {
  pinMode(sensor.trigger, OUTPUT);
  digitalWrite(sensor.trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(sensor.trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(sensor.trigger, LOW);
  pinMode(sensor.echo, INPUT);
  return pulseIn(sensor.echo, HIGH, 30000);  // Timeout at 30ms
}

float mapDistance(float val) {
  return frecquency.min + ((val - distance.min) / (distance.max - distance.min)) * (frecquency.max - frecquency.min);
}

bool changeInterval(const String& message) {
  if (message.length() != 2) {
    return false;
  }
  if ((message.charAt(0) != 'l' && message.charAt(0) != 'r')
    || (message.charAt(1) != 'i' && message.charAt(1) != 'd')) {
        return false;
  }

  struct Interval changed = distance;

  float& toModify = message.charAt(0) == 'r' ? changed.max : changed.min;
  float delta = message.charAt(1) == 'i' ? 5.0f : -5.0f;

  toModify += delta;

  if (changed.min >= limits.min && changed.max <= limits.max && changed.min < changed.max) {
    distance = changed;
    return true;
  }

  return false;
}

void setup() {
  Serial.begin(9600);
  pinMode(outPin, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    String message = Serial.readStringUntil('\n'); 
    bool result = changeInterval(message);
    Serial.println(message + result + ' ' + distance.min + ' ' + distance.max);
  }

  for (int i = 0; i < count; i++) {
    long cm = timeToCm * readUltrasonicTime(sensors[i]);

    if (distance.min <= cm && cm <= distance.max) {
      lastDistances[i] = cm;
    } else {
      lastDistances[i] = cm;
    }
  }

  float minDistance = distance.max + 1;
  for (int i = 0; i < count; i++) {
    if (lastDistances[i] != 0) {
      minDistance = MIN(lastDistances[i], minDistance);
    }
  }

  if (distance.min <= minDistance && minDistance <= distance.max) {
    float mappedDistance = mapDistance(minDistance);

    tone(outPin, soundFrecquency);
    delay(mappedDistance);
    noTone(outPin);
    delay(mappedDistance);

  } else if (distance.min > minDistance) {
    tone(outPin, soundFrecquency);
    delay(defaultDelay);

  } else {
    noTone(outPin);
    delay(defaultDelay);
  }
}
