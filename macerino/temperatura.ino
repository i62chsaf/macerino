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

#include "temperatura.h"
#include <math.h>

short temperatura::tendencia()
{
  float  tmedia = media; 
  float x;
  
  x = 100*(temp - media)/temp;
  
    if ( x > media_diferencial )
        return 3;
    else
        if ( x < -media_diferencial )
           return 4;
        else
           return 5;   
}


temperatura temperatura::operator=(const double t) 
{ 
   if ( t < -20 ) 
      temp = temp; // error en la lectura de la sonda. No cambiamos nada
   else 
      temp = t; // Redondeamos a un decimal
      
   if (media == 0)
      media = temp;
   else
      media = media_alpha * temp + (1-media_alpha) * media;
};
