#include <LiquidCrystal.h>
#include <EEPROM.h>

#define YELLOWBTN 2
#define BLUEBTN 3
#define REDBTN 4
#define GREENBTN 5
#define YELLOWLED 6
#define BLUELED 7
#define REDLED 8
#define GREENLED 9

#define C 262
#define D 294
#define E 330
#define F 349

#define SWITCHPIN A3

#define MAXLEN 10
#define STARTDELAY 1000

unsigned long gameTime = 0;
unsigned int timeDelay = STARTDELAY;
int currentNum = 0;
int ledarr[MAXLEN];
int arrlen = 0;
bool btnPressed = false;
unsigned int score = 0;
unsigned int highscore = 0;

LiquidCrystal lcd(A5, A4, 11, 10, 12, 13);
bool inMenu = true;
bool printed = false;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);

  lcd.begin(16, 2);
  pinMode(YELLOWBTN, INPUT);  //YELLOW led btn INPUT
  pinMode(BLUEBTN, INPUT);    //BLUE led btn INPUT
  pinMode(REDBTN, INPUT);     //RED led btn INPUT
  pinMode(GREENBTN, INPUT);   //GREEN led btn INPUT
  pinMode(YELLOWLED, OUTPUT); //YELLOW LED OUTPUT
  pinMode(BLUELED, OUTPUT);   //BLUE LED OUTPUT
  pinMode(REDLED, OUTPUT);    //RED LED OUTPUT
  pinMode(GREENLED, OUTPUT);  //GREEN LED OUTPUT

  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  gameTime = millis();
  randomSeed(analogRead(0));
  int i = 0;
  while (i < MAXLEN)
  {
    ledarr[i] = -1;
    i++;
  }
  if (EEPROM.read(412) == 255)
    highscore = 0;
  else
    highscore = EEPROM.read(412);
}

//Each time new random led is choosed we add it to array
void addstep()
{
  int i = 0;
  if (arrlen < 10)
  {
    ledarr[arrlen] = currentNum;
  }
  else
  {
    lost();
    return;
  }
  arrlen++;
}

//Each time we press correct btn we delete that step from array
void deletestep()
{
  int i = 0;
  while (i < arrlen)
  {
    ledarr[i] = ledarr[i + 1];
    i++;
  }
  arrlen--;
}

//where the magic happens
void gameloop()
{
  //check if we need to choose new LED
  if (gameTime < millis())
  {
    //Let's decrease delay so it is harder
    timeDelay -= 2;
    // Make sure activated LED is not activated anymore
    digitalWrite(YELLOWLED, LOW);
    digitalWrite(BLUELED, LOW);
    digitalWrite(REDLED, LOW);
    digitalWrite(GREENLED, LOW);
    //delay(50);
    //Make sure we dont pick same LED which is already active
    int temp = currentNum;
    while (currentNum == temp)
      currentNum = random(4) + 6;
    // Activate chosen LED
    digitalWrite(currentNum, HIGH);
    // Update timer
    gameTime = millis() + timeDelay;
    // Add step to array
    addstep();
  }
}

void inputloop()
{

  //YELLOW pressed
  if (digitalRead(YELLOWBTN) == HIGH && !btnPressed)
  {
    // this makes sure we only execute this code once after pressing button
    btnPressed = true;
    //correct button pressed
    if (ledarr[0] == YELLOWLED)
    {
      deletestep();
      tone(A2, C, 100);
      digitalWrite(YELLOWLED, LOW);
      score++;
    }
    //wrong button pressed
    else
    {
      lost();
    }
  }

  //BLUE pressed
  else if (digitalRead(BLUEBTN) == HIGH && !btnPressed)
  {
    btnPressed = true;
    tone(A2, D, 100);
    if (ledarr[0] == BLUELED)
    {
      deletestep();
      digitalWrite(BLUELED, LOW);
      score++;
    }
    else
    {
      lost();
    }
  }

  //RED pressed
  else if (digitalRead(REDBTN) == HIGH && !btnPressed)
  {
    btnPressed = true;
    tone(A2, E, 100);
    if (ledarr[0] == REDLED)
    {
      deletestep();
      digitalWrite(REDLED, LOW);
      score++;
    }
    else
    {
      lost();
    }
  }

  //GREEN
  else if (digitalRead(GREENBTN) == HIGH && !btnPressed)
  {
    btnPressed = true;
    tone(A2, F, 100);
    if (ledarr[0] == GREENLED)
    {
      deletestep();
      digitalWrite(GREENLED, LOW);
      score++;
    }
    else
    {
      lost();
    }
  }

  else
  {
    btnPressed = false;
    int i = 2;
    while (i < 6)
    {
      if (digitalRead(i) == HIGH)
      {
        btnPressed = true;
      }
      i++;
    }
  }
  //Update lcd score
  clearLCD();
  lcd.print("Score: ");
  lcd.print(score);
  lcd.setCursor(0, 1);
  lcd.print("Delay: ");
  lcd.print(timeDelay);
  lcd.print("ms");
}

void clearLCD()
{
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 0);
}

void lost()
{
  highscore = max(score, highscore);
  EEPROM.write(412, highscore);
  clearLCD();
  lcd.print("You lost!");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  arrlen = 0;
  score = 0;
  int i = 0;
  // Little animation after losing
  while (i < 5)
  {
    digitalWrite(YELLOWLED, HIGH);
    digitalWrite(BLUELED, HIGH);
    digitalWrite(REDLED, HIGH);
    digitalWrite(GREENLED, HIGH);
    delay(500);
    digitalWrite(YELLOWLED, LOW);
    digitalWrite(BLUELED, LOW);
    digitalWrite(REDLED, LOW);
    digitalWrite(GREENLED, LOW);
    delay(500);
    i++;
  }
  i = 0;
  //reset array
  while (i < MAXLEN)
  {
    ledarr[i] = -1;
    i++;
  }
  timeDelay = STARTDELAY;
  highscore = max(score, highscore);
  inMenu = true;
  printed = false;
}

void menuloop()
{
  if (!printed)
  {
    clearLCD();
    digitalWrite(YELLOWLED, HIGH);
    lcd.print("Play: Yellow btn");
    lcd.setCursor(0, 1);
    lcd.print("Highscore: ");
    lcd.print(highscore);
    printed = true;
  }
  if (digitalRead(YELLOWBTN) == HIGH)
  {
    inMenu = false;
    digitalWrite(YELLOWLED, LOW);
  }
  delay(100);
}

// the loop function runs over and over again forever
void loop()
{
  if (inMenu)
  {
    menuloop();
  }
  else
  {
    gameloop();
    inputloop();
  }
  delay(50); // wait for a "second"
}