#include<Wire.h>
#define BLYNK_PRINT Serial
#include <Blynk.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include<math.h>

#define ACC_XOFFSET       100
#define ACC_YOFFSET       -600
#define ACC_ZOFFSET       100
#define GY_XOFFSET        -250
#define GY_YOFFSET        120
#define GY_ZOFFSET        -50
#define NORMALIZATION     1000

int buffersize=1000;     //Amount of readings used to average, make it higher to get more precision but sketch will be slower  (default:1000)

//char auth[] = "e1LOmKOs1l2MDKVEx3Xs_3qlHViztt6c"; 
char auth[] = "dCzLnc37KknYwRAAUl7H72go3lWVbchV";       // You should get Auth Token in the Blynk App.
char ssid[] = "Dr.Ehab";                       // Your WiFi credentials.
char pass[] = "0000000009";
 
const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int Acc=0, Prev_Acc=0, Norm_Acc, Prev_AcX, Norm_AcX, Max=0;
 
int minVal=265;
int maxVal=402;
 
double x;
double y;
double z;
 
void setup()
{
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
}
void loop()
{
  Blynk.run();
  
  Prev_AcX = AcX;
  Prev_Acc = Acc;
  read_IMU();
  
  AcX = AcX - ACC_XOFFSET;
  AcY = AcY - ACC_YOFFSET;
  AcZ = (AcZ - ACC_ZOFFSET)*-1;
  GyX = GyX - GY_XOFFSET;
  GyY = GyY - GY_YOFFSET;
  GyZ = GyZ - GY_ZOFFSET;

  Acc = pow((AcX*AcX + AcY*AcY + AcZ*AcZ),0.5);

  Norm_AcX = abs((AcX-Prev_AcX) / NORMALIZATION);
  Norm_Acc = abs((Acc-Prev_Acc) / NORMALIZATION);

  if(Max<Norm_AcX)
    Max=Norm_AcX;

  Serial.print("AcX\tAcY\tAcZ\tGyX\tGyY\tGyZ\n");
  Serial.print(AcX); 
  Serial.print("\t");
  Serial.print(AcY); 
  Serial.print("\t");
  Serial.print(AcZ); 
  Serial.print("\t");
  Serial.print(GyX); 
  Serial.print("\t");
  Serial.print(GyY); 
  Serial.print("\t");
  Serial.print(GyZ);

  Serial.print("\nAcc\tPrev\tNorm\n");
  Serial.print(Acc); 
  Serial.print("\t");
  Serial.print(Prev_Acc); 
  Serial.print("\t");
  Serial.print(Norm_Acc); 
  Serial.print("\n");
   
  Blynk.virtualWrite(V2, Norm_AcX);
  Blynk.virtualWrite(V3, Max);
   
  delay(10);
}

void read_IMU()
{
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  AcX=Wire.read()<<8|Wire.read();
  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read();
  Tmp=Wire.read()<<8|Wire.read();
  GyX=Wire.read()<<8|Wire.read();
  GyY=Wire.read()<<8|Wire.read();
  GyZ=Wire.read()<<8|Wire.read();
}
