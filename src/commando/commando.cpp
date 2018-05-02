#include <Arduino.h>
#include <hashmap.h>
#include <Wire.h>
#include <EEPROM.h>

any_key_t** cmd;
any_value_t* cmdcalls;
size_t cmds_len, i;
hashmap_ptr Commands;

void addCommands(){

}

void setup(){
  Serial.begin(9600);

  addCommands();
  delay(500);
  Serial.print(F(":) Serial ON"));
  Serial.println();
}

void loop(){
  if(Serial.available() == 0){ return; }

  Serial.print(F(":] "));
  String cmd = Serial.readStringUntil(13);
  Serial.print(cmd);
  Serial.println();

  // TODO: Custom Commands (via structs and function pointers)


}
