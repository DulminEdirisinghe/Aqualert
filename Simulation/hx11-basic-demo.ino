/*
  Source: https://www.electroschematics.com/pressure-sensor-guide/
*/
#include "HX711.h"
#include <EEPROM.h>
#define HIGH 0x1
#define LOW 0x0

unsigned long  time; 
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;

float totalIntake = 0; // total water
float tolerance = 10; //value when nothing on scale

float peakOld;
float peakNow;
float Current_weight;

//LED
const int LED = 7;


//Light Switch
const int LightPin =8;
boolean Light=digitalRead(LightPin);
  
// Front panel
const int blinkOffPin = 4;
const int avgUpPin = 5;
const int avgDownPin = 6;


boolean blinkOff=digitalRead(blinkOffPin);
boolean avgUp=digitalRead(avgUpPin);
boolean avgDown=digitalRead(avgDownPin);

float maxW =3000;
float minW =10; // water level limits
float defaultW =700;


HX711 scale;

unsigned long minutes() {
  time=(millis()) / 60000UL ; // avg for 30mins
  //Serial.println(millis());  
  //Serial.println(time); 
  if (millis()==0){
    return     1;  }
  else{
    return time;   
  }
}


float avg ; //  average water

float average(float totalIntake){
  Serial.println("users Avg");
  Serial.println(totalIntake);
  Serial.println("checking Avg");
  Serial.println( avg);
    return(totalIntake/(minutes()+1));}

void blink(float Current_weight){
  
  while (Current_weight-10 < scale.get_units() and blinkOff == HIGH){ 
  Serial.print("U need water");
  //Screen("YOU");
  //Screen("NEED");     
  //Screen("WATER");

//led warning
  digitalWrite(LED,0); // prevent overlap with light on button
  delay(1000);
  
  digitalWrite(LED,1);
  delay(1000);
  digitalWrite(LED,0);
  delay(1000);                    //led warning off with light off
  }
  if (Light = HIGH){             //turn on the light only if lamp button on
   digitalWrite(LED,1);
   }
  delay(1000*2);  
}

void setup() {
  Serial.begin(9600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.println("Setup started");
  pinMode(LED,OUTPUT);
  pinMode(LightPin,INPUT);
  pinMode(blinkOffPin, INPUT);
  pinMode(avgUpPin, INPUT);
  pinMode(avgDownPin, INPUT);

 if(avg>minW and avg<maxW){
  avg=EEPROM.read(0); //  get set lvl or default lvl from memory##########################
  Serial.print("eeprom avg");
  Serial.println(avg);
}
else{
  avg =defaultW;
  EEPROM.write(0,defaultW);
  Serial.println("default avg");
}


  if (Light =HIGH){           //Turn on the LED at beginning depend on users preference
    digitalWrite(LED,1);
  }
  else{
    digitalWrite(LED,0);
  }
Current_weight = scale.get_units(10);
peakOld =Current_weight;
peakNow =Current_weight;

}

void loop() {
  
  if(avgUp = LOW){
      if (avg < maxW){
        avg+=10; // increase avg water by 10;
        Serial.println("New Avg" + String(avg));
        //Screen("New Avg" + String(avg));
        EEPROM.update(0, avg); // update avg in memory -- may cause eeprom degrade   maybe send the code to setup 
        delay(1000);
        }
      else{
        //Screen("Max");
        delay(1000);
        }
      return;
    }
    if(avgDown == HIGH){
      if (avg > minW){
        avg-=10; // increase avg water by 10;
        EEPROM.update(0, avg); // update avg in memory -- may cause eeprom degrade
        Serial.println("New Avg" + String(avg));
        //Screen("New Avg" + String(avg));
        delay(1000);
        }
      else{
        Serial.println("Min");
        //Screen("Min");
        delay(1000);
        }
      return; // go to beginning void loop
    }

    Current_weight = scale.get_units(10);
    Serial.println("currenr weight");
    Serial.println(Current_weight);
    
    // checking every  between 29and 31 mins
   //Serial.println(minutes()%1);
   if (minutes()%1 ==0 and average(totalIntake)<=avg){    
       blink(Current_weight);
       }
   if (Current_weight > tolerance and Current_weight < peakNow){
    peakOld=peakNow;
    peakNow=Current_weight;
    totalIntake += (peakOld - peakNow);
    Serial.println("current user intake");
    Serial.println(totalIntake);
    }
   if (Current_weight > tolerance and Current_weight > peakNow){
      totalIntake = avg*minutes();
      Serial.println("current user intake default");
      Serial.println(totalIntake);
    }
    
   if (average(totalIntake)<=avg)
   {
       Serial.println();
       Serial.println("in screen");
       Serial.println("U need Water");
       //Screen("YOU");
       //Screen("NEED");
       //Screen("WATER");
    }
   else{
      Serial.println();
       Serial.println("in screen");
      Serial.println("U Hydrated");
      Serial.println(average(totalIntake));
      //Screen("YOU");
      //Screen("ARE");
      //Screen("HYDRATED");
      }
    
   Serial.println("checking condition usr avg> avg");
  Serial.println(average(totalIntake)> avg);
  scale.power_down();             // put the ADC in sleep mode
  delay(3000);
  scale.power_up();
  Serial.println();
       Serial.println("###########################"); 

  delay(5000);//debug delay     

}