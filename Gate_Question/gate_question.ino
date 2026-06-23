// Define Input Pins (Switches)
const int pinA = 2;
const int pinB = 3;
const int pinCin = 4;

// Define Output Pins (LEDs)
const int pinSum = 8;
const int pinCout = 9;

void setup() {
  // Configure input pins with internal pull-up resistors
  // This means the pin is HIGH by default, and goes LOW when connected to GND
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  pinMode(pinCin, INPUT_PULLUP);

  // Configure output pins for the LEDs
  pinMode(pinSum, OUTPUT);
  pinMode(pinCout, OUTPUT);
}

void loop() {
  // Read inputs and invert them (!) because INPUT_PULLUP makes active-LOW switches.
  // If the switch is closed (connected to GND), it reads LOW, so !LOW becomes HIGH (1).
  int A = !digitalRead(pinA);
  int B = !digitalRead(pinB);
  int Cin =!digitalRead(pinCin);

  // Full Adder Logic
  // XOR operator in C++ is '^'
  int sum = A ^ B ^ Cin; 
  
  // AND is '&&', OR is '||'
  int cout = (A && B) || (B && Cin) || (A && Cin);

  // Output the results to the LEDs
  digitalWrite(pinSum, sum);
  digitalWrite(pinCout, cout);

  delay(50); // Small delay for stability
}

