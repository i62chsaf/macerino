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

#ifndef CLASS_TEMPERATURA
#define CLASS_TEMPERATURA

#include "sonda.h"

struct  temperatura
{     
  public:

     temperatura()   { temp = 0; media_diferencial = 0.5; media_alpha = 0.3; };
     temperatura  operator=(const double);
     float media_diferencial; // Solo como referencia para saber si la tendencia de la temperatura es a subir o a bajar
     operator double() { return temp; } ;
     short tendencia(); // Caracter a imprimir en funcion de la subida/bajada de la temperatura

     Sonda sonda;
     double temp;
     double media;
     float media_alpha = 0.3;    // Para el calculo de la media utilizando no se que formula rara

};

#endif
