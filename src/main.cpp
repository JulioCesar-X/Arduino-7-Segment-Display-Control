#include <Arduino.h>


// Maximum digits the display can handle
#define MAX_DIGITS 10

// Segment pins for the 7-segment display
const int segmentPins[] = {2, 3, 4, 5, 6, 7, 8};
const int segmentCount = sizeof(segmentPins) / sizeof(segmentPins[0]); // Auto-calculated segment count

// Button pins for user interaction
const int buttonPins[] = {9, 10, 11, 12};
enum { BUTTON_INCREMENT, BUTTON_DECREMENT, BUTTON_EVEN, BUTTON_ODD };
const int buttonCount = sizeof(buttonPins) / sizeof(buttonPins[0]); // Auto-calculated button count

// Binary patterns for displaying numbers 0-9 on the 7-segment display
const byte digitPatterns[MAX_DIGITS] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};

int currentValue = 0;     // Stores the current value displayed
bool isCyclingEven = false; // Indicates if cycling through even numbers
bool isCyclingOdd = false;  // Indicates if cycling through odd numbers
int evenIndex = 0;         // Tracks the current position in the even cycle
int oddIndex = 0;          // Tracks the current position in the odd cycle

// Configure the specified pins for input or output
void configurePins(const int* pins, int count, int mode) {
    for (int i = 0; i < count; i++) {
        pinMode(pins[i], mode);
    }
}

// Display a number on the 7-segment display
void displayNumber(int number) {
    if (number < 0 || number >= MAX_DIGITS) return; // Ensure number is within bounds
    byte pattern = digitPatterns[number];
    for (int i = 0; i < segmentCount; i++) {
        digitalWrite(segmentPins[i], pattern & (1 << i)); // Set each segment based on the pattern
    }
}

// Check which button is pressed; return the button index or -1 if none
int readButtonPressed() {
    for (int i = 0; i < buttonCount; i++) {
        if (digitalRead(buttonPins[i]) == LOW) return i; // Button pressed
    }
    return -1; // No button pressed
}

// Increment the current value and update the display
void incrementValue() {
    currentValue = (currentValue + 1) % MAX_DIGITS; // Circular increment
    displayNumber(currentValue);
}

// Decrement the current value and update the display
void decrementValue() {
    currentValue = (currentValue - 1 + MAX_DIGITS) % MAX_DIGITS; // Circular decrement
    displayNumber(currentValue);
}

// Cycle through all even numbers (0, 2, 4, 6, 8) in order
void cycleEvenNumbers() {
    currentValue = (evenIndex * 2) % MAX_DIGITS; // Map index to even number
    evenIndex = (evenIndex + 1) % 5;            // Move to the next even number
    displayNumber(currentValue);
}

// Cycle through all odd numbers (1, 3, 5, 7, 9) in order
void cycleOddNumbers() {
    currentValue = (oddIndex * 2 + 1) % MAX_DIGITS; // Map index to odd number
    oddIndex = (oddIndex + 1) % 5;                 // Move to the next odd number
    displayNumber(currentValue);
}

void setup() {
    // Configure segment and button pins
    configurePins(segmentPins, segmentCount, OUTPUT);
    configurePins(buttonPins, buttonCount, INPUT_PULLUP);
    
    // Display the initial value
    displayNumber(currentValue);
}

void loop() {
    int button = readButtonPressed();
    if (button != -1) { // If a button is pressed
        switch (button) {
            case BUTTON_INCREMENT:
                incrementValue();
                isCyclingEven = false; // Stop cycling when incrementing normally
                isCyclingOdd = false;
                break;
            case BUTTON_DECREMENT:
                decrementValue();
                isCyclingEven = false; // Stop cycling when decrementing normally
                isCyclingOdd = false;
                break;
            case BUTTON_EVEN:
                isCyclingEven = true; // Start cycling through even numbers
                isCyclingOdd = false;
                cycleEvenNumbers();
                break;
            case BUTTON_ODD:
                isCyclingOdd = true; // Start cycling through odd numbers
                isCyclingEven = false;
                cycleOddNumbers();
                break;
        }
        delay(200); // Debounce delay
    }

    // Continue cycling if active
    if (isCyclingEven) {
        delay(1000); // 1-second delay for cycling
        cycleEvenNumbers();
    } else if (isCyclingOdd) {
        delay(1000); // 1-second delay for cycling
        cycleOddNumbers();
    }
}