#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // �ޥ�I2C�ǦC��ܾ����{���w
 
#define KEY_ROWS 4  // �������䪺�C��
#define KEY_COLS 4  // �������䪺���
#define LCD_ROWS 2  // LCD��ܾ����C��
#define LCD_COLS 16 // LCD��ܾ������
 
// �]�m����Ҳ�
char keymap[KEY_ROWS][KEY_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
 
byte rowPins[KEY_ROWS] = {13, 12, 11, 10};
byte colPins[KEY_COLS] = {9, 8, 7, 6};
 
Keypad keypad = Keypad(makeKeymap(keymap), rowPins, colPins, KEY_ROWS, KEY_COLS);
 
String passcode = "4321";   // �w�]�K�X
String inputCode = "";      // �Ȧs�Τ᪺����r��
bool acceptKey = true;      // �N��O�_�����Τ�����J���ܼơA�w�]���u�����v
byte pressC = 0;
// LCD��ܾ�
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);  // �]�w�Ҳզ�}0x27�A�H��16��, 2�C����ܧΦ�
 
void clearRow(byte n) {
  byte last = LCD_COLS - n;
  lcd.setCursor(n, 1); // ���ʨ��2��A"PIN:"����
 
  for (byte i = 0; i < last; i++) {
    lcd.print(" ");
  }
  lcd.setCursor(n, 1);
}
 
// ��ܡu�󴫱K�X�v��A���]LCD��ܤ�r�M��J���A�C
void resetLocker() {
  lcd.clear();
  lcd.print("Knock, knock...");
  lcd.setCursor(0, 1);  // �������2��
  lcd.print("PIN:");
  lcd.cursor();
 
  acceptKey = true;
  inputCode = "";
}

// ��ܡu�w����{�v��A���]LCD��ܤ�r�M��J���A�C
/*
void resetLocker_yn() {
  lcd.clear();
  lcd.print("Change PIN ?");
  lcd.setCursor(0, 1);  // �������2��
  lcd.print("Yes->1/NO->2:");
  lcd.cursor();
 
  acceptKey = true;
  //inputCode = "";
  
      char key = keypad.getKey();
      if (acceptKey && key != NO_KEY) {
        if (key == '1') {
          lcd.print('1');
          delay(1000);
          changePinCode();
        }
        else if(key == '2'){
          lcd.print('2');
          delay(1000);
          resetLocker();
        }
      }
  
  
}*/

// ���Τ��J���K�X
void checkPinCode() {
  acceptKey = false;  // �Ȯɤ������Τ�����J
  clearRow(0);        // �q��0�Ӧr���}�l�M��LCD�e��
  lcd.noCursor();
  lcd.setCursor(0, 1);  // �������2��
  // ���K�X
  if (inputCode == passcode) {
    lcd.print("Welcome home!");

    //�߰ݨϥΪ̬O�_��K�X
    //resetLocker_yn();
    
    
  } else {
    lcd.print("***WRONG!!***");
  }
  delay(3000);
  resetLocker();     // ���]LCD��ܤ�r�M��J���A
}

/*****************************************************/

void changePinCode() {
  /*
  lcd.clear();
  lcd.noCursor();
  lcd.print("New PIN:"); // �п�J�s�K�X
  lcd.cursor();
  acceptKey = true;
  inputCode = "";
  */
  acceptKey = false;  // �Ȯɤ������Τ�����J
  // ����±K�X
  if (inputCode == passcode) {
    lcd.clear();
    lcd.noCursor();
    lcd.print("New PIN:");  // �п�J�s�K�X
    
    acceptKey = true;
    inputCode = "";
  } else {
    resetLocker();     // ���]LCD��ܤ�r�M��J���A    
  }
}
/*********************************************/
void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
//  lcd.init();       // ��l��lcd����
  lcd.backlight();  // �}�ҭI��
 
  resetLocker();
}
 
void loop() {
  char key = keypad.getKey();
   
  // �Y�ثe�����Τ��J�A�ӥB���s���r����J�K
  if (acceptKey && key != NO_KEY) {
      if (key == '*') {   // �M���e��
          clearRow(4);  // �q��4�Ӧr���}�l�M��
          inputCode = "";
      } else if (key == '#') {  // ����J�K�X
          checkPinCode();
      } else if(key == 'C') {
          pressC++;
          if(pressC == 1)
            changePinCode();
          if(pressC == 2){
            passcode = inputCode;
            pressC = 0;
            resetLocker();
          }
      } else {
          inputCode += key;  // �x�s�Τ᪺����r��
          lcd.print('*');
      }
  }  
}
