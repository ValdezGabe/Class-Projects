/*
 * Gabe Valdez
 * Final Assignment
 * Tracks PIR Sensor: Prints out Max, Min, Average every 10 mins and prints out Overall Average since start of program.
 * 5/30/23
 */

unsigned long shortTime;
unsigned long longTime;
int inputPin = 33;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status
int movement = 0;
int totalMovement = 0;
int maxi = 0;
int mini = 1;
float shortAve = 0;
float longAve = 0;
int myMaxs[144];
int myMins[144];
float myAves[144];
int j = 0;

void setup() {
  pinMode(inputPin, INPUT);     // declare sensor as input
 
  Serial.begin(115200);
  Serial.println("Starting Program");
}

void loop() {
  
  val = digitalRead(inputPin);  // read input value
  
  if(millis() > shortTime + 1000){            // One Second Interval(1000ms)
      if (val == HIGH){            
            movement++;
            totalMovement++;
            
      }
      Serial.print("Detected Movement(s): ");
      Serial.println(totalMovement);
      
      longAve = totalMovement;     
      shortAve = movement; 
                  
      shortTime = millis();         
  }
  if(millis() > longTime + 10000){           //Ten Minute Interval(600000ms)
       if(movement > maxi) {
              maxi=movement;
          }
       if(movement < mini){
              mini= movement;
          }
       shortAve = shortAve / 1000;          // used to be  divided by 600


       myMaxs[j] = maxi;               //need to make array to store values
       myMins[j] = mini;
       myAves[j] = shortAve;
      j++;
      movement= 0; //resets all values back to 0 to restart for next 10 mins
      maxi= 0;
      mini= 1;
      shortAve = 0;

       longTime= millis();
  }
  if(Serial.available() > 0){
        char message = Serial.read();
        longAve = (longAve * 1000) / millis();            // convert to seconds
        
        
        if(message == 'r'){
            for ( int i = 0; i < 144; ++i ){ // 
                  Serial.print("Maximum for interval ");
                  Serial.print(i);
                  Serial.print(": ");
                  Serial.print(myMaxs[i]);
                  Serial.println(" movement(s)");
                  
                  Serial.print("Minimum for interval ");
                  Serial.print(i);
                  Serial.print(": ");
                  Serial.print(myMins[i]);
                  Serial.println(" movement(s)");
                  
                  Serial.print("Average movement for interval ");
                  Serial.print(i);
                  Serial.print(": ");
                  Serial.print(myAves[i]);
                  Serial.println(" movement(s) per second");
            }
            Serial.print("Overall average movement: ");
            Serial.print(longAve);
            Serial.println(" movement(s) per second");
         }
  }
    
}
