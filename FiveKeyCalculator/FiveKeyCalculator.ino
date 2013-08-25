#include <LiquidCrystal.h>

// Initialize the liquid crystal display
LiquidCrystal lcd(8,9,10,11,12,13);

// Number of buttons
const int BUTTON_COUNT = 8;

// To clear the line
const char* SPACES_15 = "               ";

// Button pins
byte buttons[BUTTON_COUNT] = {0, 1, 2, 3, 4, 5, 6, 7};

// Last button that was pressed
byte button_pressed = -1;

// Store operands
long operand_1 = 0;
long operand_2 = 0;
long last_result = 0;

// How many digits are already store in active operand
byte digit_count = 0;

// 1 = add, 2 = subtract, 3 = multiply, 4 = divide
byte calc_operator = 0; 

/* 
 * If shift is off, user accesses numbers from 0-4.
 * If shift is on, so that the user will access number 5-9.
 */
boolean shift_on = false;

// Special characters that we'll use
byte UP_ARROW [8] = {
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};

void setup() {
  initButtons();
  initDisplay();
  showWelcome();
  showNumbers();
  calc_operator = 0;
}

void loop() {
  checkClicked();
}

void buttonClicked(int index) {
  // Process button clicked event
  switch(index) {
    // Zero button is the equal button
    case 0:
      equalPressed();
      break;
      
    // Operator button
    case 1:
      operatorPressed();
      break;
      
    // Shift button
    case 2:
      shiftPressed();
      break;
      
    // Numbers
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
      numberPressed(index - 3);
      break;
     
    // Should not happen
    default:
      // Do nothing
      break;
  }
}

void checkClicked() {
  // Store if any button was pressed
  boolean button_on = false;
  
  // Go through all buttons
  for (byte i = 0; i < BUTTON_COUNT; i++) {
    // If button was pressed
    if (digitalRead(buttons[i]) == LOW) {
      // Store button index
      button_pressed = i;
      button_on = true; // button was pressed
      break; // only consider the first button pressed
    }
  }
  
  // If no button was pressed and a button was pressed before
  if (button_on == false && button_pressed != -1) {
    // Generate the "event"
    buttonClicked(button_pressed);
    // Reset button index, event was handled
    button_pressed = -1;
  }
}

void equalPressed() {
  // Only compute if an operator was selected
  if (calc_operator != 0) {
    long result = 0;
    switch(calc_operator) {
      case 1: // add
        result = operand_1 + operand_2;
        break;
      case 2: // subtract
        result = operand_1 - operand_2;
        break;
      case 3: // multiply
        result = operand_1 * operand_2;
        break;
      case 4: // divide
        result = operand_1 / operand_2;
        break;
      }
    reset();
    last_result = result;
    showResult();
  }
}

void initButtons() {
  // Initialize the buttons as pull up
  for (byte i = 0; i < BUTTON_COUNT; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
  }
}

void initDisplay() {
  // Create special characters
  lcd.createChar(0, UP_ARROW);
  
  // Initialize it
  lcd.begin(16,2);  
}

void numberPressed (int number) {
  long* operand;
  
  // no operator selected
  if (calc_operator == 0) {
    // Select operand 1 to add numbers
    operand = &operand_1;
  } else {
    // Select operand 2 to add numbers
    operand = &operand_2;
  }
  
  // Add numbers
  *operand = *operand * 10 + number + (shift_on ? 5 : 0);
  
  // Show all operands
  showNumbers();
}

void operatorPressed() {
  // If pressing operator after finished first operation
  if (last_result != 0) {
    // Store last result as first operator
    operand_1 = last_result;
  }
  
  // Add 1 to operator
  calc_operator++;
  
  // If passed the max operator
  if (calc_operator >= 5) {
    // Go back to first one
    calc_operator = 1;
  }
  
  // Show operator
  showOperator();
}

void reset() {
  // Reset all variables
  operand_1 = 0;
  operand_2 = 0;
  calc_operator = 0;
}

void shiftPressed() {
  // Toggle shift
  shift_on = !shift_on;
  showShift();
}

void showNumbers() {
  // clear first line
  lcd.setCursor(1, 0);
  lcd.print(SPACES_15);
  
  // Transform to string
  String s_operand_1 = String(operand_1);
  // Position cursor so that number fits correctly
  lcd.setCursor(16 - s_operand_1.length(), 0);
  // Print number
  lcd.print(s_operand_1);
  
  // If we have an operator selected, also prints second operand
  if (calc_operator > 0) {
    // clear second line
    lcd.setCursor(1, 1);
    lcd.print(SPACES_15);
    
    String s_operand_2 = String(operand_2);
    lcd.setCursor(16 - s_operand_2.length(), 1);
    lcd.print(s_operand_2);
  }
  
  showOperator();
  showShift();
}

void showOperator() {
  lcd.setCursor(0, 0);
  switch(calc_operator) {
    case 1:
      lcd.print("+");
      break;
    case 2:
      lcd.print("-");
      break;
    case 3:
      lcd.print("x");
      break;
    case 4:
      lcd.write(byte(253));
      break;
    default:
      lcd.print(" ");
      break;
  }
}

void showResult() {
  lcd.clear();
  
  lcd.setCursor(0,0);
  lcd.print("=");
  
  String s_result = String(last_result);
  lcd.setCursor(16 - s_result.length(), 0);
  lcd.print(s_result);
  
  // We have to keep the shift on screen
  showShift();
}

void showShift() {
  lcd.setCursor(0, 1);
  if (shift_on) {
    lcd.write(byte(0));
  } else {
    lcd.write(" ");
  }
}

void showWelcome() {
  lcd.clear();
  lcd.print("-=> Welcome <=-");
  for (byte i = 0; i < 16; i++) {
    delay(100);
    lcd.setCursor(i, 1);
    lcd.write(byte(255));
  }
  delay(500);
  lcd.clear();
}
