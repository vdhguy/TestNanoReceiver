#include <Arduino.h>
#include <SoftwareSerial.h>

#define LED_PIN    5
#define HC12_RX    2   // Arduino RX <- HC-12 TX
#define HC12_TX    3   // Arduino TX -> HC-12 RX
#define HC12_SET   4   // HC-12 SET pin (LOW = AT command mode)

SoftwareSerial hc12(HC12_RX, HC12_TX);

bool reading = true;
bool inMenu = false;

void printMenu() {
  Serial.println();
  Serial.println("=== HC-12 Menu ===");
  Serial.println("1) Read HC-12 and output to Serial USB");
  Serial.println("2) Stop reading");
  Serial.println("3) Configure HC-12");
  Serial.println("==================");
  Serial.print("Choice: ");
}

// Read a line from Serial with blocking wait (timeout in ms)
String readSerialLine(unsigned long timeout = 10000) {
  String input = "";
  unsigned long start = millis();
  while (millis() - start < timeout) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') {
        if (input.length() > 0) break;
      } else {
        Serial.print(c); // echo character
        input += c;
      }
    }
  }
  Serial.println();
  return input;
}

// Enter AT mode, send a command, collect response, exit AT mode
String sendATCommand(const char* cmd) {
  digitalWrite(HC12_SET, LOW);
  delay(40);

  hc12.println(cmd);

  delay(100);
  String response = "";
  unsigned long t = millis();
  while (millis() - t < 500) {
    if (hc12.available()) {
      response += (char)hc12.read();
    }
  }

  digitalWrite(HC12_SET, HIGH);
  delay(80);

  return response;
}

void hc12SetChannel() {
  Serial.print("Enter channel (1-127): ");
  String input = readSerialLine();
  int channel = input.toInt();

  if (channel < 1 || channel > 127) {
    Serial.println("Invalid channel. Aborting.");
    return;
  }

  char cmd[12];
  snprintf(cmd, sizeof(cmd), "AT+C%03d", channel);
  Serial.print("Sending: ");
  Serial.println(cmd);

  String response = sendATCommand(cmd);

  if (response.length() > 0) {
    Serial.print("HC-12 response: ");
    Serial.println(response);
  } else {
    Serial.println("No response from HC-12 (check SET pin wiring).");
  }
}

void hc12ReadChannel() {
  Serial.println("Reading channel from HC-12...");

  String response = sendATCommand("AT+RC");

  if (response.length() > 0) {
    Serial.print("HC-12 response: ");
    Serial.println(response);
  } else {
    Serial.println("No response from HC-12 (check SET pin wiring).");
  }
}

void configureHC12() {
  Serial.println("--- HC-12 Configuration ---");
  Serial.println("1) Set channel");
  Serial.println("2) Read current channel");
  Serial.print("Choice: ");

  String input = readSerialLine();

  if (input == "1") {
    hc12SetChannel();
  } else if (input == "2") {
    hc12ReadChannel();
  } else {
    Serial.println("Invalid choice.");
  }

  printMenu();
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(HC12_SET, OUTPUT);
  digitalWrite(HC12_SET, HIGH); // HIGH = normal transmission mode

  Serial.begin(9600);
  hc12.begin(9600);

  Serial.println("HC-12 active. Press SPACE for menu.");
}

void loop() {
  // Handle Serial input
  if (Serial.available()) {
    char c = Serial.read();
    while (Serial.available()) Serial.read(); // flush

    if (!inMenu) {
      if (c == ' ') {
        reading = false;
        inMenu = true;
        printMenu();
      }
      // ignore any other key while reading
    } else {
      if (c == '1') {
        Serial.println("1");
        reading = true;
        inMenu = false;
        Serial.println("Reading HC-12... (press SPACE for menu)");
      } else if (c == '2') {
        Serial.println("2");
        reading = false;
        inMenu = false;
        Serial.println("Stopped. Press SPACE for menu.");
      } else if (c == '3') {
        Serial.println("3");
        configureHC12(); // shows menu again at end, stay in menu
        inMenu = true;
      } else {
        printMenu();
      }
    }
  }

  // Read from HC-12 when active
  if (reading && hc12.available()) {
    String message = "";
    while (hc12.available()) {
      message += (char)hc12.read();
      delay(2);
    }
    Serial.print("Received: ");
    Serial.println(message);

    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
  }
}
