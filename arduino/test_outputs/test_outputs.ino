#include "DFRobotDFPlayerMini.h"

// Seriële communicatie met DFPlayer Mini
HardwareSerial mySerial(2);
DFRobotDFPlayerMini player;

// DFPlayer pins
static const int PIN_MP3_TX = 17; // TX van ESP32 naar RX van DFPlayer
static const int PIN_MP3_RX = 16; // RX van ESP32 naar TX van DFPlayer

// LED
const int LED_PIN = 2;

void setup() {
  // Seriële monitor starten
  Serial.begin(115200);

  // LED als uitgang instellen
  pinMode(LED_PIN, OUTPUT);

  Serial.println("Output-test gestart");
  Serial.println("DFPlayer wordt opgestart...");

  // Seriële verbinding met DFPlayer starten
  mySerial.begin(9600, SERIAL_8N1, PIN_MP3_RX, PIN_MP3_TX);

  // Even wachten zodat DFPlayer kan opstarten
  delay(1000);

  // DFPlayer initialiseren
  if (!player.begin(mySerial)) {
    Serial.println("FOUT: DFPlayer Mini niet gevonden.");
    Serial.println("Controleer:");
    Serial.println("- TX en RX kruislings aangesloten");
    Serial.println("- VCC aangesloten op 5V");
    Serial.println("- GND gemeenschappelijk");
    Serial.println("- SD-kaart aanwezig");
    while (true);
  }

  Serial.println("DFPlayer Mini gevonden.");

  // Instellingen DFPlayer
  player.setTimeOut(500);
  player.outputDevice(DFPLAYER_DEVICE_SD);
  player.EQ(DFPLAYER_EQ_NORMAL);
  player.volume(25);

  Serial.println("Volume ingesteld op 25.");
  Serial.println("Start automatische output-test...");
}

void loop() {
  // LED aan
  Serial.println("LED AAN");
  digitalWrite(LED_PIN, HIGH);
  delay(1000);

  // LED uit
  Serial.println("LED UIT");
  digitalWrite(LED_PIN, LOW);
  delay(1000);

  // Track 1 afspelen
  Serial.println("Track 1 wordt afgespeeld.");
  player.play(1);
  delay(5000);

  // Pauzeren
  Serial.println("Audio pauzeren.");
  player.pause();
  delay(2000);

  // Hervatten
  Serial.println("Audio hervatten.");
  player.start();
  delay(5000);

  // Track 2 afspelen
  Serial.println("Track 2 wordt afgespeeld.");
  player.play(2);
  delay(5000);

  // Loop testen
  Serial.println("Track 1 wordt in loop afgespeeld.");
  digitalWrite(LED_PIN, HIGH);
  player.loop(1);
  delay(8000);

  // Loop stoppen door track opnieuw normaal af te spelen
  Serial.println("Loop stoppen. Track 1 normaal afspelen.");
  digitalWrite(LED_PIN, LOW);
  player.play(1);
  delay(5000);

  Serial.println("Output-test wordt opnieuw gestart.");
  delay(2000);
}
