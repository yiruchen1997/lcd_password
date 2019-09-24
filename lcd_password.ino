#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // 引用I2C序列顯示器的程式庫
 
#define KEY_ROWS 4  // 薄膜按鍵的列數
#define KEY_COLS 4  // 薄膜按鍵的行數
#define LCD_ROWS 2  // LCD顯示器的列數
#define LCD_COLS 16 // LCD顯示器的行數
 
// 設置按鍵模組
char keymap[KEY_ROWS][KEY_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
 
byte rowPins[KEY_ROWS] = {13, 12, 11, 10};
byte colPins[KEY_COLS] = {9, 8, 7, 6};
 
Keypad keypad = Keypad(makeKeymap(keymap), rowPins, colPins, KEY_ROWS, KEY_COLS);
 
String passcode = "4321";   // 預設密碼
String inputCode = "";      // 暫存用戶的按鍵字串
bool acceptKey = true;      // 代表是否接受用戶按鍵輸入的變數，預設為「接受」
byte pressC = 0;
// LCD顯示器
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);  // 設定模組位址0x27，以及16行, 2列的顯示形式
 
void clearRow(byte n) {
  byte last = LCD_COLS - n;
  lcd.setCursor(n, 1); // 移動到第2行，"PIN:"之後
 
  for (byte i = 0; i < last; i++) {
    lcd.print(" ");
  }
  lcd.setCursor(n, 1);
}
 
// 顯示「更換密碼」後，重設LCD顯示文字和輸入狀態。
void resetLocker() {
  lcd.clear();
  lcd.print("Knock, knock...");
  lcd.setCursor(0, 1);  // 切換到第2行
  lcd.print("PIN:");
  lcd.cursor();
 
  acceptKey = true;
  inputCode = "";
}

// 顯示「歡迎光臨」後，重設LCD顯示文字和輸入狀態。
/*
void resetLocker_yn() {
  lcd.clear();
  lcd.print("Change PIN ?");
  lcd.setCursor(0, 1);  // 切換到第2行
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

// 比對用戶輸入的密碼
void checkPinCode() {
  acceptKey = false;  // 暫時不接受用戶按鍵輸入
  clearRow(0);        // 從第0個字元開始清除LCD畫面
  lcd.noCursor();
  lcd.setCursor(0, 1);  // 切換到第2行
  // 比對密碼
  if (inputCode == passcode) {
    lcd.print("Welcome home!");

    //詢問使用者是否改密碼
    //resetLocker_yn();
    
    
  } else {
    lcd.print("***WRONG!!***");
  }
  delay(3000);
  resetLocker();     // 重設LCD顯示文字和輸入狀態
}

/*****************************************************/

void changePinCode() {
  /*
  lcd.clear();
  lcd.noCursor();
  lcd.print("New PIN:"); // 請輸入新密碼
  lcd.cursor();
  acceptKey = true;
  inputCode = "";
  */
  acceptKey = false;  // 暫時不接受用戶按鍵輸入
  // 比對舊密碼
  if (inputCode == passcode) {
    lcd.clear();
    lcd.noCursor();
    lcd.print("New PIN:");  // 請輸入新密碼
    
    acceptKey = true;
    inputCode = "";
  } else {
    resetLocker();     // 重設LCD顯示文字和輸入狀態    
  }
}
/*********************************************/
void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
//  lcd.init();       // 初始化lcd物件
  lcd.backlight();  // 開啟背光
 
  resetLocker();
}
 
void loop() {
  char key = keypad.getKey();
   
  // 若目前接受用戶輸入，而且有新的字元輸入…
  if (acceptKey && key != NO_KEY) {
      if (key == '*') {   // 清除畫面
          clearRow(4);  // 從第4個字元開始清除
          inputCode = "";
      } else if (key == '#') {  // 比對輸入密碼
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
          inputCode += key;  // 儲存用戶的按鍵字元
          lcd.print('*');
      }
  }  
}
