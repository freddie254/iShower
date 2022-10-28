#include <SD.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SPI.h>
//#include <SoftwareSerial.h>

File myFile;


LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows



char a;
char b;
char c;
char d;


///////////sys id
String sys_id = "E00000";




char ten_digit_array[11];
char twelve_digit_array[13];

byte customer_number = 0;

int amountx = 0;
//byte cost_of_water;
//byte cost_of_power;

int my_results = 1;//transaction fiinder

int my_water = 0; 

float High_peak,Low_peak;         
float Amps_Peak_Peak, Amps_RMS;
float wattage;
float power_used = 0.0;
float watthour = 0.0;
float my_money = 0.00;

float calibrationFactor = 4.5;
volatile byte pulseCount;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime = 0;
int start, finished, elapsed;

byte i = 0;
byte j = 0;
byte input_count = 0; 
int ij = 0;

byte sensorInterrupt = 0; //flow sensor 
byte sensorPin       = 2;//WAtER FLOW SENSOR


byte my_relay1 = A1;
byte my_relay3 = A3;

byte gatevalveopen = A0;//for the temporal switch 
byte sensorIn = A2; //current sensor      
byte buzzer_pin = A3;

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'}, 
  {'*','0','#'}
  };
    
 /* byte rowPins[ROWS] = {3,4,5,6};
  byte colPins[COLS] = {7,0,1};
*/
  byte rowPins[ROWS] = {0,7,6,4};
  byte colPins[COLS] = {3,1,5};
  
  
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS );//  ---


  //t.txt ----> to hold transactions done
  //w.txt ----> to hold water used
  //p.txt ----> to hold power used
  //wd.txt ---> to hold water cost
  //pd.txt ---> to hold power cost
void setup(){
 // Serial.begin(9600);

  //Serial.println("h");
 // buzzer();
  
  lcd.backlight();
  
  lcd.init();
  
    pinMode(buzzer_pin, OUTPUT);
    pinMode(gatevalveopen, INPUT_PULLUP);
    pinMode(my_relay3, OUTPUT);
    pinMode(my_relay1, OUTPUT);

    
    pinMode(sensorPin, INPUT);
    pinMode(sensorIn,INPUT);

    
    digitalWrite(sensorPin, HIGH); 
   // lcd.setCursor(0, 0);
  lcd.print(F ("Starting up "));
  lcd.setCursor(0, 1);
  lcd.print(F ("   iSHOWER254"));
  delay(1100);
  
  lcd.clear();
  
  if (!SD.begin(10)) {
    lcd.print(F ("error call for "));
    lcd.setCursor(0, 1);
    lcd.print(F ("help."));
    //Serial.println("failed!");
    while (1);
  }
  
  delay(500);
  lcd.clear();
  lcd.print( F("press any KEY to cont.."));
  lcd.setCursor(0, 1);
  lcd.print(F ("continue...."));
  delay(50);
    //Serial.println(F("in"));

    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
    my_jump:

    my_meter();
    if(my_water > 20){
      
     digitalWrite(my_relay1, LOW);
     digitalWrite(my_relay3, HIGH);

      delay(1000);

     digitalWrite(my_relay1, LOW);     
     digitalWrite(my_relay3, LOW);

    goto my_jump;
    }
}
void loop()
{
  main_menu();
  //buzzer();
}
void main_menu(){
  
   d = keypad.getKey();
   if(d != NO_KEY){
    
    lcd.clear();
    lcd.print(F("please wait "));
    lcd.setCursor(0, 1);
    lcd.print(F("  alittle"));
    
    delay(750);
    
    lcd.clear();
    lcd.print(F("push switch"));
    lcd.setCursor(0, 1);
    lcd.print(F("to continue"));
    
  jump:
  digitalWrite(A0, LOW);
  pinMode(A0, INPUT_PULLUP);
  ij = digitalRead(gatevalveopen);
  while(ij == HIGH){
    ij = digitalRead(gatevalveopen);
    delay(1000);
    my_meter();
    lcd.noBacklight();
    if(my_water > 20){
      
     digitalWrite(my_relay1, LOW);
     digitalWrite(my_relay3, HIGH);

      delay(1000);

     digitalWrite(my_relay1, LOW);     
     digitalWrite(my_relay3, LOW);

    }
  }
  
  lcd.clear();
 // lcd.print(ij);
  delay(50);
  if(ij == LOW){
    lcd.backlight();
    lcd.init();
    
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print( F("1) create acc") );
      lcd.setCursor(0, 1);
      lcd.print( F("2)LOGIN ") );
      lcd.print( F("3)GetPin") );

      buzzer();
      do{
        b = keypad.getKey();
        }while (b == NO_KEY);
             if(b == '1'){
              create_account();
              goto jump;
             }
             
             else if (b == '2'){
              login_account();
              goto jump;
              }
              
             else if (b == '3'){
               forgot_pin();
               goto jump;
              }
              
              else{
                buzzer();
                lcd.clear();
                lcd.print(F("press a nice key"));
                delay(1500);
                goto jump;
              }
    }
    goto jump;
   }  
  
}

void create_account(){
  
  char file_ending[5] = ".txt";
  char confirmed_password[5];
  
  jump:
  ij = digitalRead(gatevalveopen);
  while(ij == LOW){
    ij = digitalRead(gatevalveopen);
  lcd.clear();
  lcd.print(F("key your number"));

  delay(500);
  lcd.setCursor(0, 1);
  
  input_count = 0;
  j = 0; 
  
  ij = digitalRead(gatevalveopen);
  while(input_count < 10 && ij == LOW){
    
    ij = digitalRead(gatevalveopen);
       b = keypad.getKey();
       
       if(b){
        if(b == '*'){
          input_count--;
          
          lcd.clear();
          lcd.print(F("key your number"));
          lcd.setCursor(0, 1);
          
          for(int presses = 0; presses < input_count; presses++){
            lcd.print(ten_digit_array[presses]);
          }
         }
          else{
            ten_digit_array[input_count] = b;
            input_count++;
            lcd.print(b);
          }
       }
  }

    for(i = 2; i < 10; i++){
      twelve_digit_array[j] = ten_digit_array[i];//cut the first two digits
      j++;
    }

    j = 8;
    for(i=0; i<4; i++){
     twelve_digit_array[j] = file_ending[i];//add the .txt suffix
     j++;
      }
      
    memset(ten_digit_array, 0, sizeof(ten_digit_array));
    
    myFile = SD.open(twelve_digit_array);
    if (myFile){
        lcd.clear();
        lcd.print(F("user exists!"));
        delay(2000);
        memset(twelve_digit_array, 0, sizeof(twelve_digit_array));
        myFile.close();
        goto jump;
      }
    
    delay(1000); 

    lcd.clear();
    lcd.print(F("create a pin"));
    lcd.setCursor(0, 1);
    
//getting the pin
     input_count = 0;
     ij = digitalRead(gatevalveopen);
     while(input_count <4 && ij == LOW){
      ij = digitalRead(gatevalveopen);
       c = keypad.getKey();
       if(c){
          ten_digit_array[input_count] = c;
          input_count++;
          lcd.print(F("*"));
        }
    }
    int pin1 = atoi(ten_digit_array);
    
    //Serial.println(F("first pin"));
    
    delay(300);
    
    lcd.clear();
    lcd.print(F("confirm your pin"));
    
    myFile = SD.open(twelve_digit_array, FILE_WRITE);
    
    lcd.setCursor(0, 1);
     input_count = 0;
     ij = digitalRead(gatevalveopen);
     while(input_count <4 && ij == LOW){
      ij = digitalRead(gatevalveopen);
       c = keypad.getKey();
       if(c){
          confirmed_password[input_count] = c;
          while(confirmed_password[input_count] != ten_digit_array[input_count]){
            ////.println("pin do not match");
            lcd.clear();
            lcd.print(F("pin do not match"));

            buzzer();
            delay(300);
            memset(twelve_digit_array, 0, sizeof(twelve_digit_array));
            memset(ten_digit_array, 0, sizeof(ten_digit_array));
            memset(confirmed_password, 0, 5);
            
            delay(1500);
            
            myFile.close();
             
            goto jump;
          }
          myFile.print(confirmed_password[input_count]);
          input_count++;
          lcd.print(F("*"));
        }
    }

    myFile.println();
    myFile.println("0");      
      
    myFile.close();
   
    delay(300);
    
      lcd.clear();
      lcd.print(F("account created"));
      
      lcd.setCursor(0, 1);
      lcd.print(F("successfully"));
      
            
      memset(ten_digit_array, 0, sizeof(ten_digit_array));
      memset(confirmed_password, 0, 5);
      
      delay(2000);


      i = 1;
      ij = digitalRead(gatevalveopen);
      while(i == 1 && ij == LOW){
        ij = digitalRead(gatevalveopen);
        if(ij == LOW){
          i = 12;
          digitalWrite(gatevalveopen, LOW);
        }
        lcd.clear();
        
        lcd.print(F("send this sms "));///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        lcd.setCursor(0, 1);/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        lcd.print(F("make Acc "));//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        lcd.print(sys_id);
        delay(6000);

        lcd.clear();
        lcd.print(F("send the sms to "));
        lcd.setCursor(0, 1);
        lcd.print(F("   0729 984 638"));

        delay(6000);

        lcd.clear();
        lcd.print(F("then power off "));  
        delay(1000);
      }
  }
        
}
///////////////LOGIN
void login_account(){
  
  char file_ending[5] = ".txt";
  char password[5];
  char number_last[3];
  
  jump:
  
   lcd.clear();
   lcd.print(F("enter number"));
   
   delay(200);
   
   lcd.setCursor(0, 1);
   
   input_count = 0;
   j = 0;
   
   char pin[5];
   ij = digitalRead(gatevalveopen);
   while(input_count < 10 && ij == LOW){
    ij = digitalRead(gatevalveopen);
       c = keypad.getKey();
       
       if(c){
        if(c == '*'){
          input_count--;

          lcd.clear();
          lcd.print(F("enter number"));
          lcd.setCursor(0, 1);
  
          for(int presses = 0; presses < input_count; presses++){
            lcd.print(ten_digit_array[presses]);
          }
         }else{
            ten_digit_array[input_count] = c;
            input_count++;
            lcd.print(c);
            // Serial.println(ten_digit_array);
         }
            
       }
       
  }

    delay(300);
    
    for(i = 2; i < 10; i++){
      twelve_digit_array[j] = ten_digit_array[i];//cut the first two chars
      j++;
    }

    j = 8;
    
    for(i = 0; i < 4;i++){
      twelve_digit_array[j] = file_ending[i];//put.txt extension
      j++;
     }
   
   number_last[0] = ten_digit_array[8];
   number_last[1] = ten_digit_array[9];
     
   customer_number = atoi(number_last);
     
   myFile = SD.open(twelve_digit_array);
   
    byte count_it = 0;
   
    if(myFile){
      
        myFile.seek(0);
        
        while(count_it < 4){
          
          char character_read = myFile.read();
          pin[count_it] = character_read;
          count_it++;
          
        }
        
        myFile.close();
        
     }
          
    else {
      
      lcd.clear();
      lcd.print(F("no account found"));
      
      delay(1500);
      
      memset(ten_digit_array, 0, sizeof(ten_digit_array));
      memset(twelve_digit_array, 0, sizeof(twelve_digit_array));
      
      myFile.close();
           
      goto jump;
      
      }
        
  //Serial.println("enter pin");
  
  lcd.clear();
  lcd.print(F("enter your pin"));
  
  lcd.setCursor(0, 1);
  
//getting the pin

     input_count = 0;
     ij = digitalRead(gatevalveopen);
     while(input_count < 4 && ij == LOW){
      ij = digitalRead(gatevalveopen);
       c = keypad.getKey();
       
       if(c){
        
          password[input_count] = c;

          while(password[input_count] != pin[input_count]){
            
            lcd.clear();
            
            lcd.print(F("wrong pin!"));
            
            buzzer();
            
            memset(ten_digit_array, 0, sizeof(ten_digit_array));
             memset(twelve_digit_array, 0, sizeof(twelve_digit_array));
             memset(password, 0, sizeof(password));
             
             delay(500);
             
            goto jump;
            
          }
          
          input_count++;
          lcd.print(F("*"));
          
        }
        
    }
            
    delay(500);
    
    lcd.clear();
    lcd.print(F("logged in"));
    
    lcd.setCursor(0, 1);
    lcd.print(F("successfully"));

    crazy_jump:
    
    myFile = SD.open(twelve_digit_array, O_RDWR);
    myFile.seek(5);
    
    my_money = myFile.parseInt() / 100;
    myFile.close();
    
    delay(1000);             
    lcd.clear();
    lcd.print(F("1)Shower"));
    lcd.print(F(" 3)Next"));
    lcd.setCursor(0, 1);
    lcd.print(F("2)Load "));
    lcd.print(my_money);  //basic money   
    input_count = 0;   
    
    delay(100); 
    ij = digitalRead(gatevalveopen);
    input_count = 0;

    my_jump:
    while(input_count < 4 && ij == LOW){
      ij = digitalRead(gatevalveopen);
       c = keypad.getKey();
       
       if(c == '1'){

          //my_meter();
          start_shower();
          //shut off the lcd
          lcd.noBacklight();
          input_count = 10;
          goto my_jump;
          
       }
       
       else if(c == '2'){
             // veery ery very long func
             
          load_itime();
          
          goto crazy_jump;
          
        }
        
        else if (c == '3'){
          
          lcd.clear();
          lcd.print(F("1) Change pin"));
          
          lcd.setCursor(0, 1);
          lcd.print(F("2)Delete Account"));
          
          delay(100);
          
          byte my_counter = 0;
          ij = digitalRead(gatevalveopen);
            while(my_counter < 4 && ij == LOW){
              
              b = keypad.getKey();
                          
              if(b == '1'){
                
                 change_pin();
                 
                 goto crazy_jump; 
                 
              }
              else if(b == '2'){
                
                 lcd.clear();
                 lcd.print(F("1)Confirm delete"));
                 
                 lcd.setCursor(0, 1);
                 lcd.print(F("2) Quit!"));
                 
                 byte my_time = 0;
                 ij = digitalRead(gatevalveopen);
                 while(my_time < 4 && ij == LOW){
                  
                   c = keypad.getKey();
                   
                   if(c == '1'){
                    
                     SD.remove(twelve_digit_array);
                     
                     lcd.clear();
                     lcd.print(F("account deleted"));
                     
                     lcd.setCursor(0, 1);
                     lcd.print(F("    successfully"));
                     
                     delay(1700);
                     ij = digitalRead(gatevalveopen);
                   while(ij == LOW){
                    
                     lcd.clear();
                     lcd.print(F("send this sms "));
                     
                     lcd.setCursor(0, 1);
                     lcd.print(F("Delete my Acc"));
                     
                     delay(5000);
  
                     lcd.clear();
                     lcd.print(F("send the sms to "));
                     
                     lcd.setCursor(0, 1);
                     lcd.print(F("   0729 984 638"));
                     
                     delay(6000);
                     
                     lcd.clear();
                     
                     lcd.print(F("then power off "));
  
                     delay(2000);
                     
                   }
                   
                 }
                 
                 else if(c == '2'){
                  
                    goto crazy_jump;
                    
                 }
                 
               }
                                
          }
          
       }   
                        
  memset(ten_digit_array, 0, sizeof(ten_digit_array));
  memset(twelve_digit_array, 0, sizeof(twelve_digit_array));
  
}
 else{
                        
  }
   //input_count = 10;                   
 }
 
}
/////////FORGOT PIN
void forgot_pin(){
  
   char file_ending[5] = ".txt";
   
   char random_used[2];

   byte my_slave_number = 86;
  
   lcd.clear();
   lcd.print(F("enter number"));
   
   delay(100);
   
   lcd.setCursor(0, 1);
   
   input_count = 0;
   ij = digitalRead(gatevalveopen);
   while(input_count < 10 && ij == LOW){//get customer number
    ij = digitalRead(gatevalveopen);
       c = keypad.getKey();
       if(c){
        if(c == '*'){
          input_count--;

          lcd.clear();
          lcd.print(F("enter number"));
          lcd.setCursor(0, 1);
          for(int presses = 0; presses < input_count; presses++){
            lcd.print(ten_digit_array[presses]);
          }
         }
         else{
            ten_digit_array[input_count] = c;
            input_count++;
            lcd.print(c);
         }
       }
  }

    j = 0;
    for(i = 2; i < 10; i++){
      twelve_digit_array[j] = ten_digit_array[i];
      j++;
    }
  
    j = 8;
    for(i=0; i<4; i++){
     twelve_digit_array[j] = file_ending[i];
     j++;
      }
      
  myFile = SD.open(twelve_digit_array);

  memset(twelve_digit_array, 0, sizeof(twelve_digit_array));

  twelve_digit_array[0] = ten_digit_array[8];
  twelve_digit_array[1] = ten_digit_array[9];
 
  customer_number = atoi(twelve_digit_array); 

  //Serial.println(F("customer number"));
 // Serial.println(customer_number);

  memset(ten_digit_array, 0, sizeof(ten_digit_array));
  memset(twelve_digit_array, 0, sizeof(twelve_digit_array));
  
  if(myFile){
  for(i = 0; i < 3; i++){

  if(digitalRead(gatevalveopen) == HIGH){
    i = 4;
  }
    lcd.clear();
    lcd.print(F("send this sms"));
    
    lcd.setCursor(0, 1);
    lcd.print(F("I forgot my pin"));
    
    delay(4000);
  
    lcd.clear();
    lcd.print(F("send the sms to"));
    
    lcd.setCursor(0, 1);
    lcd.print(F("  0729 984 638"));
    
    delay(5000);
  }
  
  
    
    lcd.clear();
    lcd.print(F("enter reset key"));
    
    delay(100);
    
    lcd.setCursor(0, 1);

     input_count = 0;
     ij = digitalRead(gatevalveopen);
     while (input_count < 15 && ij == LOW){// get the reset key
        c = keypad.getKey();
        if(c){
          
          if(c == '#'){
            j = input_count;
            input_count = 15;
            
            }
          else if(c == '*'){
          input_count--;
          
          lcd.clear();
          lcd.print(F("enter reset key"));
          lcd.setCursor(0, 1);
          for(int presses = 0; presses < input_count; presses++){
            lcd.print(ten_digit_array[presses]);
          }
         }
         else{
          ten_digit_array[input_count] = c;
          lcd.print(c);
          input_count++;
         }
        
      }
      
    }

    delay(200);
    
    lcd.clear();
    lcd.print(F("  getting pin"));
    
    
    random_used[0] = ten_digit_array[0];
    
    byte my_random = atoi(random_used);
    

    input_count = 0;
    
    for( i = 1;i < j;i++){
      twelve_digit_array[input_count] = ten_digit_array[i];
      input_count++;
      }
      
    long real_key = atoi(twelve_digit_array);

    memset(ten_digit_array, 0, sizeof(ten_digit_array));
    // Serial.println(real_key);
    
    long arduino_key = my_random * customer_number * my_slave_number;
   // Serial.println(arduino_key);   

      
      myFile.seek(0);
      
      i = 0;
      ij = digitalRead(gatevalveopen);
      while(i < 4 && ij == LOW){
        ij = digitalRead(gatevalveopen);
          char character_read = myFile.read();//read pin from file
          ten_digit_array[i] = character_read;
          i++;
          
            if(character_read == 10 ){  //new line
             break;
            }
            
        }
      myFile.close();

       if(arduino_key == real_key  && ij == LOW){
        
        delay(3000);
        
        ////.println("your pin is");
        lcd.clear();
        lcd.print(F("your pin is")); 
        
        delay(500);
        
        lcd.setCursor(0, 1);
        
        for(i = 0;i < 4;i++){
          lcd.print(ten_digit_array[i]);
        }
        delay(2200);
        
        lcd.clear();
        lcd.print(F("press any key"));
        
        lcd.setCursor(0, 1);
        lcd.print(F("to continue.."));
        
        memset(ten_digit_array, 0, sizeof(ten_digit_array));
        memset(twelve_digit_array, 0, sizeof(twelve_digit_array));
        
        return;
        
      }
      else{
          //.println("reset key not correct");
          
          delay(4500);
          
            lcd.clear();
            lcd.print(F("key not correct"));
            
            delay(2000);

            lcd.clear();
            lcd.print(F("press any key"));
            
            lcd.setCursor(0, 1);
            lcd.print(F("to continue.."));
            
            memset(ten_digit_array, 0, sizeof(ten_digit_array));
            memset(twelve_digit_array, 0, sizeof(twelve_digit_array));
            memset(ten_digit_array, 0, sizeof(ten_digit_array));
           return;
          }
        
    }
    else{
      lcd.clear();
      lcd.print(F("no account found"));
      
      delay(1500);
      
      lcd.clear();
      lcd.print(F("press any key"));
      
      lcd.setCursor(0, 1);
      lcd.print(F("to continue.."));
      
      memset(twelve_digit_array, 0, sizeof(twelve_digit_array));
      memset(ten_digit_array, 0, sizeof(ten_digit_array));
      
      return ;
      
    }
      
      return;
  
  }
  void my_meter(){
    
   // while(1){
      if((millis() - oldTime) > 1000){
    totalMilliLitres = 0;
    detachInterrupt(sensorInterrupt);    
   flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
      
    oldTime = millis();
   
    flowMilliLitres = (flowRate / 60) * 1000 ;//two milliseconds to complete cycle
   
    totalMilliLitres += flowMilliLitres * 1.7668/2;
   
    pulseCount = 0;

    my_water = totalMilliLitres;
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
    
    //water += totalMilliLitres;
    
    pulseCount = 0;

    
    }
   // lcd.setCursor(0, 0);
    //lcd.print(my_water);
    //}
  }
////////////////////////////////////////////////////////////////////////////////////////////////////////////START SHOWER
void start_shower(){

  lcd.clear();
  lcd.print(F("iShower started"));
  delay(1500);
  lcd.clear();
  lcd.print(F("power off 2 exit"));
  lcd.setCursor(0, 1);
  lcd.print(F("iTime available"));
  int water = 0;
  int my_water = 0;

  //lcd.clear();
  buzzer();
  
  float water_money = 0.0;
  float power_money = 0.0;
  myFile.close();

  myFile = SD.open("p.txt", O_RDWR);//get power cost
  myFile.seek(0);
  i = myFile.parseInt();
  myFile.close();

  myFile = SD.open("w.txt", O_RDWR);//get water cost
  myFile.seek(0);
  j = myFile.parseInt();
  myFile.close();

  float myfactor = i / j;
  myFile = SD.open(twelve_digit_array, O_RDWR);
  myFile.seek(5);
  long money_available = myFile.parseInt();//accurate to the cent  
  //get the valve open
  if(money_available > 0){
     digitalWrite(my_relay1, HIGH);
     digitalWrite(my_relay3, HIGH);
     
     delay(2500);
     
     digitalWrite(my_relay1, LOW);
     digitalWrite(my_relay3, LOW);
  }
  
   jump:
   delay(50);
  myFile = SD.open(twelve_digit_array, O_RDWR);

  myFile.seek(5);
   money_available = myFile.parseInt();//accurate to the cent 
  ij = digitalRead(gatevalveopen); 
  if(ij == HIGH ){
    money_available = 0;
  //set money available to zero so as to log out
  }
 while(money_available > 0){ 

   if((millis() - oldTime) > 1000){
    totalMilliLitres = 0;
    detachInterrupt(sensorInterrupt);    
   flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
      
    oldTime = millis();
   
    flowMilliLitres = (flowRate / 60) * 1000 ;//two milliseconds to complete cycle
   
    totalMilliLitres += flowMilliLitres * 1.7668/2;
   
    pulseCount = 0;

    my_water += totalMilliLitres;
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
    
    //water += totalMilliLitres;
    
    pulseCount = 0;

    
    }
    
    
    get_current_peaks();//get the peak to peak value
    Amps_RMS = Amps_Peak_Peak * 0.3536 * 0.06;
    wattage =  Amps_RMS * 240 * 0.025 * 3;//calibration factor
    if(wattage > 5){
      power_used += wattage;
      power_used -= 5;
    }
    if (power_used > 5450){
     // buzzer();
      watthour = power_used * 0.0014;
      power_used = 0.0;
     }
    power_money = watthour * i / 10;
        

    watthour = 0;
  
   if(money_available < 500){//money in cents
          
           buzzer();
        } 
                
        if(my_water > 200){
             
             water_money =  (0.2 * j) / 10;
             my_water -= 200;
        }
            

          //byte used_money =  water_money;//+ power_money;             
          long new_money = 0.0;
          new_money = money_available - water_money - power_money;
          
          if(new_money < money_available){             
              myFile.seek(5); 
              myFile.println(new_money);
              myFile.close(); 
          } 
          myFile.close(); 
          new_money = 0.0;   
          water_money = 0.0; 
          power_money = 0.0;        
          goto jump;  
                        
     }
        
     digitalWrite(my_relay1, LOW); 
     digitalWrite(my_relay3, HIGH);
     
     delay(2500);

     digitalWrite(my_relay1, LOW); 
     digitalWrite(my_relay3, LOW);
     
    myFile.close();
    lcd.clear();
    lcd.setCursor(0, 1);
    if(money_available < 0){
      lcd.print(F("no iTime !"));
    }
    buzzer();
    delay(2000);
    memset(twelve_digit_array, 0, sizeof(twelve_digit_array));

  
}
void load_itime(){
  
  
  lcd.clear();
  lcd.print(F("enter token"));

  delay(200);

  lcd.setCursor(0, 1);
  //Serial.println("token");
  

  char recharge_key[40];//the recharge key
  char divisor[20];//after dividing the recharge key
  char digi_amount[2];//digits in amount
  char digi_trans[2]; // digits in trnsaction id
  char water_cost [3]; //cost of water
  char power_cost[3];//cost of power
  int my_random = 0;
  char my_rand[2];
  char the_divisor[3];//answer after dividing
  char to_divide[3];//to be diivided
  char my_remainder [2];//the remainder
  int to_div = 0;
  int my_divisor = 0;
  int my_modulo = 0;
  int answer_count = 0;
  int current_position = 0;
  
  input_count = 0;
  
  
  //char cust_end[3];
  byte to_end = 0;
  ij = digitalRead(gatevalveopen);
  while (input_count < 40 && ij == LOW){
      ij = digitalRead(gatevalveopen);

    c = keypad.getKey();
    if(c){
      if (c == '#'){
        to_end = input_count;
        input_count = 40;
      }
      else if(c == '*'){
          input_count--;
          
          lcd.clear();
          lcd.print(F("enter token"));
          lcd.setCursor(0, 1);
          
          for(int presses = 0; presses < input_count; presses++){
            lcd.print(recharge_key[presses]);
          }
         }
         else{
          recharge_key[input_count] = c;
          lcd.print(c);
          input_count++;
         }
    }
    
  }
  
  if(to_end > 9){
    
    my_rand[0]= recharge_key[0]; 
    my_random = atoi(my_rand);
    delay(100);
    //Serial.println(my_random);
    //Serial.println(my_random); 
    //Serial.println(i);
    //Serial.println(to_end);
    memset(to_divide, 0, sizeof(to_divide));
    i = 1;
    jump:
    ij = digitalRead(gatevalveopen);
    while(i < to_end && ij == LOW){
            ij = digitalRead(gatevalveopen);

      to_divide[current_position] = recharge_key[i];
      to_div = atoi(to_divide);
     // Serial.println(to_div);
      if(to_div == 0){
        
        my_modulo = to_div % my_random;
        
         if(my_modulo == 0){
          
            my_divisor = to_div / my_random;
            
            itoa(my_divisor, the_divisor, 10);
            
            divisor[answer_count] = the_divisor[0];
            
            memset(the_divisor, 0, sizeof(the_divisor));
            memset(to_divide, 0, sizeof(to_divide));
            
            answer_count++;
            current_position = 0;
          
          }
          
          i++;
          goto jump;
        }
      ij = digitalRead(gatevalveopen);
      while(to_div < my_random && ij == LOW){
                   ij = digitalRead(gatevalveopen);
        current_position++;
        if(i > 1){//put a zero incase we are not at the second char of number ro be divided//greater or equal to
           divisor[answer_count] = '0';
           answer_count++;
          }
          ++i;
        to_divide[current_position] = recharge_key[i];
        
        to_div = atoi(to_divide);
        
      }
      
    // Serial.print(to_div);
      //Serial.print(F(" current poss "));
        //Serial.println(i);
         i++;
        my_modulo = to_div % my_random;
         if(my_modulo == 0){
          
            my_divisor = to_div / my_random;
           // Serial.println(my_divisor);
            itoa(my_divisor, the_divisor, 10);
            
            divisor[answer_count] = the_divisor[0];
            
            memset(the_divisor, 0, sizeof(the_divisor));
            memset(to_divide, 0, sizeof(to_divide));
            
            answer_count++;
            current_position = 0;
            
          }
          else{
            
            my_divisor = to_div / my_random;
            //Serial.println(my_divisor);
            //divisor[answer_count] = the_divisor[0];
            
            itoa(my_divisor, the_divisor, 10);
            
            divisor[answer_count] = the_divisor[0];
            
            memset(the_divisor, 0, sizeof(the_divisor));
            memset(my_remainder, 0, sizeof(my_remainder));
            
            itoa(my_modulo, my_remainder, 10);
            
            memset(to_divide, 0, sizeof(to_divide));
            to_divide[0] = my_remainder[0];
            answer_count++;
            current_position = 1;
            
         }
                 
      }
  
     memset(recharge_key, 0, sizeof(recharge_key));
     memset(digi_amount, 0, sizeof(digi_amount));
     memset(digi_trans, 0, sizeof(digi_trans));
     
    
    digi_amount[0] = divisor[2];//digits in amount
    digi_trans[0] = divisor[3];//digits in transacted
    
    delay(1000);
    
    
    int amount = atoi(digi_amount);   


    int trans = atoi(digi_trans);
        

    water_cost[0] = divisor[4];
    water_cost[1] = divisor[5];
    int my_water_cost = atoi(water_cost);
    
    power_cost[0] = divisor[6];
    power_cost[1] = divisor[7];
    byte my_power_cost = atoi(power_cost);
    
    //Serial.println(amount);
  
    char trans_no[trans+1];
   
    

    memset(trans_no, 0, sizeof(trans_no));
    byte u = 0;
    if(amount == 1){//get the amount paid and transaction number used
      char digits_amount [2];
      digits_amount[0] = divisor[8];
      amountx = atoi(digits_amount);

     
      u = 9;
       memset(digits_amount, 0, sizeof(digits_amount));
      
      for(i = 0; i < trans; i++){
        trans_no[i] = divisor[u];
        u++;
        }
      
      }
    if(amount == 2){
      char digits_amount [3];

      digits_amount[0] = divisor[8];
      digits_amount[1] = divisor[9];
      u = 10;
      amountx = atoi(digits_amount);
       memset(digits_amount, 0, sizeof(digits_amount));

      for(i = 0; i < trans; i++){
        trans_no[i] = divisor[u];
        u++;
        //Serial.println(trans_no[i]);
      }    
    }
        
    if(amount == 3){
      char digits_amount [4];
      digits_amount[0] = divisor[8];
      digits_amount[1] = divisor[9];
      digits_amount[2] = divisor[10];
      amountx = atoi(digits_amount);
       memset(digits_amount, 0, sizeof(digits_amount));
          
      u = 11;
      for(i = 0; i < trans; i++){
        trans_no[i] = divisor[u];
        u++;
        }
      }
    if(amount == 4){
      char digits_amount [5];
      digits_amount[0] = divisor[8];
      digits_amount[1] = divisor[9];
      digits_amount[2] = divisor[10];
      digits_amount[3] = divisor[11];
      amountx = atoi(digits_amount);
      memset(digits_amount, 0, sizeof(digits_amount));
      u = 12;
      
      for(i = 0; i < trans; i++){
        trans_no[i] = divisor[u];
        u++;
        }
      }
      
    

       int tota_trans = atoi(trans_no);//transaction number used
       
       //Serial.println(amountx);
       //Serial.println(tota_trans);
  
       byte signature_size = 4 + 4 + amount + trans;//digits in amount paid and digits in transaction id
       
      // Serial.println(signature_size);
      
       byte real_signature_size = (answer_count - signature_size)+1;
       
       char my_sign [real_signature_size];
       
       //Serial.println(trans);
     //  Serial.println(real_signature_size);
       //Serial.println(real_signature_size);
       memset(my_sign, 0, sizeof(my_sign));
       for(i = 0;i < real_signature_size-1;i++){//get the signature
        my_sign[i] = divisor[signature_size];
        signature_size++;
       }
     
      // Serial.println(i);
      
       int input_sign = atoi(my_sign);//signature inputed to int
              
       long sign_ure = customer_number * tota_trans ;
       
      /* Serial.println(customer_number);
       Serial.println(tota_trans);
       Serial.println(input_sign);
       Serial.println(sign_ure);*/
       delay(200);
    
       if(input_sign == sign_ure){//recharge key correct
         
         check_transaction_number_used(tota_trans);
         if(my_results == 1){ //the transaction number has not yet been used
           delay(500);
           lcd.clear();
           lcd.print(F("calculating"));
           
           delay(1500);
           
           lcd.clear();
           lcd.print(F("acc recharged"));
           
           delay(1500);

           update_water(my_water_cost);

           update_power(my_power_cost);

           add_transaction(tota_trans);

           update_amount_balance(amountx);

           lcd.setCursor(0, 1);
           lcd.print(F("   successfully"));
           lcd.clear();
           
           
           
           
           
           delay(2000);
           memset(trans_no, 0, sizeof(trans_no));
           memset(divisor, 0, sizeof(divisor));
         }else{
              //recharge token used
              lcd.clear();
              lcd.print(F("used token"));
              
              delay(1500);
              
              lcd.clear();
              
              memset(trans_no, 0, sizeof(trans_no));
              memset(divisor, 0, sizeof(divisor));
              //Serial.println("used");
            }
            
        }
        else{
          //incorrect recharge key
          lcd.clear();
          lcd.print(F("incorrect token"));
          
          delay(1500);
          
          lcd.clear();
          
          memset(divisor, 0, sizeof(divisor));
          //Serial.println("incorrect");
          }
  }
  else{
    lcd.clear();
    lcd.print(F("enter all digits"));
    
    lcd.setCursor(0, 1);
    lcd.print(F("  in the token"));
    delay(2000);
    
  }

}

void change_pin(){

 ////.println("enter new pin");
  lcd.clear();
  lcd.print(F("enter new pin"));
  
  delay(200);
  
  lcd.setCursor(0, 1);
  
  char password[5];
  char confirmed_password[5];
  
  input_count = 0;
  ij = digitalRead(gatevalveopen);
  while (input_count < 4 && ij == LOW){
  c = keypad.getKey();
  if(c){
      password[input_count] = c;
      lcd.print(F("*"));
        input_count++;

    }
    
  }
  
  int pin1 = atoi(password);
  
  lcd.clear();
  lcd.print(F("confirm pin"));
  
  delay(200);
  
  lcd.setCursor(0, 1);
  
  input_count = 0;
  ij = digitalRead(gatevalveopen);
  while (input_count < 4 && ij == LOW){
  c = keypad.getKey();
  if(c){
      confirmed_password[input_count] = c;
      lcd.print(F("*"));
        input_count++;

    }
  }

  int pin2 = atoi(confirmed_password);
  
     //.println(confirmed_password);
       if(pin1 == pin2){
        
        lcd.clear();
        lcd.print(F("pin changed"));
        
        delay(500);
        
        lcd.setCursor(0, 1);
        lcd.print(F("  successfully"));
        
        delay(1500);
        
         myFile = SD.open(twelve_digit_array, O_RDWR);
         myFile.seek(0);
         
         myFile.println(password);
         
         myFile.close();
         
         memset(ten_digit_array, 0, sizeof(ten_digit_array));
         memset(twelve_digit_array, 0, sizeof(twelve_digit_array));
 
        }
        else{
          ////.println("pin dont match");
          lcd.clear();
          lcd.print(F("pin do not match!"));
          
          delay(1500);
          
          memset(ten_digit_array, 0, sizeof(ten_digit_array));
          memset(twelve_digit_array, 0, sizeof(twelve_digit_array));

          }

  }
void pulseCounter(){
  // Increment the pulse counter
  pulseCount++;
}
void get_current_peaks(){
  int count;            
  High_peak = 0;      
  Low_peak = 1024;
  
      for(count = 0 ; count < 300 ; count++)          
      {
        float ACS_Value = analogRead(sensorIn); 

        
        if(ACS_Value > High_peak)                
            {
              High_peak = ACS_Value;            
            }
        
        if(ACS_Value < Low_peak)                
            {
              Low_peak = ACS_Value;             
            }
      }                                        
      
  Amps_Peak_Peak = High_peak - Low_peak;      
}

void update_water(byte my_water_cost){
  
  myFile = SD.open("W.txt", O_RDWR);
  
  myFile.seek(0);
  
  myFile.println(my_water_cost);
  
  myFile.close();
  
}
void update_power(byte my_power_cost){
  
  myFile = SD.open("P.txt", O_RDWR);
  
  myFile.seek(0);
  
  myFile.println(my_power_cost);
  
  myFile.close();
  
}
void update_amount_balance(int my_amount){

  myFile = SD.open(twelve_digit_array, O_RDWR);//get customers balance
  
  myFile.seek(5);
  
  long old_money = myFile.parseInt();
  
  long total_money = my_amount * 100 + old_money;
  
  myFile.seek(5);
  
  myFile.println(total_money);
  
  myFile.close();
  
}
void check_transaction_number_used(int transaction_in_lab){
  
  String Buffer_Read_Line = "";
  byte LastPosition = 0;
  
  myFile = SD.open("T.txt");
  
  int totalBytes = myFile.size();

  while (myFile.available()){
    for(LastPosition = 0; LastPosition <= totalBytes; LastPosition++){
      char character = myFile.read();
      
      Buffer_Read_Line = Buffer_Read_Line + character;
      if(character == 10 ){//10 ==  new line
        
        int transaction_index = Buffer_Read_Line.toInt();

        if(transaction_index == transaction_in_lab){
         /* Serial.println(transaction_index);
          Serial.println(transaction_in_lab);*/
          //Serial.println(transaction_index);
          my_results = 2;
          
        }
        
        Buffer_Read_Line="";      
    }
    
   }
   
 }
 
 myFile.close();

}
void add_transaction(int k){
  myFile = SD.open("T.txt",FILE_WRITE);
  
  myFile.println(k);
  
  myFile.close();
  
}
void buzzer(){
  
    tone(buzzer_pin, 440, 50);
    delay(500);
  
    return;
  } 
