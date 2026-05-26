// Knoppen
const int BTN_1 = 14; // Reset
const int BTN_2 = 27; // Pauze / Loop-combo
const int BTN_3 = 26; // Pauze / Loop-combo
const int BTN_4 = 25; // Volgende track

// Vorige knopstatussen
bool lastBtn1 = HIGH;
bool lastBtn2 = HIGH;
bool lastBtn3 = HIGH;
bool lastBtn4 = HIGH;

// Debounce
const unsigned long DEBOUNCE_TIME = 200;
unsigned long lastPrintBtn1 = 0;
unsigned long lastPrintBtn2 = 0;
unsigned long lastPrintBtn3 = 0;
unsigned long lastPrintBtn4 = 0;

void setup() {
  Serial.begin(115200);

  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  pinMode(BTN_3, INPUT_PULLUP);
  pinMode(BTN_4, INPUT_PULLUP);

  Serial.println("Input-test gestart");
  Serial.println("Druk op een knop om de status te zien.");
  Serial.println("Omdat INPUT_PULLUP gebruikt wordt:");
  Serial.println("HIGH = niet ingedrukt");
  Serial.println("LOW = ingedrukt");
}

void loop() {
  unsigned long now = millis();

  bool btn1 = digitalRead(BTN_1);
  bool btn2 = digitalRead(BTN_2);
  bool btn3 = digitalRead(BTN_3);
  bool btn4 = digitalRead(BTN_4);

  // Knop 1 indrukken
  if (btn1 == LOW && lastBtn1 == HIGH) {
    if (now - lastPrintBtn1 > DEBOUNCE_TIME) {
      Serial.println("BTN_1 ingedrukt - Resetknop werkt");
      lastPrintBtn1 = now;
    }
  }

  // Knop 1 loslaten
  if (btn1 == HIGH && lastBtn1 == LOW) {
    Serial.println("BTN_1 losgelaten");
  }

  // Knop 2 indrukken
  if (btn2 == LOW && lastBtn2 == HIGH) {
    if (now - lastPrintBtn2 > DEBOUNCE_TIME) {
      Serial.println("BTN_2 ingedrukt - Pauzeknop / combo-knop werkt");
      lastPrintBtn2 = now;
    }
  }

  // Knop 2 loslaten
  if (btn2 == HIGH && lastBtn2 == LOW) {
    Serial.println("BTN_2 losgelaten");
  }

  // Knop 3 indrukken
  if (btn3 == LOW && lastBtn3 == HIGH) {
    if (now - lastPrintBtn3 > DEBOUNCE_TIME) {
      Serial.println("BTN_3 ingedrukt - Pauzeknop / combo-knop werkt");
      lastPrintBtn3 = now;
    }
  }

  // Knop 3 loslaten
  if (btn3 == HIGH && lastBtn3 == LOW) {
    Serial.println("BTN_3 losgelaten");
  }

  // Knop 4 indrukken
  if (btn4 == LOW && lastBtn4 == HIGH) {
    if (now - lastPrintBtn4 > DEBOUNCE_TIME) {
      Serial.println("BTN_4 ingedrukt - Volgende track-knop werkt");
      lastPrintBtn4 = now;
    }
  }

  // Knop 4 loslaten
  if (btn4 == HIGH && lastBtn4 == LOW) {
    Serial.println("BTN_4 losgelaten");
  }

  // Combo knop 2 + knop 3 testen
  if (btn2 == LOW && btn3 == LOW) {
    Serial.println("COMBO gedetecteerd: BTN_2 en BTN_3 tegelijk ingedrukt");
    delay(500);
  }

  // Statussen opslaan
  lastBtn1 = btn1;
  lastBtn2 = btn2;
  lastBtn3 = btn3;
  lastBtn4 = btn4;
}
