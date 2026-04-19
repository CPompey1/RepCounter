#include <Arduino.h>
#include "LSM6DSOX.h"
#include <stdio.h>
#include <stdlib.h>
#include <cQueue.h>
#include <Thread.h>
#define LFS_MBED_RP2040_VERSION_MIN_TARGET      "LittleFS_Mbed_RP2040 v1.1.0"
#define LFS_MBED_RP2040_VERSION_MIN             1001000

#define _LFS_LOGLEVEL_          1
#define RP2040_FS_SIZE_KB       128

#define FORCE_REFORMAT          false
            
#include <LittleFS_Mbed_RP2040.h>
const char REP_LOG_PATH[] = MBED_LITTLEFS_FILE_PREFIX "/log1.txt";     
#define SET_TIME_LENGTH_uS 20000000 // 20 seconds

LittleFS_MBED *myFS;
unsigned long startTime = 0;
#define USE_TIMER_1     true
#define TIMER0_INTERVAL_MS  1000 // Record data every 1000 ms (1 second)

#define		NB_ITEMS	18000

#define BUTTON_PIN 6

#define X_CAL -0.01f
#define Y_CAL 0.04f
#define Z_CAL -1.01f
typedef struct accelData {
  float x;
  float y;
  float z;
} accelData_t;
accelData_t			q_dat[NB_ITEMS];
Queue_t		q;	// Queue declaration
unsigned long timeTaken = 0;

void readCharsFromFile(const char * path);
void readFile(const char * path);
void writeFile(const char * path, const char * message, size_t messageSize);
void appendFile(const char * path, const char * message, size_t messageSize);
void deleteFile(const char * path);
void renameFile(const char * path1, const char * path2);
void testFileIO(const char * path);
void printLine();
void recordData();
void readRecordedData();
void niceCallback();
Thread myThread = Thread();

// callback for myThread
void niceCallback(){
	Serial.print("COOL! I'm running on: ");
	Serial.println(micros());
}

void readCharsFromFile(const char * path) 
{
  Serial.print("readCharsFromFile: "); Serial.print(path);

  FILE *file = fopen(path, "r");
  
  if (file) 
  {
    Serial.println(" => Open OK");
  }
  else
  {
    Serial.println(" => Open Failed");
    return;
  }

  char c;

  while (true) 
  {
    c = fgetc(file);
    
    if ( feof(file) ) 
    { 
      break;
    }
    else   
      Serial.print(c);
  }
   
  fclose(file);
}

void readFile(const char * path) 
{
  Serial.print("Reading file: "); Serial.print(path);

  FILE *file = fopen(path, "r");
  
  if (file) 
  {
    Serial.println(" => Open OK");
  }
  else
  {
    Serial.println(" => Open Failed");
    return;
  }

  char c;
  uint32_t numRead = 1;
  
  while (numRead) 
  {
    numRead = fread((uint8_t *) &c, sizeof(c), 1, file);

    if (numRead)
      Serial.print(c);
  }
  
  fclose(file);
}

void writeFile(const char * path, const char * message, size_t messageSize) 
{
  Serial.print("Writing file: "); Serial.print(path);

  FILE *file = fopen(path, "w");
  
  if (file) 
  {
    Serial.println(" => Open OK");
  }
  else
  {
    Serial.println(" => Open Failed");
    return;
  }
 
  if (fwrite((uint8_t *) message, 1, messageSize, file)) 
  {
    Serial.println("* Writing OK");
  } 
  else 
  {
    Serial.println("* Writing failed");
  }
  
  fclose(file);
}

void appendFile(const char * path, const char * message, size_t messageSize) 
{
  Serial.print("Appending file: "); Serial.print(path);

  FILE *file = fopen(path, "a");
  
  if (file) 
  {
    Serial.println(" => Open OK");
  }
  else
  {
    Serial.println(" => Open Failed");
    return;
  }

  if (fwrite((uint8_t *) message, 1, messageSize, file)) 
  {
    Serial.println("* Appending OK");
  } 
  else 
  {
    Serial.println("* Appending failed");
  }
   
  fclose(file);
}

void deleteFile(const char * path) 
{
  Serial.print("Deleting file: "); Serial.print(path);
  
  if (remove(path) == 0) 
  {
    Serial.println(" => OK");
  }
  else
  {
    Serial.println(" => Failed");
    return;
  }
}
void renameFile(const char * path1, const char * path2) 
{
  Serial.print("Renaming file: "); Serial.print(path1);
  Serial.print(" to: "); Serial.print(path2);
  
  if (rename(path1, path2) == 0) 
  {
    Serial.println(" => OK");
  }
  else
  {
    Serial.println(" => Failed");
    return;
  }
}

void testFileIO(const char * path) 
{
  Serial.print("Testing file I/O with: "); Serial.print(path);

  #define BUFF_SIZE     512
  
  static uint8_t buf[BUFF_SIZE];
  
  FILE *file = fopen(path, "w");
  
  if (file) 
  {
    Serial.println(" => Open OK");
  }
  else
  {
    Serial.println(" => Open Failed");
    return;
  }

  size_t i;
  Serial.println("- writing" );
  
  uint32_t start = micros();

  size_t result = 0;

  // Write a file only 1/4 of RP2040_FS_SIZE_KB
  for (i = 0; i < RP2040_FS_SIZE_KB / 2; i++) 
  {
    result = fwrite(buf, BUFF_SIZE, 1, file);

    if ( result != 1)
    {
      Serial.print("Write result = "); Serial.println(result);
      Serial.print("Write error, i = "); Serial.println(i);

      break;
    }
  }
  
  Serial.println("");
  uint32_t end = micros() - start;
  
  Serial.print(i / 2);
  Serial.print(" Kbytes written in (ms) ");
  Serial.println(end);
  
  fclose(file);

  printLine();

  /////////////////////////////////

  file = fopen(path, "r");
  
  start = micros();
  end = start;
  i = 0;
  
  if (file) 
  {
    start = micros();
    Serial.println("- reading" );

    result = 0;

    fseek(file, 0, SEEK_SET);

    // Read file only 1/4 of RP2040_FS_SIZE_KB
    for (i = 0; i < RP2040_FS_SIZE_KB / 2; i++) 
    {
      result = fread(buf, BUFF_SIZE, 1, file);

      if ( result != 1 )
      {
        Serial.print("Read result = "); Serial.println(result);
        Serial.print("Read error, i = "); Serial.println(i);

        break;
      }
    }
      
    Serial.println("");
    end = micros() - start;
    
    Serial.print((i * BUFF_SIZE) / 1024);
    Serial.print(" Kbytes read in (ms) ");
    Serial.println(end);
    
    fclose(file);
  } 
  else 
  {
    Serial.println("- failed to open file for reading");
  }
}

void printLine()
{
  Serial.println("====================================================");
}

void TimerHandler0()
{
  recordData();
}
void setup() 
{
  Serial.begin(115200);
  Serial.flush();
  while (!Serial)

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Accelerometer in m/s^2");
  Serial.println("X\tY\tZ");
  
  delay(1000);

  Serial.print("Starting LITTLEFS on MBED RP2040 ");
  Serial.println(LFS_MBED_RP2040_VERSION_MIN_TARGET);
  delay(1000);

  q_init_static(&q, sizeof(accelData_t), NB_ITEMS, FIFO, false, q_dat, NB_ITEMS * sizeof(accelData_t));
  Serial.println("Waiting for button press to start recording data...");
  
  while ( Serial.read() != '\n')
  {
  }
  Serial.println("3");
  delay(1000);
  Serial.println("2");
  delay(1000);
  Serial.println("1");
  delay(1000);
  Serial.println("0");
  digitalWrite(LED_BUILTIN, HIGH);
  startTime = micros();
  Serial.println("Started recording data...");
  Serial.flush();

}

void loop() {
  recordData();
  if ( micros() - startTime > SET_TIME_LENGTH_uS || Serial.available())
  {
    
    Serial.println("Finished recording data");
    delay(2000);
    Serial.read();
    Serial.readStringUntil('\n');
    Serial.flush();
    Serial.println("Press button to get data...");
    while ( Serial.read() != '\n')
    {
    }
    timeTaken = micros() - startTime;
    Serial.println("Finished recording data");
    readRecordedData();
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Time taken for recording: ");
    Serial.print(timeTaken / 1000000);
    Serial.println(" seconds");
    while(1);
  }
}
void recordData(){
  float x, y, z;
  char buf[64];
  unsigned long currentTime = micros();
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x,y ,z );
    accelData_t data = {x + X_CAL , y + Y_CAL,  z + Z_CAL};
     q_push(&q, &data);
    
    // delayMicroseconds(570); 
    //  delay(1); // Delay to ensure we don't overwhelm the queue and to simulate a realistic sampling rate. Adjust as needed. 
    // Serial.print("Recording data (press button to stop)... ");
  }
  // snprintf(buf, 64, "%f,%f,%f\n", x, y, z);
  
  // appendFile(REP_LOG_PATH, buf, strlen(buf));
 
}


void readRecordedData() 
{
  // Serial.println("Reading recorded data...");
  // readFile(REP_LOG_PATH);
  Serial.print("x,y,z\n");
  for (int i = 0; i < NB_ITEMS; i++) {
    accelData_t data;
    if (q_pop(&q, &data)) {
      
      Serial.print(data.x);
      Serial.print(",");
      Serial.print(data.y);
      Serial.print(",");
      Serial.println(data.z);
    } else {
      Serial.print("No more data in queue at index ");
      Serial.println(i);
      break;
    }
  }
}
