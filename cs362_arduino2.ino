/*
 * CS 362 Musical Whack-A-Mole
 * Angela Timochina 
 * Zaynab Jarad
 * 
 * Description:
 * 
 * References:
 * http://www.arduino.cc/en/Tutorial/LiquidCrystalSetCursor
 */
// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// these constants won't change.  But you can change the size of
// your LCD using them:
const int numRows = 2;
const int numCols = 16;

//Buttons pins
const int leftButton = 10;
const int selectButton = 9;
const int rightButton = 8;

//Button State for selection
int rightButtonState = 0;

//Menu Options
String menu[] = {
  "Play Game", //0
  "Difficulty", //1
};

//Menu Pointer
int menuSelection = -1;

String difficulty[] = {
  "Easy", //0
  "Intermediate", //1
  "Hard", //2
};

int selectButtonState = 0;

//Difficulty Pointer
int difficultySelection = 0;

void setup() {
  //Setting pin mode
  pinMode(leftButton, INPUT);
  pinMode(selectButton, INPUT);
  pinMode(rightButton, INPUT);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(numCols, numRows);

  //set up Serial
  Serial.begin(9600);
}

void loop() {
 lcd.setCursor(0,0);
 lcd.clear(); 


 //MENU - Will send string to other arduino based on selection
 menuSelect();
 
 if(menuSelection == -1 && selectButtonState == 0) { //Display Welcome message
   lcd.print("Musical Whack-a-Mole");
 }
 else if(menuSelection == 0 && selectButtonState == 0) { //show play game option
   lcd.print(menu[menuSelection]);  
   if(digitalRead(selectButton) == HIGH) {
    //send string of difficulty
    //if(difficulty selection == 0 ...
    if(difficultySelection == 0) {
      //Send "D:0"
      Serial.write("D:0",3);
      //delay(10);
    }
    else if(difficultySelection == 1) {
      //Send "D:1"
      Serial.write("D:1",3);
      //delay(10);
    }
    else if(difficultySelection == 2) {
      //Send "D:2"
      Serial.write("D:2",3);
      //delay(10);
    }
    //send string to start
      Serial.write("S:1");
    //clear screen to game play

      gamePlay();
   }
 }
 else if(menuSelection == 1 && selectButtonState == 0) { //show difficulty option
    lcd.print(menu[menuSelection]);

    //Only enter difficulty menu if selected
    if(digitalRead(selectButton) == LOW) {
      difficultySelect();
    }  
    menuSelect();
 }
 selectButtonState = 0;
 delay(50);
}

void menuSelect() {
  
  //boundaries of menu selection is -1 to 1
  if(menuSelection < -1) {
    menuSelection = -1;
  }
  else if(menuSelection > 1) {
    menuSelection = 1;
  }

  //Get button state
  if(digitalRead(leftButton) == LOW) {
    menuSelection--;
  }
  if(digitalRead(rightButton) == LOW) {
    menuSelection++;
  }
}

void difficultySelect() {

  while(selectButtonState == 0) { //if havent selected difficulty then cannot exit section
    lcd.clear();
    lcd.print(difficulty[difficultySelection]);
    
    //Difficulty message boundaries is 0 to 2
    if(difficultySelection < 0) {
      difficultySelection = 0;
    }
    else if(difficultySelection > 2) {
      difficultySelection = 2;
    }

    //Check for button presses
    if(digitalRead(leftButton) == LOW) {
      difficultySelection--;
    }
    
    lcd.clear();
    lcd.print(difficulty[difficultySelection]);
    
    if(digitalRead(rightButton) == LOW) {
      difficultySelection++;
    }
    
    lcd.clear();
    lcd.print(difficulty[difficultySelection]);
    
    if(digitalRead(selectButton) == LOW) {
      selectButtonState = 1; //Exits while loop
    }
  } //end of while loop  
} //end of difficultySelection function

void gamePlay() {
  int score = 0;
  int misses = 0;
  String str[3];
  int i = 0;
  char results;
  //10 misses is losing the game
  while( misses < 10) {
    if(Serial.available()) {
      delay(100); //that way all serial sent together is received
      while(Serial.available() && i < 3) {
        str[i++] = Serial.read(); //read in string
      }
      bool result = false;
      String finalResult = "";
      int finalIntResult = -1;
      //parse string
      for(int j = 0; j < 3; j++) {
        
        // Go through string and find symbol B and then : 
        // and then store result of 0 or 1
        if(str[j] == 'B'){
          result = true;
        }
        if( result == true && str[j] == ':'){
          finalResult = str[j+1];
        }
    
      }//End of for(int j = 0... loop

      if(result == true) {
        //convert char to int
        finalIntResult = finalResult.toInt();
      }
      //based on string perform score calculation
      //hit
      if(finalIntResult == 1){
        score++;
        lcd.clear();
        lcd.print("Score: ");
        lcd.print(score);
        lcd.print(" Misses: ");
        lcd.print(misses);
      }
      else if(finalIntResult == 0) {
        misses++;
        if(misses == 5) {
          lcd.clear();
          lcd.print("Halfway to Losing, Careful!");
          delay(1000);
        }
        lcd.clear();
        lcd.print("Score: ");
        lcd.print(score);
        lcd.print(" Misses: ");
        lcd.print(misses);
      }
    }//End of if(Serial.available()...
  }//End of while(misses < 10)

  // At the end of the game therefore 
  // arduino sends messagse to stop
  Serial.write("S:0");

  //clear lcd screen, and display score
  lcd.print("Too Many Loses!");
  delay(1000);
  lcd.clear(); 
  lcd.print("Score: " + score);
  delay(1000);
  
  menuSelection = -1; //reset menu to welcome message
  selectButtonState = 0; //reset selectButtonState to be in state of menu 0
}//End of Game Play
