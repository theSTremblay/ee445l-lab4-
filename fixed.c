// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO)
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground
#include <stdio.h>
#include <stdint.h>
#include "string.h"
#include "ST7735.h"
#include "PLL.h"
#include "fixed.h"
#include "../inc/tm4c123gh6pm.h"
#include <math.h>

// const will place these structures in ROM

int maxY_value;
int maxX_value;

int minx_scale;
int miny_scale;

//Has issues with the 5th value
/****************ST7735_sDecOut2***************
 converts fixed point number to LCD
 format signed 32-bit with resolution 0.01
 range -99.99 to +99.99
 Inputs:  signed 32-bit integer part of fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
 12345    " **.**"
  2345    " 23.45"  
 -8100    "-81.00"
  -102    " -1.02" 
    31    "  0.31" 
-12345    "-**.**"
 */ 
void ST7735_sDecOut2(int32_t value_Decimal){
	int number_of_digits;
	char Ascii_Array[7];
	int i;
	//bool negative_flag;
	//negative_flag = false;
	number_of_digits = 0;
	Ascii_Array[(6)] = 0;

	if(value_Decimal < 0){
		//negative_flag = true;
		Ascii_Array[(0)] = (0x2D);//this is the "-" 
		
		//Fill rest of array
		Ascii_Array[(6)] = 0;
		Ascii_Array[(5)] = 0;
		Ascii_Array[(4)] = 0;
		Ascii_Array[(3)] = 0x2E;
		Ascii_Array[(2)] = 0;
		Ascii_Array[(1)] = (0x20);//space
		value_Decimal = -1 * value_Decimal;
	}
	else{
		Ascii_Array[(0)] = (0x20);
		Ascii_Array[(1)] = (0x20);
		
		//Fill rest of arrray
		Ascii_Array[(6)] = 0;
		
		Ascii_Array[(5)] = 0;
		Ascii_Array[(4)] = 0;
		Ascii_Array[(3)] = 0x2E;
		Ascii_Array[(2)] = 0;
		Ascii_Array[(1)] = (0x20);
	}
	
	// checks if magnitude less than 10, if not thne get the modulo and this becomes the next lowest ASCII value to be returned 
	
	while(((value_Decimal / 10)) > 0 && number_of_digits < 5){
		int value_Modulo;
		value_Modulo = value_Decimal%10;
		value_Decimal = value_Decimal / 10;
		if(number_of_digits < 4){
			Ascii_Array[(5- number_of_digits)] = (0x30 + value_Modulo);  //Might be an issue with compiler for doing hex and dec adds 
		}
		number_of_digits += 1;
		if(number_of_digits == 2){
			Ascii_Array[3] = 0x2E; //THis is the period ASCII
			number_of_digits += 1;
		}
	}
	if(number_of_digits >= 5){
		Ascii_Array[(5)] = 0x2A;
		Ascii_Array[(4)] = 0x2A;
		Ascii_Array[(3)] = 0x2E;
		Ascii_Array[(2)] = 0x2A;
		Ascii_Array[(1)] = 0x2A;
	}
		
	else if(value_Decimal > 0 && number_of_digits < 5){
		//Check edge condition of 0
		if(number_of_digits == 0 && value_Decimal == 0){
			
			Ascii_Array[(5)] = 0x30;
			Ascii_Array[(4)] = 0x30;
			Ascii_Array[(3)] = 0x2E;
			Ascii_Array[(2)] = 0x30;
			Ascii_Array[(1)] = 0x30;
			
			number_of_digits = 5;
		}
		//Put the left nums in 
		else if(number_of_digits >= 2 && number_of_digits < 5){
			Ascii_Array[(5- number_of_digits)] = (0x30 + value_Decimal);  //Might be an issue with compiler for doing hex and dec adds 
			number_of_digits += 1;
		}else if (number_of_digits < 2){
			Ascii_Array[(5- number_of_digits)] = (0x30 + value_Decimal);
			number_of_digits += 1;
		}
		}
	//
	for(i = number_of_digits; i <= 3; i++){
		if(Ascii_Array[5-i]== (0)){
			Ascii_Array[5-i] = (0x30);
		}		
}
	ST7735_OutString(&Ascii_Array[0]);
	}

//This one I've changed the name of the parameter, but I can go back
/**************ST7735_uBinOut6***************
 unsigned 32-bit binary fixed-point with a resolution of 1/64. 
 The full-scale range is from 0 to 999.99. 
 If the integer part is larger than 63999, it signifies an error. 
 The ST7735_uBinOut6 function takes an unsigned 32-bit integer part 
 of the binary fixed-point number and outputs the fixed-point value on the LCD
 Inputs:  unsigned 32-bit integer part of binary fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
     0	  "  0.00"
     1	  "  0.01"
    16    "  0.25"
    25	  "  0.39"
   125	  "  1.95"
   128	  "  2.00"
  1250	  " 19.53"
  7500	  "117.19"
 63999	  "999.99"
 64000	  "***.**"
*/	
void ST7735_uBinOut6(uint32_t value_Decimal){
	
  int32_t fpoint_Decimal;
	float temp_value_for_rounding;
	float Decimal= value_Decimal;
	int number_of_digits;
	char Ascii_Array[7];
	int i;
	Ascii_Array[6] = 0;
	//bool negative_flag = false;
	fpoint_Decimal = 0;
	temp_value_for_rounding = (1000 *Decimal)/640;
	fpoint_Decimal= round(temp_value_for_rounding);
	
	value_Decimal = fpoint_Decimal;
	number_of_digits = 0;
	
	Ascii_Array[(6)] = 0;
		
		Ascii_Array[(5)] = 0;
		Ascii_Array[(4)] = 0;
		Ascii_Array[(3)] = 0x2E;
		Ascii_Array[(2)] = 0;
		Ascii_Array[(1)] = (0x20);
	  Ascii_Array[(0)] = (0x20);

	// checks if magnitude less than 10, if not thne get the modulo and this becomes the next lowest ASCII value to be returned 
	
	while(((value_Decimal / 10)) > 0 && number_of_digits < 6){
		int value_Modulo;
		value_Modulo = value_Decimal%10;
		value_Decimal = value_Decimal / 10;
		if(number_of_digits <= 5){
			Ascii_Array[(5- number_of_digits)] = (0x30 + value_Modulo);  //Might be an issue with compiler for doing hex and dec adds 
		}
 		number_of_digits += 1;
		if(number_of_digits == 2){
			Ascii_Array[3] = 0x2E; //THis is the period ASCII
			number_of_digits += 1;
		}
	}
	if(number_of_digits >= 6){
		Ascii_Array[(5)] = 0x2A;
		Ascii_Array[(4)] = 0x2A;
		Ascii_Array[(3)] = 0x2E;
		Ascii_Array[(2)] = 0x2A;
		Ascii_Array[(1)] = 0x2A;
		Ascii_Array[(0)] = 0x2A;
	}
		
	else if(value_Decimal > 0 && number_of_digits < 6){
		//Check edge condition of 0
		if(number_of_digits == 0 && value_Decimal == 0){
			
			Ascii_Array[(5)] = 0x30;
			Ascii_Array[(4)] = 0x30;
			Ascii_Array[(3)] = 0x2E;
			Ascii_Array[(2)] = 0x30;
			Ascii_Array[(1)] = 0x30;
			
			number_of_digits = 6;
		}
		//Put the left nums in 
		else if(number_of_digits >= 2 && number_of_digits <= 5){
			Ascii_Array[(5- number_of_digits)] = (0x30 + value_Decimal);  //Might be an issue with compiler for doing hex and dec adds 
			number_of_digits += 1;
		}else if (number_of_digits < 2){
			Ascii_Array[(5- number_of_digits)] = (0x30 + value_Decimal);
			number_of_digits += 1;
		}
		}
	//
	for(i = number_of_digits; i <= 3; i++){
		if(Ascii_Array[5-i]== (0)){
			Ascii_Array[5-i] = (0x30);
		}		
}
	ST7735_OutString(&Ascii_Array[0]);
	}
	
/**************ST7735_XYplotInit***************
 Specify the X and Y axes for an x-y scatter plot
 Draw the title and clear the plot area
 Inputs:  title  ASCII string to label the plot, null-termination
          minX   smallest X data value allowed, resolution= 0.001
          maxX   largest X data value allowed, resolution= 0.001
          minY   smallest Y data value allowed, resolution= 0.001
          maxY   largest Y data value allowed, resolution= 0.001
 Outputs: none
 assumes minX < maxX, and miny < maxY
*/	

void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY){

	
	int16_t minx_16;
	int16_t miny_16;
	int16_t minw_16;
	int16_t minh_16;
	//int32_t radius;
	//int32_t radial_center[2];
	
	
	minx_16	= minX;
	miny_16	= minY;
	minw_16	= (maxX - minX);
	minh_16	= (maxY - minY);
	
	ST7735_FillScreen(0);  // set screen to black
   ST7735_SetCursor(0,0);


		if(*title != 0x00){
			ST7735_OutString(title);
		}
	
	//radius = 0;
	//radial_center[0] = minX + ((maxX - minX) /2);
	//radial_center[1] = minY + ((maxY - minY) /2);
	
	//radius = ((maxX - minX) /2);
	
	maxX_value = maxX + abs(minx_16);
	maxY_value = maxY + abs(miny_16);
	
	minx_scale = minx_16;
	miny_scale = miny_16;
	
	//ST7735_FillRect(minx_16, miny_16,minw_16,minh_16, 0xFF);
	//ST7735_SetCursor(minx_16,miny_16);
	
}
/**************ST7735_XYplot***************
 Plot an array of (x,y) data
 Inputs:  num    number of data points in the two arrays
          bufX   array of 32-bit fixed-point data, resolution= 0.001
          bufY   array of 32-bit fixed-point data, resolution= 0.001
 Outputs: none
 assumes ST7735_XYplotInit has been previously called
 neglect any points outside the minX maxY minY maxY bounds
*/
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]){
	int32_t x_value;
	int32_t y_value;
	
	for(int i = 0; i < num; i++){
		x_value = ((bufX[i] - minx_scale)  * 128)/ maxX_value;
		y_value = 160 - (((bufY[i] - miny_scale)* 160) / maxY_value);
		
		ST7735_DrawPixel(x_value,y_value, 0xFF);
	}
}	
