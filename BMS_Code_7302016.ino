// #include <OneWire.h>
// #include <DallasTemperature.h>
  #include <SPI.h>
#include <SD.h>


int tPIN = 7;
// Data wire is plugged into pin 2 on the Arduino
 #define ONE_WIRE_BUS tPIN
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
// OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
// DallasTemperature sensors(&oneWire);

//=--------------------------------------------------
boolean logg_values = true;
//=-----------------------------------------------------


//Variables
//Analogue input pins
    int CurrentPin = A5;
    int VoltagePin = A2;
    //int TempPin = A12;
   File dataFile ;
//Define the number of samples to taken and averaged.  The higher the number,the more the readings will be smoothed, but the slower the output will respond to the input.  Using a constant rather than a normal variable lets use this value to determine the size of the readings array.*/
    const int numReadings = 9;

//Basic variable 
    int index = 0;
    boolean CRANKING; //True or false
    boolean tempRequest = true;
    
//Current processing Variable 
    //Average current proccesing variables
    float rawCurrent_readings[numReadings];      // array readings from the analog input
    float average_current_value = 0;
    float currentValue = 0;
    float currentTotal = 0;
    int current_index = 0;
    
//Current processing Variable 
    //Average voltage proccesing variables
    float rawVoltage_readings[numReadings];      // array readings from the analog input
    float average_voltage_value = 0;
    float voltageValue = 0;
    float voltageTotal = 0;
    float open_circuitVoltage = 0;
    int voltage_index = 0;
    int set = 0;
    //VDR variables
    float R1 = 99000.0;   //voltage divider resistor 1
    float R2 = 9820.0;     //voltage divider resistor 2
    
//Temprature processing Variable 
    float tempatureReadings = 0;
    float LM35Readings = 0;
//     File dataFile = SD.open("datalog.csv", FILE_WRITE);
// Reggresion algorithim variables
    int N=0;
     float C = 0;
     float V = 0;
     float sumC = 0;
     float sumV = 0;
     float sumC2 = 0;
     float sumV2 = 0;
     float sumCV = 0;

//Health statues variables and constants
     const float vLossThsh = 2;
     const float vLossFrsh = 1;
     float voltageLoss = 0;
     float sohIndex = 0;
     float soh = 0;
     float soc = 0;
     String sohStat;
     
//Timing variable
float StartTime = 0, CurrentTime= 0, ElapsedTime = 0;
float time;
int intTime;
int loopy = 0;
int crank_N = 0;
//const int chipSelect = 10;

void setup(){
   // start serial port
      Serial.begin(9600);
  // Start up the library
//      sensors.begin();
      
   
       if (startSDCard() == true) {
//       File dataFile = SD.open("datalog.csv", FILE_WRITE);
      Serial.println(F("-------------------------------------------------------"));
     Serial.println();   
     Serial.println(F("----SD.CARD IS READY!!---"));
      
     Serial.println();
     Serial.println(F("---------------------------------------------------------"));
   }
//    File dataFile = SD.open("datalog.csv", FILE_WRITE);

     
    // initialize all the readings to 0:  
    for (int thisReading = 0; thisReading < numReadings; thisReading++){
        rawCurrent_readings[thisReading] = 0; 
        rawVoltage_readings[thisReading] = 0;
    }
    //first request for temprature
                  // request for temprature from the brobe
//                    sensors.requestTemperatures(); // Send the command to get temperatures
//                    tempatureReadings = sensors.getTempCByIndex(0);
                     Serial.println(F("----Temprature Reading taken!!---"));
     Serial.println();
     Serial.println(F("---------------------------------------------------------"));
}
  
void loop() {
  //**************************************************************************

  
 //*****************************************************************************
    
        // Read cuurent value:
            //Processing Average of a certain number(numReadings) of of samples.
            
              currentTotal = currentTotal - rawCurrent_readings[current_index]; //subtract the last reading:  
          
                // Read raw data from CurrentPin and save in array called rawCurrent_Readings[].   
              rawCurrent_readings[current_index] = analogRead(CurrentPin); //Raw data reading from the current sensor
             
                       
                // Convert Raw data to exact current Reading from the current sensor
              rawCurrent_readings[current_index] = (rawCurrent_readings[current_index]-510)*5/1024/0.0209 ;//Data processing:510-raw data from analogRead when the input is 0; 5-5v; the first 0.04-0.04V/A(sensitivity); the second 0.04-offset val;
              //rawCurrent_readings[current_index] = (((rawCurrent_readings[current_index] / 1023.0) * 5000 - 2500) / 40)+0.08;
              
                 // Add up the current readings
              currentTotal = currentTotal  + rawCurrent_readings[current_index]; 
              
                  //Increament the variable index (used to change addresses in array after every loop)    
              current_index = current_index + 1; 
              
//****************************************************************************

    // Read Voltage value:
            //Processing Average of a certain number(numReadings) of of samples.
            
              voltageTotal = voltageTotal - rawVoltage_readings[voltage_index]; //subtract the last reading:  
          
                // Read raw data from voltagePin and save in array called rawVoltage_readings[].   
              rawVoltage_readings[voltage_index] = analogRead(VoltagePin); //Raw data reading from the current sensor
             
                 // Convert Raw data to exact voltage Reading from the voltage divider consisting of R1 and R2
              rawVoltage_readings[voltage_index] = ((rawVoltage_readings[voltage_index] * 5.015) / 1024.0)/ (R2/(R1+R2));
              
                 // Add up the current readings
              voltageTotal = voltageTotal  + rawVoltage_readings[voltage_index]; 
              
                  //Increament the variable index (used to change addresses in array after every loop)    
              voltage_index = voltage_index + 1; 
              //current_index = current_index + 1; 
//-------------------------------------
   
//-------------------------------------
       //Passing the number of indexes to vatiable index
       index = voltage_index ; //Or current_index = index;
         
       // if we get to the end of the array
        if (index >= numReadings){ 
                voltage_index = 0; 
                current_index = 0;  
                
                  
              //Calculating the average values:    
                voltageValue = voltageTotal/numReadings;   
                currentValue = currentTotal/numReadings; 
             
             
            
              //Calibration
                voltageValue = voltageValue + 0.91;
                
                currentValue = currentValue - 0.0;
                index = 0;
          
        //Setting up the open circuit voltage to be maximum voltage Value   
         if(voltageValue > open_circuitVoltage){
               open_circuitVoltage = voltageValue;
         }
        }
        //===================================
        
         time = micros();
         (time = time/1000000);
         
//----------------------------------------
        
        /*LM35 Temperature sensor
          LM35Readings = analogRead(TempPin);
          tempatureReadings = LM35Readings * 0.48828125;
        */
//----------------------------------------
                 
                 
                    //   ---------//Serial monitor print-------------
//                         Serial.print(F("  Current: "));
//                         Serial.print(currentValue);
//                                                  
//                         //Serial.print("   Voltage: "));
//                         Serial.print(F(", Voltage: ")); 
//                         Serial.print(voltageValue);
//                           Serial.print(F(",  Temp: "));
//                         Serial.print (tempatureReadings);
//                          Serial.print(F(",  Time: "));                    
//                         Serial.print (time);
//                            Serial.print(F("   :"));
//                         //Serial.print (tempatureReadings);
//                                             
//                      
                     // --------------------------------------------**/
//*************************************************************************************************
    
    //Crankinng status
        if(currentValue > 8.2){
              CRANKING = true;
              if (loopy == 0){StartTime = time;
              
              loopy = 1;
            }
          }else{
              CRANKING = false;
              loopy = 0;
             
         }
                     // ---------//Serial monitor print-------------
                         // Serial.print("   Status: ");
                       //  Serial.print(" ,"); 
//                     Serial.println(CRANKING? "Cranking":"Stand By");
                     // --------------------------------------------
  
//*********************************************************************************************

       //Regression Algorithim for determining Vo
           //During Cranking Event:
            if (CRANKING == true) {
                //Placing current and voltage values to variable x and y
                    C = currentValue;
                    V = voltageValue;
                //Getting the sum, sum squred and sum of xy tha will be used in the regression formulae
                    sumC += C;
                    sumV += V;
                    sumC2 += (C * C);
                    sumV2 += (V * V);
                    sumCV += (C * V);
                 //Count number of values taken and check whether cranking event hapenned
                    N = N + 1;
//                     File dataFile = SD.open("datalog.csv", FILE_WRITE);
                   
             if (set == 0){dataFile = SD.open("datalog.csv", FILE_WRITE);set=1; }  
               if (dataFile) {
                if (logg_values==true){
                                dataFile.print(currentValue);
                                dataFile.print(",");
                               dataFile.print(voltageValue);
                                dataFile.print(",");
                                dataFile.println(time);}
//                                dataFile.close();
//                  Serial.println(F(" FINISHED LOGGING CURRENT/VOLTAGE/TIME!!"));
//                  Serial.println(F("-------------------------------------------"));
//                  
                 
                }  
                // if the file isn't open, pop up an error:
                else {
                  Serial.println(F("error opening datalog.csv"));
                  Serial.println(F("-------------------------------------------"));
                  
                }
            }
            
            //After Cranking event:
               if( CRANKING == false && N > 30 ) {
                 
                 //Time elapsed
                  CurrentTime = time;
                  ElapsedTime = CurrentTime - StartTime;
                
                 
                  double Cbar = sumC / N;
                  double Vbar = sumV / N;
                  double Cbar2 = Cbar * Cbar;
                  double Vbar2 = Vbar * Vbar;
                  double CbarVbar = Cbar * Vbar;
                           
                 
                  double CC = 0.0, VV = 0.0, CV = 0.0;
                    CC = sumC2 - (2 * sumC * Cbar) + (Cbar2 * N);
                    VV = sumV2 - (2 * sumV * Vbar) + (Vbar2 * N);
                    CV = sumCV - (sumV * Cbar) - (sumV * Cbar) + (CbarVbar * N);
                 
                  double gradient, y_intercept;
                 
                  gradient  = CV / CC;
                  y_intercept = Vbar - gradient * Cbar;
             
               
             //Calculatin Voltage loss 
             voltageLoss = open_circuitVoltage - y_intercept;
             
             //Calculating State of health (SOH)
             sohIndex = ((voltageLoss-vLossThsh)/(vLossFrsh-vLossThsh));
                if(sohIndex>1){
                   sohIndex = 1;
                }else if(sohIndex < 0){
                   sohIndex = 0;
                }
                soh = sohIndex * 100;
               if (soh >= 80){sohStat = "Good";}else if(soh <= 75){sohStat = "Replace Battery";}else{sohStat = "Fair";}
               
               
                //request for temprature
                  // request for temprature from the brobe
//                    sensors.requestTemperatures(); // Send the command to get temperatures
//                    tempatureReadings = sensors.getTempCByIndex(0);
                   crank_N++;
              soc = OCVArry(tempatureReadings, open_circuitVoltage);
//             
                    sumC = 0;
                    sumV = 0;
                    sumC2 =0;
                    sumV2 =0;
                    sumCV =0;
//*****************************************************************************

                      //---------//Serial monitor print-------------
                                       Serial.println(F("----------------------------------------------------"));
                              Serial.print(F("y = "));
                              Serial.print(gradient ,4);
                              Serial.print(F("x "));
                              Serial.print(F("+ "));
                              Serial.println(y_intercept);
                              
                              Serial.print(F("Total Values = "));
                              Serial.println(N);
                              Serial.print(F("OCV: "));
                              Serial.print(open_circuitVoltage);
                              Serial.println(F(" Volts"));
                             
                              Serial.print(F("Voltage loss: "));
                              Serial.print(voltageLoss);
                              Serial.println(F(" Volts"));
                              
                              
                              Serial.print(F("SOC: "));
                              Serial.print(soc);
                              Serial.println(" %"); 
                               
                              Serial.print(F("SOH: "));
                              Serial.print(soh);
                              Serial.print(F(" %  ---> Health: "));
                              Serial.println(sohStat);               
                              Serial.println(F("----------------------------------------------------"));
                              Serial.print(F("Battery Temperature: "));
                              Serial.print(tempatureReadings);
                              Serial.println(F(" Degrees Celsius"));
                              Serial.print(F("Cranking Duration: "));
                              Serial.print(ElapsedTime);
                              Serial.println(F(" Seconds"));
//                               Serial.println(F("----------------------------------------------------"));
//                 
//                                            Serial.print(now.year(), DEC);
//                            Serial.print('/');
//                            Serial.print(now.month(), DEC);
//                            Serial.print('/');
//                            Serial.print(now.day(), DEC);
                           Serial.println(F("----------------------------------------------------"));
                 
                     // --------------------------------------------
//                        File dataFile = SD.open("datalog.csv", FILE_WRITE);
//      if (set == 1){dataFile = SD.open("datalog.csv", FILE_WRITE);set=0; }
                     if (dataFile) {
                       //dataFile.println();
                       dataFile.print("--");
                                dataFile.print(",");
                               dataFile.print("--");
                                dataFile.print(",");
                                dataFile.print("->>");
                                dataFile.print(",");
                       dataFile.print(crank_N);
                                dataFile.print(",");
                               dataFile.print(open_circuitVoltage);
                                dataFile.print(",");
                                dataFile.print(voltageLoss);
                                dataFile.print(",");
                                dataFile.print(soc);
                                dataFile.print(",");
                                dataFile.print(tempatureReadings);
                                dataFile.print(",");
                                dataFile.print(ElapsedTime);
                                 dataFile.print(",");
                                dataFile.print(gradient,4);
                                dataFile.print(",");
                                dataFile.print(soh);
                                dataFile.print(",");
                                dataFile.println(N);
                                set=0;
                                dataFile.close();
                  Serial.println(F(" Data Successfully!! logged in SD-card"));
                  Serial.println(F("-------------------------------------------"));
//                   File dataFile = SD.open("datalog.csv", FILE_WRITE);
                 
                }  
                // if the file isn't open, pop up an error:
                else {
                  Serial.println(F("error opening datalog.csv"));
                  Serial.println(F("-------------------------------------------"));
                  
                } 
          N=0;
          
          open_circuitVoltage = 0;
  //***********************************************************************
      }
        //delay(50);
}
//*****************************************************************************----------------
//Temprature compensation methode for readjusting the Voltage value

double OCVArry(double tempD, double volt){
		double tempF = ((tempD*9)/5) + 32;
		double startn_Temp = 120;
		double startn_OCV = 12.663;
		double OCV_Temp[] = {2,3,3,5,7,9,12,16,18,22,24,26};
		double OCV_soc[] = {0.0,0.2,0.41,0.59,0.76};
		double OCV_socTrue[] = {0.000001,0.2,0.21,0.18,0.17,0.0};
		
		double tempSteps  = (startn_Temp - tempF)/10;
		int intSteps = (int)tempSteps;
		double fractnStep = tempSteps - intSteps;
		double clstOCV_Step = 0;
                double per = 0.0;

              
		for (int i = 0; i < intSteps; i++){
			clstOCV_Step += OCV_Temp[i];
		}

		clstOCV_Step = clstOCV_Step/1000;
		double OCV_100 = startn_OCV - clstOCV_Step;
		
		OCV_100 = OCV_100 - ((OCV_Temp[intSteps]/1000)* fractnStep);
		double OCVArry[5];

		for (int i = 0; i < 5; i++){
			OCVArry[i] = OCV_100 - OCV_soc[i];
		}
		//System.out.println(OCVArry[0]+" , "+OCVArry[1]+" , "+OCVArry[2]+" , "+OCVArry[3]+" , "+OCVArry[4]);
		
		//if (volt > OCVArry[0]){
                        //volt = OCVArry[0];
                        //Serial.println("");
			//Serial.println("Invalid Voltage Value : Too Big");
		//}else if (volt < OCVArry[4]){
                        // volt = OCVArry[4];
			//Serial.println("Invalid Voltage Value: Too Small");
		//}
		
		int SOC_Step = 0;
		for (int i = 0; i < 5; i++){
			if (OCVArry[i] > volt){
				SOC_Step++;
			}
		}
		double SOC_fractn;
		if (SOC_Step == 0){
			
			SOC_fractn = 1;	
	}
		else{
		//System.out.println(SOC_Step);
		SOC_fractn = (OCVArry[SOC_Step-1] - volt)/OCV_socTrue[SOC_Step];
		SOC_fractn = SOC_fractn;}
                
		if (volt > OCVArry[0]){per = 100.0;}else if (volt < OCVArry[4]){per = 0.0;}else{per = 100 - ((25*(SOC_Step-1))+(SOC_fractn*25));}
		
		//double per = 100 - ((25*(SOC_Step-1))+(SOC_fractn*25));

		return per;
		
	}
	

//------------**************************************************************
        boolean startSDCard() {
          boolean result = false;
          Serial.println(F("-------------------------------------------------------"));
          Serial.print(F("Initializing SD card..."));
         // pinMode(10, OUTPUT);
        
          // see if the card is present and can be initialized:
          if (!SD.begin(10)) {
            Serial.println(F("Card failed, or not present"));
            // don't do anything more:
           
            result = false;
          } 
          else {
            Serial.println(F("card initialized."));
           
             File dataFile = SD.open("datalog.csv", FILE_WRITE);
            if (dataFile) {
              dataFile.println();
              dataFile.println("CURRENT,VOLTAGE,TIME_TAKEN,CRANK_No,BMS_OCV,VOLTAGE_LOSS,SOC%,BATTERY_TEMPEATEURE,CRANK_DURATION,RESISTANCE,SOH%,No._OF_VALUES");
              
              dataFile.close();
              result = true;
              Serial.println(F("writing -------."));
            }else{
            Serial.println(F("nooooooo--.")); };
          }  
          return result;
        }
  //-------------*******************************************************
