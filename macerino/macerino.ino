/*****************************************************************************
    Copyright 2017, Fernando Chacón Sánchez personal@fernandochacon.com
    
    This file is part of Macerino.

    Macerino is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include <Wire.h>
#include <LiquidCrystal.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <PID_v1.h>

#include "macerino.h"
#include "hlt.h"
#include "mlt.h"


// Inicialización del LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


HLT hlt; // Hot liquor tank
MLT mlt; // Tamque de maceración


PID myPID(&mlt.temp_entrada.temp, &hlt.temp_deseada.temp, &mlt.temp_deseada.temp, 5, 10, 1, DIRECT);


#define PinResistencia 3  // Pin de control del SSR y calentador
#define PinHLT 11  // Pin de lectura de la sonda de temperatura del HLT
#define PinMLTout 12  // Pin de lectura de la sonda de temperatura de la salida del MLT
#define PinMLTin 13  // Pin de lectura de la sonda de temperatura de la entrade del MLT
#define PinTempMas 2  // Pin de conexión del pulsador para incrementar la temperatura de maceración
#define PinTempMen 1  // Pin de conexión del pulsador para decrementar la temperatura de maceración

OneWire WireHLT(PinHLT); // Inicialización del protocolo 1wire con el HLT
DallasTemperature sensorHLT(&WireHLT);
OneWire WireMLTout(PinMLTout); // Inicialización del protocolo 1wire con el MLT out
DallasTemperature sensorMLTout(&WireMLTout); 
OneWire WireMLTin(PinMLTin); // Inicialización del protocolo 1wire con el MLT in
DallasTemperature sensorMLTin(&WireMLTin); 

// Simbolos a usar en la pantalla
byte letraR[8] = { B11111, B10011, B10101, B10101, B10011, B10101, B11111, B00000};
byte letraE[8] = { B11111, B10001, B10111, B10011, B10111, B10001, B11111, B00000};
byte letraUP[8] = { B00100, B01110, B10101, B00100, B00100, B00100, B00100, B00000};
byte letraDOWN[8] = { B00100, B00100, B00100, B00100, B10101, B01110, B00100, B00000};
byte letraEQ[8] = { B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000};
byte letraH[8] = { B11111, B10101, B10101, B10001, B10101, B10101, B11111};

char tmp[6];

void setup() {
  Serial.begin(9600);

  
  lcd.begin(16, 2);
  lcd.createChar(1, letraR);
  lcd.createChar(2, letraE);
  lcd.createChar(3, letraUP);
  lcd.createChar(4, letraDOWN);
  lcd.createChar(5, letraEQ);
  lcd.createChar(6, letraH);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write("    Macerino    ");
  delay(1000);
  
  lcd.setCursor(0, 1);
  lcd.write(" Copyright 2017 ");
  delay(1500);
  lcd.setCursor(0, 1);
  lcd.write("F Chacon Sanchez");
  delay(1500);

  pinMode(PinTempMas, INPUT);
  pinMode(PinTempMen, INPUT);

  pinMode(PinHLT, OUTPUT);
  pinMode(PinMLTout, OUTPUT);
  pinMode(PinMLTin, OUTPUT);

  pinMode(PinResistencia, OUTPUT);
  
  sensorHLT.begin();    //Se inician los sensores
  sensorMLTout.begin(); 
  sensorMLTin.begin(); 

  mlt.temp_deseada=66;  // Temperatura de maceración por defecto


  mlt.temp_entrada = mlt.temp_deseada;
  mlt.temp_salida = mlt.temp_deseada;
  leerSondas();

  myPID.SetOutputLimits(mlt.temp_deseada - 10, mlt.temp_deseada + 10);
  myPID.SetMode(AUTOMATIC);

  Serial.print("Macerador salida: ");
  Serial.println(mlt.temp_salida);  

  lcd.clear();
}

void loop() 
{
   configurarTemperatura();   // Lee los botones para establecer la temperatura de maceración 
   actuacionesTemperatura();  // Lectura de sodas de temperatura, configuración de la temperatura de trabajo y activación de la resistencia

   byte x = (millis()/300)%6; // Una vez de cada cuatro medios segundos
   if (x == 0)
      imprimirTendencias();
   if (x == 1)
      borrarTendencias();
}

void actuacionesTemperatura(void)
{
  if ( (millis()/500)%8 == 0 )
  {
     lcd.setCursor(0, 0);
     lcd.write(1);

     leerSondas();

//     hlt.temp_deseada = mlt.temp_deseada;
//     myPID.SetOutputLimits(mlt.temp_deseada - 5, mlt.temp_deseada + 3);
     myPID.Compute();


     if ( hlt.temp_deseada > hlt.temp_salida )
        encenderResistencia();     // Activamos la resistencia del HLT
     else
        apagarResistencia();
       
      lcd.setCursor(0, 0);
      lcd.write(" ");
      imprimirEstado();  
  }
}

void leerSondas(void)
{
     sensorHLT.requestTemperatures(); //Prepara el sensor para la lectura
     sensorMLTin.requestTemperatures(); //Prepara el sensor para la lectura
     sensorMLTout.requestTemperatures(); //Prepara el sensor para la lectura

     mlt.temp_entrada = sensorMLTin.getTempCByIndex(0);
     mlt.temp_salida = sensorMLTout.getTempCByIndex(0);
     hlt.temp_salida = sensorHLT.getTempCByIndex(0);
}

void imprimirEstado(void)
{
   lcd.setCursor(0, 0);
//   lcd.write(1);
   lcd.setCursor(1, 0);
   lcd.print(ftoa(tmp, mlt.temp_salida));
   lcd.setCursor(6, 0);
   lcd.print(ftoa(tmp, hlt.temp_salida));
   lcd.setCursor(11, 0);
   lcd.print(ftoa(tmp, mlt.temp_entrada));

   lcd.setCursor(6, 1);
   lcd.print(ftoa(tmp, hlt.temp_deseada));
   lcd.setCursor(11, 1);
   lcd.print(ftoa(tmp, mlt.temp_deseada));

   Serial.print("MLT(obj): ");
   Serial.print(mlt.temp_deseada);
   Serial.print(", HLT(obj): ");
   Serial.print(hlt.temp_deseada);
   Serial.print(", MLT(out): ");
   Serial.print(mlt.temp_salida);
   Serial.print(", HLT(out): ");
   Serial.print(hlt.temp_salida);
   Serial.print(", MLT(in): ");
   Serial.print(mlt.temp_entrada);
 
   Serial.println();
}

void imprimirTendencias(void)
{
    lcd.setCursor(5, 0); 
    lcd.write(mlt.temp_salida.tendencia());
    lcd.setCursor(10, 0); 
    lcd.write(hlt.temp_salida.tendencia());
    lcd.setCursor(15, 0); 
    lcd.write(mlt.temp_entrada.tendencia());
//    lcd.setCursor(5, 1); 
//    lcd.write(mlt.temp_estimada.tendencia());
    lcd.setCursor(10, 1); 
    lcd.write(hlt.temp_deseada.tendencia());
    lcd.setCursor(15, 1); 
    lcd.write(mlt.temp_deseada.tendencia());
}

void borrarTendencias(void)
{
   lcd.setCursor(5, 0); 
   lcd.write(" ");
   lcd.setCursor(10, 0); 
   lcd.write(" ");
   lcd.setCursor(15, 0); 
   lcd.write(" ");
   lcd.setCursor(5, 1); 
   lcd.write(" ");
   lcd.setCursor(10, 1); 
   lcd.write(" ");
   lcd.setCursor(15, 1); 
   lcd.write(" ");
}

int configurarTemperatura() {              
   if (analogRead(PinTempMas)>500)
   {
      mlt.temp_deseada = mlt.temp_deseada + 0.1;
      lcd.setCursor(11, 1);
      lcd.print(ftoa(tmp, mlt.temp_deseada));
      delay(800);
      while (analogRead(PinTempMas)>500)  // Esperamos a que suelte el boton
      {
         mlt.temp_deseada = mlt.temp_deseada + 0.5;
         lcd.setCursor(11, 1);
         lcd.print(ftoa(tmp, mlt.temp_deseada));
         delay(200);
      }
      myPID.SetOutputLimits(mlt.temp_deseada - 10, mlt.temp_deseada + 10);
   }

   if (analogRead(PinTempMen)>500)
   {
      mlt.temp_deseada = mlt.temp_deseada - 0.1;
      lcd.setCursor(11, 1);
      lcd.print(ftoa(tmp, mlt.temp_deseada));
      delay(800);
      while (analogRead(PinTempMen)>500)  // Esperamos a que suelte el boton
      {
         mlt.temp_deseada = mlt.temp_deseada - 0.5;
         lcd.setCursor(11, 1);
         lcd.print(ftoa(tmp, mlt.temp_deseada));
         delay(200);
      }
      myPID.SetOutputLimits(mlt.temp_deseada - 10, mlt.temp_deseada + 10);
  }
   }

void encenderResistencia(void)
{
   digitalWrite(PinResistencia, HIGH);     // Activamos la resistencia del HLT
   lcd.setCursor(0, 1);
   lcd.write(6);
}
  
void apagarResistencia(void)
{
   digitalWrite(PinResistencia, LOW);     // Desactivamos la resistencia del HLT
   lcd.setCursor(0, 1);
   lcd.print(" ");
}

const char *ftoa(char *a, double f, int precision)
{
  long p[] = {
    0, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000  };

  char *ret = a;
  long heiltal = (long)f;
  itoa(heiltal, a, 10);
  while (*a != '\0') a++;
  *a++ = '.';
  long desimal = abs((long)((f - heiltal) * p[precision]));
  itoa(desimal, a, 10);

  return ret;
}


