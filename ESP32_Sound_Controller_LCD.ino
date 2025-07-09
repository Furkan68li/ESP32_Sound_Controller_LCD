#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_CS   -1
#define TFT_RST   4
#define TFT_DC    2
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

#define HOPARLOR_PIN 15

WebServer server(80);
const char* ssid = "BIZIM_WIFI";
const char* password = "12345678";

bool sesCalıyor = false;
unsigned long sesBaslangic = 0;
int sesTuru = 0;

int melodi1[] = {523, 659, 784, 1047, 784, 659, 523};
int melodi1_sure[] = {300, 300, 300, 500, 300, 300, 500};
int melodi1_uzunluk = 7;

int melodi2[] = {440, 494, 523, 587, 659, 698, 784};
int melodi2_sure[] = {200, 200, 200, 200, 200, 200, 400};
int melodi2_uzunluk = 7;

int sirenFrekans = 200;
bool sirenYukseliyor = true;

String getWebPage() {
  return R"rawliteral(
    <!DOCTYPE html><html><head><title>ESP32 Ses Kontrol</title><style>
    body { font-family: Arial; text-align: center; background-color: #f0f0f0;}
    button {width: 200px; height: 60px; font-size: 16px; margin: 10px; border-radius: 10px; border: none; cursor: pointer; font-weight: bold;}
    .ses1 { background-color: #4CAF50; color: white; }
    .ses2 { background-color: #2196F3; color: white; }
    .ses3 { background-color: #FF9800; color: white; }
    .ses4 { background-color: #9C27B0; color: white; }
    .durdur { background-color: #f44336; color: white; }
    .reset { background-color: #607D8B; color: white; }
    h2 { color: #333; }
    </style></head><body>
    <h2>ESP32 Ses Kontrol Paneli</h2>
    <button class="ses1" onclick="location.href='/istiklal'"> TR ISTIKLAL MARSI</button><br>
    <button class="ses2" onclick="location.href='/melodi1'"> MELODI 1</button><br>
    <button class="ses3" onclick="location.href='/melodi2'"> MELODI 2</button><br>
    <button class="ses4" onclick="location.href='/siren'"> SIREN</button><br>
    <button class="ses1" onclick="location.href='/sweep'"> SWEEP</button><br>
    <button class="ses2" onclick="location.href='/ac'"> AC</button><br>
    <button class="durdur" onclick="location.href='/kapat'"> KAPAT</button><br>
    <button class="reset" onclick="location.href='/resetle'"> RESETLE</button>
    </body></html>
  )rawliteral";
}

void yazLCD(String mesaj) {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 100);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.println(mesaj);
}

void handleRoot() { server.send(200, "text/html", getWebPage()); }
void handleMelodi1() { yazLCD("Melodi 1 Caliyor"); sesTuru = 1; sesCalıyor = true; server.send(200, "text/html", getWebPage()); }
void handleMelodi2() { yazLCD("Melodi 2 Caliyor"); sesTuru = 2; sesCalıyor = true; server.send(200, "text/html", getWebPage()); }
void handleSiren() { yazLCD("Siren Caliyor"); sesTuru = 3; sesCalıyor = true; sirenFrekans = 200; sirenYukseliyor = true; server.send(200, "text/html", getWebPage()); }
void handleSweep() { yazLCD("Sweep Caliyor"); sesTuru = 4; sesCalıyor = true; server.send(200, "text/html", getWebPage()); }
void handleAc() {
  yazLCD("Sistem Acildi");
  ledcChangeFrequency(HOPARLOR_PIN, 1000, 8);
  ledcWrite(HOPARLOR_PIN, 128);
  delay(100);
  ledcWrite(HOPARLOR_PIN, 0);
  server.send(200, "text/html", getWebPage());
}
void handleKapat() {
  yazLCD("Sistem Kapatildi");
  for (int i = 0; i < 2; i++) {
    ledcChangeFrequency(HOPARLOR_PIN, 800, 8);
    ledcWrite(HOPARLOR_PIN, 128);
    delay(80);
    ledcWrite(HOPARLOR_PIN, 0);
    delay(80);
  }
  server.send(200, "text/html", getWebPage());
}
void handleResetle() {
  yazLCD("Resetlendi");
  for (int i = 0; i < 3; i++) {
    ledcChangeFrequency(HOPARLOR_PIN, 600, 8);
    ledcWrite(HOPARLOR_PIN, 128);
    delay(70);
    ledcWrite(HOPARLOR_PIN, 0);
    delay(70);
  }
  sesCalıyor = false;
  ledcWrite(HOPARLOR_PIN, 0);
  server.send(200, "text/html", getWebPage());
}

void handleIstiklal() {
  yazLCD("Istiklal Marsi");
  sesTuru = 99;
  sesCalıyor = true;
  server.send(200, "text/html", getWebPage());
}

void melodiCal(int melodi[], int sure[], int uzunluk) {
  static int notaIndex = 0;
  static unsigned long sonNotaZamani = 0;
  unsigned long simdikiZaman = millis();
  if (simdikiZaman - sonNotaZamani >= sure[notaIndex]) {
    notaIndex++;
    if (notaIndex >= uzunluk) notaIndex = 0;
    ledcChangeFrequency(HOPARLOR_PIN, melodi[notaIndex], 8);
    ledcWrite(HOPARLOR_PIN, 128);
    sonNotaZamani = simdikiZaman;
  }
}

void sirenCal() {
  static unsigned long sonGuncelleme = 0;
  unsigned long simdikiZaman = millis();
  if (simdikiZaman - sonGuncelleme >= 10) {
    sirenFrekans += sirenYukseliyor ? 20 : -20;
    if (sirenFrekans >= 1000 || sirenFrekans <= 200) sirenYukseliyor = !sirenYukseliyor;
    ledcChangeFrequency(HOPARLOR_PIN, sirenFrekans, 8);
    ledcWrite(HOPARLOR_PIN, 128);
    sonGuncelleme = simdikiZaman;
  }
}

void sweepCal() {
  static int frekans = 100;
  static bool yukseliyor = true;
  static unsigned long sonGuncelleme = 0;
  unsigned long simdikiZaman = millis();
  if (simdikiZaman - sonGuncelleme >= 5) {
    frekans += yukseliyor ? 10 : -10;
    if (frekans >= 2000 || frekans <= 100) yukseliyor = !yukseliyor;
    ledcChangeFrequency(HOPARLOR_PIN, frekans, 8);
    ledcWrite(HOPARLOR_PIN, 64);
    sonGuncelleme = simdikiZaman;
  }
}

void istiklalMarsiCal() {
  static int index = 0;
  static unsigned long oncekiZaman = 0;
  static int notalar[] = {523, 698, 784, 830, 659, 784, 698, 698, 932, 1046, 1108, 880, 1046, 932, 1046, 932, 1046, 784, 784, 932, 831, 659, 698, 784, 830, 932, 1046, 1108, 1244, 1396, 1244, 622, 587, 622, 1046, 932, 831, 523, 494, 523, 784, 523, 1046, 932, 831, 784, 831, 698, 1396, 1244, 1108, 1046, 932, 831, 784, 698, 1046, 523, 698};
  static int sureler[] = {800, 800, 800, 800, 400, 200, 1600, 800, 800, 800, 800, 400, 200, 1600, 200, 200, 200, 400, 100, 400, 200, 400, 200, 400, 200, 400, 200, 400, 200, 400, 200, 200, 200, 200, 800, 800, 1600, 200, 200, 200, 800, 800, 800, 200, 200, 400, 200, 800, 800, 200, 400, 200, 400, 200, 400, 200, 400, 800, 1600};
  int uzunluk = sizeof(notalar) / sizeof(notalar[0]);

  unsigned long simdikiZaman = millis();
  if (index >= uzunluk) {
    ledcWrite(HOPARLOR_PIN, 0);
    sesCalıyor = false;
    index = 0;
    return;
  }

  if (simdikiZaman - oncekiZaman > sureler[index]) {
    int frekans = notalar[index];
    if (frekans > 0) {
      ledcChangeFrequency(HOPARLOR_PIN, frekans, 8);
      ledcWrite(HOPARLOR_PIN, 128);
    } else {
      ledcWrite(HOPARLOR_PIN, 0);
    }
    oncekiZaman = simdikiZaman;
    index++;
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  tft.init(240, 240, SPI_MODE2);
  tft.setRotation(3);
  yazLCD("WiFi Baslatiliyor");

  ledcAttach(HOPARLOR_PIN, 1000, 8);
  ledcWrite(HOPARLOR_PIN, 0);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Baglanti IP: ");
  Serial.println(IP);
  yazLCD("Baglanin: " + IP.toString());

  server.on("/", handleRoot);
  server.on("/melodi1", handleMelodi1);
  server.on("/melodi2", handleMelodi2);
  server.on("/siren", handleSiren);
  server.on("/sweep", handleSweep);
  server.on("/ac", handleAc);
  server.on("/kapat", handleKapat);
  server.on("/resetle", handleResetle);
  server.on("/istiklal", handleIstiklal);

  server.begin();
}

void loop() {
  server.handleClient();
  if (sesCalıyor) {
    switch (sesTuru) {
      case 1: melodiCal(melodi1, melodi1_sure, melodi1_uzunluk); break;
      case 2: melodiCal(melodi2, melodi2_sure, melodi2_uzunluk); break;
      case 3: sirenCal(); break;
      case 4: sweepCal(); break;
      case 99: istiklalMarsiCal(); break;
    }
  }
}