#include "LPD8806.h"
#include "SPI.h"

                    
short LEDMap[16][16] = {{0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15 },
                        {31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16 }, 
                        {32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47 },
                        {63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48 },
                        {64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79 },
                        {95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80 },
                        {96, 97, 98, 99, 100,101,102,103,104,105,106,107,108,109,110,111},
                        {127,126,125,124,123,122,121,120,119,118,117,116,115,114,113,112},
                        {128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143},
                        {159,158,157,156,155,154,153,152,151,150,149,148,147,146,145,144},
                        {160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175},
                        {191,190,189,188,187,186,185,184,183,182,181,180,179,178,177,176},
                        {192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207},
                        {223,222,221,220,219,218,217,216,215,214,213,212,211,210,209,208},
                        {224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239},
                        {255,254,253,252,251,250,249,248,247,246,245,244,243,242,241,240}};
int shapes[28] = {0x0066, 0x0066, 0x0066, 0x0066,0x2222, 0x00F0, 0x2222, 0x00F0, 0x006C, 0x0462, 0x006C, 0x0462, 0x00E4, 0x04C4, 0x04E0, 0x0464, 0x00E8, 0x0C44, 0x02E0, 0x0446, 0x00E2, 0x044C, 0x08E0, 0x0644, 0x00C6, 0x0264, 0x00C6, 0x0264}; 	//defines each shape
//		  Square 4 times 		 Cyan vert, horiz, vert, horiz   Redzig, RZturn	Blue triangle	Orange L	Magenta J						GrnZig  GZturn
char blocks[16][10];  //16 rows, 10 columns. Row 15  on bottom, col 0 on left
int offX = 0; //Global X offset
int offY = 0; //Global Y offset
int scale = 2; //Scale (text size)
int score = 0;
int high = 0;
int actRow = 0; //Row of the active moving piece
int actCol = 4; //Column of the active moving piece
int aPiece = 0; //Which piece is active
int nPiece = 0;	//Which piece will come next
char pieceOff = 0;
char pause = 0;
char gameover = 0;
char count = 1, count2 = 0; //For timer
char val;
long delaytime = 10000;
int bright = 10;
unsigned int v1, v2;

//Controller
// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 2;
int clockPin = 3;
int pot1 = A0;
int pot2 = A1;
int pot3 = A2;
int ButtonA = 4;
int ButtonB = 5;
int ButtonUp = 6;
int ButtonDown = 7;
int ButtonLeft = 8;
int ButtonRight = 9;
int redIn = 0, greenIn = 0, blueIn = 0;


// Data Pin is 11, Clock Pin is 13
LPD8806 strip = LPD8806(256, A5, A4);
LPD8806 strip2 = LPD8806(2, 2, 3); //# LED, dataPin, ClockPin

uint32_t RED = strip.Color(10*bright,0,0);
uint32_t GRN = strip.Color(0,0,10*bright);
uint32_t BLU = strip.Color(0,10*bright,0);
uint32_t BLU2 = strip.Color(1*bright,1*bright,1*bright);
uint32_t BLK = strip.Color(0,0,0);
uint32_t CYAN = strip.Color(0,8*bright,6*bright);
uint32_t MAG = strip.Color(10*bright,4*bright,0);
uint32_t YELLOW = strip.Color(10*bright,0,5*bright);
uint32_t ORG = strip.Color(9*bright,0,2*bright);
uint32_t SC = strip.Color(3*bright,0,0);
uint32_t SC2 = strip.Color(0,0,3*bright);
uint32_t SC3 = strip.Color(0,3*bright,0);
uint32_t SC4 = strip.Color(2*bright,2*bright,0);



void setup() {
  pinMode(ButtonA, INPUT);
  pinMode(ButtonB, INPUT);
  pinMode(ButtonUp, INPUT);
  pinMode(ButtonDown, INPUT);
  pinMode(ButtonLeft, INPUT);
  pinMode(ButtonRight, INPUT);
  strip2.begin();
  
  Serial.begin(9600);
  //pinMode(8, OUTPUT);
  //digitalWrite(8, HIGH); //bluetooth
  strip.begin();
  strip.show();
  clearBoard();
  drawBG();
  for(int i=0; i<10; i++)
    nPiece=(((random(0,7))*4));
  drawNext(nPiece);
}

void loop() {
  /*for(int p=0; p< 256; p++)
  {
    strip.setPixelColor(p, BLU);
  }
  strip.show();
  delay(100000);*/
  tick();
  for (long i = 0; i < delaytime; i++)
  {
    controller(); //polling for button presses
    //delay(1);
  }
    
}
void clearBoard(void)
{
  int ttt, rrr;
    for (ttt=0; ttt<16; ttt++)
      for(rrr=0; rrr<10; rrr++)
	blocks[ttt][rrr] = 0x00;	//clear out the array.
}
void moveIt(char m){
	if (m == 'r')
		{
			if (downCheck(3))
			{
				clearPieceRight(aPiece+pieceOff);
				actCol++;
				drawPiece(aPiece+pieceOff);
			}
		}
	else if (m == 'l')
		{
			if (downCheck(4))
			{
				clearPieceLeft(aPiece+pieceOff); //clear other side
				actCol--;
				drawPiece(aPiece+pieceOff);
			}
		}

	if (m == 'd')
		{
			if (downCheck(0))
			{
				clearPiece(aPiece+pieceOff);
				actRow++;
				drawPiece(aPiece+pieceOff);
			}
		}
}

void drawPiece(char piece)
{
	uint32_t color1;
	switch (piece/4)
	{
	case 0: color1 = YELLOW;
		break;
	case 1: color1 = CYAN;
		break;
	case 2: color1 = RED;
		break;
	case 3: color1 = BLU;
		break;
	case 4: color1 = ORG;
		break;
	case 5: color1 = MAG;
		break;
	case 6: color1 = GRN;
		break;
	default: color1 = RED;
	}
	int i, shaper, here = 3;
	shaper = shapes[piece];
	for (i = 0; i < 4; i++)
	{
		if (shaper & 0x0001){
			//drawSquare(((actRow)*16), (here+actCol)*16,color1, color2);
                        if(actRow >= 0 && here+actCol >=0)
                          strip.setPixelColor(LEDMap[actRow][here+actCol], color1);
                        strip.show();}
		here--;
		shaper = shaper >> 1;
	}
	here = 3;
	for (i = 0; i < 4; i++)
	{
		if (shaper & 0x0001){
			//drawSquare((actRow-1)*16, (here+actCol)*16,color1, color2);
                        if(actRow-1 >= 0 && here+actCol >=0)
                          strip.setPixelColor(LEDMap[actRow-1][here+actCol], color1);
                        strip.show();}
		here--;
		shaper = shaper >> 1;
	}
	here = 3;
	for (i = 0; i < 4; i++)
	{
		if (shaper & 0x0001){
			//drawSquare((actRow-2)*16, (here+actCol)*16,color1, color2);
                        if(actRow-2 >= 0 && here+actCol >=0)
                          strip.setPixelColor(LEDMap[actRow-2][here+actCol], color1);
                        strip.show();}
		here--;
		shaper = shaper >> 1;
	}
	here = 3;
	for (i = 0; i < 4; i++)
	{
		if (shaper & 0x0001){
			//drawSquare((actRow-3)*16, (here+actCol)*16,color1, color2);
                        if(actRow-3 >= 0 && here+actCol >=0)
		          strip.setPixelColor(LEDMap[actRow-3][here+actCol], color1);
                        strip.show();}
		here--;
		shaper = shaper >> 1;
	}
}

char downCheck(char check){
	int actRowNew = actRow;
	int actColNew = actCol;
	int i, shaper,j,pOff = pieceOff;
	switch (check)
	{
		case 0: actRowNew = actRow+1;	//Will move the piece down and see if it collides.
				break;
		case 1:	pOff++;					//Will rotate the piece and see if it collides
				if (pOff > 3) pOff = 0;
				break;
		case 2: if (pOff == 0) pOff = 3;
				else pOff--;
				break;
		case 3: actColNew = actCol+1;
				break;
		case 4: actColNew = actCol-1;
				break;
		default: break;
	}
	shaper = shapes[aPiece+pOff];
	for (i = 0; i < 4; i++)   //Check bottom row of piece
	{
		for (j=actColNew+3; j >= actColNew; j--)   //start from the right side of piece to left
		{
			if ((shaper & 0x0001) && j > 9) return 0;	//If there would be a piece out of bounds
			if ((shaper & 0x0001) && j < 0) return 0;	//If there would be a piece out of bounds
			if ((shaper & 0x0001) && actRowNew > 15) return 0;	//If there would be a piece out of bounds
                        if(actRowNew > 0)
        		  if((blocks[actRowNew][j] > 0) && (shaper & 0x0001))  //If there is a landed block and a current block in the same place
        		  {
        		    return 0; //Collision! you fail!!
        		  }
			shaper = shaper >> 1;
		}
		actRowNew--;
	}
	return 1; //No collisions. We can move!
}


void land (void)
{
	int i,shaper,j,o;
	char gotline=0;
	o = actRow;
	int piece = aPiece+pieceOff;
	shaper = shapes[piece];
	for (i = 0; i < 4; i++)   //Check bottom row of piece
	{
		for (j=actCol+3; j >= actCol; j--)   //start from the right side of piece to left
		{
			if ((shaper & 0x0001) && o < 0) //If there would be a piece out of bounds
				{
					pause = 1;
					gameover = 1;
				}
			if(shaper & 0x0001) //If there is a current block
			{
				blocks[o][j]= ((piece/4)+1);	//0 means no block, anything else represents color
			}
			shaper = shaper >> 1;
		}
		o--;
	}
        if(gameover)
        {
          for(int i = 0; i< 35; i++)
            firework();
          drawBG();
          for(int i = 0; i<= score; i++)
            scorer(i); //Redraw final score after explosion
          redraw();
        }

	//Check to see if any rows are finished
	for (i = 0; i < 20; i++)
	{
		if (blocks[i][0] && blocks[i][1] && blocks[i][2] && blocks[i][3] && blocks[i][4] && blocks[i][5] && blocks[i][6] && blocks[i][7] && blocks[i][8] && blocks[i][9])
		{
			score++;	//We got a line, increment score
                        scorer(score);
                        delaytime = delaytime - 1000;
						//Move all other lines down
			gotline=1;
			for (j=i; j>0;j--)
			{
				for(o=0;o<10;o++)
					blocks[j][o] = blocks[j-1][o];	//moving line above down one
			}
		}
	}
	if (gotline)
		redraw();

}

void redraw(void)
{
	int i,j;
        uint32_t color1;
	for (i = 0; i < 16; i++)
	{
		for(j=0;j<10;j++)
		{
			if (blocks [i][j])
			{
				switch(blocks [i][j]-1)
				{
					case 0: color1 = YELLOW;
						break;
					case 1: color1 = CYAN;
						break;
					case 2: color1 = RED;
						break;
					case 3: color1 = BLU;
						break;
					case 4: color1 = ORG;
						break;
					case 5: color1 = MAG;
						break;
					case 6: color1 = GRN;
						break;
					default: color1 = RED;
				}
				//drawSquare((i*16), (j)*16,color1, color2);
                                strip.setPixelColor(LEDMap[i][j], color1);
                                strip.show();
			}
			//else clearSquare((i), (j));
                        else {strip.setPixelColor(LEDMap[i][j], BLK); strip.show();}
		}
	}


	
	/*if (score > high)
	{
		high = score;
		int returned = 0;
		char half, half2;
		half = (char)high;
		half2 = (char)((high >> 8) & 0x00ff);
		writeSRAM(0x0000, half);
		writeSRAM(0x0002, half2);
		returned = readSRAM(0x0002);
		returned = returned << 8;
		returned = returned + (readSRAM(0x0000));
		higher(returned);
	}*/
}

void scorer(int s)
{
  s = s - 1;
    if (s < 50)
    {
      strip.setPixelColor(LEDMap[15-(s%10)][15-(s/10)], SC);
      strip.show();
    }
    else if(s > 49 && s < 100)
    {
      strip.setPixelColor(LEDMap[15-((s-50)%10)][15-((s-50)/10)], SC2);
      strip.show();
    }
    else if(s > 99 && s < 150)
    {
      strip.setPixelColor(LEDMap[15-((s-100)%10)][15-((s-100)/10)], SC3);
      strip.show();
    }
    else if(s > 149 && s < 200)
    {
      strip.setPixelColor(LEDMap[15-((s-150)%10)][15-((s-150)/10)], SC4);
      strip.show();
    }
}

void clearPiece(char piece)
{
		if (piece < 4){
			//clearSquare(-16+(actRow*16), 32+(actCol*16)); //Yellow Square
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 4 || piece == 6){
			//clearSquare(-48+(actRow*16), 32+(actCol*16)); //Cyan Line
                        strip.setPixelColor(LEDMap[actRow-3][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 5 || piece == 7){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Cyan Line Horiz
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
			//clearSquare(-16+(actRow*16), 48+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+3], BLK);
                        strip.show();
		}
		else if (piece == 8 || piece == 10){
			//clearSquare(0+(actRow*16), 0+(actCol*16)); //Red Zig
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 9 || piece == 11){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Red Zig turned
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 12){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Blue Triangle
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 13){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Blue Triangle
			//clearSquare(-32+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 14){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Blue Triangle
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
			//clearSquare(-32+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.show();
			}
		else if (piece == 15){
			//clearSquare(-16+(actRow*16), 32+(actCol*16)); //Blue Triangle
			//clearSquare(-32+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.show();

		}
		else if (piece == 16){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Orange L
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 17){
			//clearSquare(-32+(actRow*16), 0+(actCol*16)); //Orange L
			//clearSquare(-32+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 18){
			//clearSquare(-16+(actRow*16), 16+(actCol*16)); //Orange L
			//clearSquare(-16+(actRow*16), 0+(actCol*16));
			//clearSquare(-32+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 19){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Orange L
			//clearSquare(0+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 20){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Magenta J
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 21){
			//clearSquare(0+(actRow*16), 0+(actCol*16)); //Magenta J
			//clearSquare(-32+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 22){
			//clearSquare(-16+(actRow*16), 16+(actCol*16)); //Magenta J
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
			//clearSquare(-32+(actRow*16), 0+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol], BLK);
                        strip.show();
		}
		else if (piece == 23){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Magenta J
			//clearSquare(-32+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 24 || piece == 26){
			//clearSquare(0+(actRow*16), 32+(actCol*16));  //Green Zig
			//clearSquare(-16+(actRow*16), 0+(actCol*16));
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 25 || piece == 27){
			//clearSquare(-32+(actRow*16), 32+(actCol*16));  //Green Zig
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK); 
                        strip.show();   
			}
}

void drawNext(char piece){
        for(int c = 1; c < 5; c++)
          for(int c2 = 11; c2 < 15; c2++)
        strip.setPixelColor(LEDMap[c][c2], BLK); //Clear last piece
	if (piece < 4){
                        strip.setPixelColor(LEDMap[2][12], YELLOW); 
                        strip.setPixelColor(LEDMap[2][13], YELLOW); 
                        strip.setPixelColor(LEDMap[3][12], YELLOW); 
                        strip.setPixelColor(LEDMap[3][13], YELLOW); 
                        strip.show();
		}
		else if (piece > 3 && piece < 8){
                        strip.setPixelColor(LEDMap[1][12], CYAN); 
                        strip.setPixelColor(LEDMap[2][12], CYAN); 
                        strip.setPixelColor(LEDMap[3][12], CYAN); 
                        strip.setPixelColor(LEDMap[4][12], CYAN); 
                        strip.show();
		}
		else if (piece > 7 && piece < 12){
                        strip.setPixelColor(LEDMap[2][12], RED); 
                        strip.setPixelColor(LEDMap[2][13], RED); 
                        strip.setPixelColor(LEDMap[3][11], RED); 
                        strip.setPixelColor(LEDMap[3][12], RED); 
                        strip.show();
		}
		else if (piece > 11 && piece < 16){
                        strip.setPixelColor(LEDMap[3][12], BLU); 
                        strip.setPixelColor(LEDMap[2][11], BLU); 
                        strip.setPixelColor(LEDMap[2][12], BLU); 
                        strip.setPixelColor(LEDMap[2][13], BLU); 
                        strip.show();
		}
		else if (piece > 15 && piece < 20){
                        strip.setPixelColor(LEDMap[3][11], ORG); 
                        strip.setPixelColor(LEDMap[2][11], ORG); 
                        strip.setPixelColor(LEDMap[2][12], ORG); 
                        strip.setPixelColor(LEDMap[2][13], ORG); 
		}
		else if (piece > 19 && piece < 24){
                        strip.setPixelColor(LEDMap[2][11], MAG); 
                        strip.setPixelColor(LEDMap[2][12], MAG); 
                        strip.setPixelColor(LEDMap[2][13], MAG); 
                        strip.setPixelColor(LEDMap[3][13], MAG); 
                        strip.show();
		}
		else if (piece > 23 && piece < 28){
                        strip.setPixelColor(LEDMap[2][11], GRN); 
                        strip.setPixelColor(LEDMap[2][12], GRN); 
                        strip.setPixelColor(LEDMap[3][12], GRN); 
                        strip.setPixelColor(LEDMap[3][13], GRN); 
                        strip.show();
		}
}

void clearPiecePlus(char piece)
{

			//Yellow squre. Doesn't rotate. Do nothing if < 4
		if (piece == 4 || piece == 6){
			//clearSquare(-48+(actRow*16), 32+(actCol*16)); //Cyan Line
			//clearSquare(-32+(actRow*16), 32+(actCol*16));
			//clearSquare(0+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-3][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.show();

		}
		else if (piece == 5 || piece == 7){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Cyan Line Horiz
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(-16+(actRow*16), 48+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+3], BLK);
                        strip.show();
		}
		else if (piece == 8 || piece == 10){
			//clearSquare(0+(actRow*16), 0+(actCol*16)); //Red Zig
			//clearSquare(0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 9 || piece == 11){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Red Zig turned
			//clearSquare(0+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 12){
			//clearSquare(-16+(actRow*16), 32+(actCol*16)); //Blue Triangle
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 13){
			//clearSquare(0+(actRow*16), 16+(actCol*16)); //Blue Triangle
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 14){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Blue Triangle
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.show();
			}
		else if (piece == 15){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Blue Triangle
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 16){
			//clearSquare(0+(actRow*16), 0+(actCol*16)); //Orange L
			//clearSquare(-16+(actRow*16), 0+(actCol*16));
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 17){
			//clearSquare(-32+(actRow*16), 0+(actCol*16)); //Orange L
			//clearSquare(-32+(actRow*16), 16+(actCol*16));
			//clearSquare(-0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 18){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Orange L
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
			//clearSquare(-32+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 19){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Orange L
			//clearSquare(0+(actRow*16), 32+(actCol*16));
			//clearSquare(0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 20){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Magenta J
			//clearSquare(0+(actRow*16), 32+(actCol*16));
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 21){
			//clearSquare(0+(actRow*16), 0+(actCol*16)); //Magenta J
			//clearSquare(-32+(actRow*16), 16+(actCol*16));
			//clearSquare(0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 22){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Magenta J
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
			//clearSquare(-32+(actRow*16), 0+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol], BLK);
                        strip.show();
		}
		else if (piece == 23){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Magenta J
			//clearSquare(-32+(actRow*16), 32+(actCol*16));
			//clearSquare(0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 24 || piece == 26){
			//clearSquare(0+(actRow*16), 32+(actCol*16));  //Green Zig
			//clearSquare(-16+(actRow*16), 0+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.show();
		}
		else if (piece == 25 || piece == 27){
			//clearSquare(-32+(actRow*16), 32+(actCol*16));  //Green Zig
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.show();
			}
}

void clearPieceMinus(char piece) //clear when rotating
{

			//Yellow square. Doesn't rotate. Do nothing if < 4
		if (piece == 4 || piece == 6){
			//clearSquare(-48+(actRow*16), 32+(actCol*16)); //Cyan Line
			//clearSquare(-32+(actRow*16), 32+(actCol*16));
			//clearSquare(0+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-3][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.show();

		}
		else if (piece == 5 || piece == 7){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Cyan Line Horiz
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(-16+(actRow*16), 48+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+3], BLK);
                        strip.show();
		}
		else if (piece == 8 || piece == 10){
			//clearSquare(0+(actRow*16), 0+(actCol*16)); //Red Zig
			//clearSquare(0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 9 || piece == 11){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Red Zig turned
			//clearSquare(0+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 12){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Blue Triangle
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.show();
		}
		else if (piece == 13){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Blue Triangle
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 14){
			//clearSquare(-16+(actRow*16), 32+(actCol*16)); //Blue Triangle
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.show();
			}
		else if (piece == 15){
			//clearSquare(0+(actRow*16), 16+(actCol*16)); //Blue Triangle
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 16){
			//clearSquare(0+(actRow*16), 0+(actCol*16)); //Orange L
			//clearSquare(-16+(actRow*16), 0+(actCol*16));
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 17){
			//clearSquare(-32+(actRow*16), 0+(actCol*16)); //Orange L
			//clearSquare(-32+(actRow*16), 16+(actCol*16));
			//clearSquare(-0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 18){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Orange L
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
			//clearSquare(-32+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 19){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Orange L
			//clearSquare(0+(actRow*16), 32+(actCol*16));
			//clearSquare(0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 20){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Magenta J
			//clearSquare(0+(actRow*16), 32+(actCol*16));
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 21){
			//clearSquare(0+(actRow*16), 0+(actCol*16)); //Magenta J
			//clearSquare(-32+(actRow*16), 16+(actCol*16));
			//clearSquare(0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 22){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Magenta J
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
			//clearSquare(-32+(actRow*16), 0+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol], BLK);
                        strip.show();
		}
		else if (piece == 23){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Magenta J
			//clearSquare(-32+(actRow*16), 32+(actCol*16));
			//clearSquare(0+(actRow*16), 16+(actCol*16));  
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 24 || piece == 26){
			//clearSquare(0+(actRow*16), 32+(actCol*16));  //Green Zig
			//clearSquare(-16+(actRow*16), 0+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.show();
		}
		else if (piece == 25 || piece == 27){
			//clearSquare(-32+(actRow*16), 32+(actCol*16));  //Green Zig
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.show();
			}
}

void clearPieceLeft(char piece) //clear when moving left
{

		if (piece < 4)
		{
			//clearSquare(0+(actRow*16), 32+(actCol*16)); //Yellow square. k
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 4 || piece == 6){
			//clearSquare(-48+(actRow*16), 32+(actCol*16)); //Cyan Line vert
			//clearSquare(-32+(actRow*16), 32+(actCol*16));
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
			//clearSquare(0+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-3][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 5 || piece == 7){
			//clearSquare(-16+(actRow*16), 48+(actCol*16)); //Cyan Line Horiz
                        strip.setPixelColor(LEDMap[actRow-1][actCol+3], BLK);
                        strip.show();
		}
		else if (piece == 8 || piece == 10){
			//clearSquare(-16+(actRow*16), 32+(actCol*16)); //Red Zig
			//clearSquare(0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 9 || piece == 11){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Red Zig turned
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
			//clearSquare(0+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 12){
			//clearSquare(-16+(actRow*16), 32+(actCol*16)); //Blue Triangle
			//clearSquare(0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 13){
			//clearSquare(0+(actRow*16), 16+(actCol*16)); //Blue Triangle
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(-32+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 14){
			//clearSquare(-16+(actRow*16), 32+(actCol*16)); //Blue Triangle
			//clearSquare(-32+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.show();
			}
		else if (piece == 15){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Blue Triangle
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
			//clearSquare(0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 16){
			//clearSquare(0+(actRow*16), 0+(actCol*16)); //Orange L
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 17){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Orange L
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(-0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 18){
			//clearSquare(-32+(actRow*16), 32+(actCol*16)); //Orange L
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.show();

		}
		else if (piece == 19){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Orange L
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(0+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 20){
			//clearSquare(-16+(actRow*16), 32+(actCol*16)); //Magenta J
			//clearSquare(0+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 21){
			//clearSquare(0+(actRow*16), 16+(actCol*16)); //Magenta J
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(-32+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 22){
			//clearSquare(-32+(actRow*16), 0+(actCol*16)); //Magenta J
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 23){
			//clearSquare(-32+(actRow*16), 32+(actCol*16)); //Magenta J
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 24 || piece == 26){
			//clearSquare(-16+(actRow*16), 16+(actCol*16));  //Green Zig
			//clearSquare(0+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 25 || piece == 27){
			//clearSquare(-32+(actRow*16), 32+(actCol*16));  //Green Zig
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
			//clearSquare(0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
			}
}

void clearPieceRight(char piece) //clear when moving left
{

		if (piece < 4)
		{
			//clearSquare(0+(actRow*16), 16+(actCol*16)); //Yellow square. k
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 4 || piece == 6){
			//clearSquare(-48+(actRow*16), 32+(actCol*16)); //Cyan Line vert
			//clearSquare(-32+(actRow*16), 32+(actCol*16));
			//clearSquare(-16+(actRow*16), 32+(actCol*16));
			//clearSquare(0+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-3][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 5 || piece == 7){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Cyan Line Horiz
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.show();
		}
		else if (piece == 8 || piece == 10){
			//clearSquare(-16+(actRow*16), 16+(actCol*16)); //Red Zig
			//clearSquare(0+(actRow*16), 0+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol], BLK);
                        strip.show();
		}
		else if (piece == 9 || piece == 11){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Red Zig turned
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(0+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 12){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Blue Triangle
			//clearSquare(0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 13){
			//clearSquare(0+(actRow*16), 16+(actCol*16)); //Blue Triangle
			//clearSquare(-16+(actRow*16), 0+(actCol*16));
			//clearSquare(-32+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 14){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Blue Triangle
			//clearSquare(-32+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.show();
			}
		else if (piece == 15){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Blue Triangle
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 16){
			//clearSquare(0+(actRow*16), 0+(actCol*16)); //Orange L
			//clearSquare(-16+(actRow*16), 0+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.show();
		}
		else if (piece == 17){
			//clearSquare(-32+(actRow*16), 0+(actCol*16)); //Orange L
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(-0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 18){
			//clearSquare(-32+(actRow*16), 32+(actCol*16)); //Orange L
			//clearSquare(-16+(actRow*16), 0+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.show();
		}
		else if (piece == 19){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Orange L
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 20){
			//clearSquare(-16+(actRow*16), 0+(actCol*16)); //Magenta J
			//clearSquare(0+(actRow*16), 32+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+2], BLK);
                        strip.show();
		}
		else if (piece == 21){
			//clearSquare(0+(actRow*16), 0+(actCol*16)); //Magenta J
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(-32+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 22){
			//clearSquare(-32+(actRow*16), 0+(actCol*16)); //Magenta J
			//clearSquare(-16+(actRow*16), 0+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.show();
		}
		else if (piece == 23){
			//clearSquare(-32+(actRow*16), 16+(actCol*16)); //Magenta J
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 24 || piece == 26){
			//clearSquare(-16+(actRow*16), 0+(actCol*16));  //Green Zig
			//clearSquare(0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-1][actCol], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
		}
		else if (piece == 25 || piece == 27){
			//clearSquare(-32+(actRow*16), 32+(actCol*16));  //Green Zig
			//clearSquare(-16+(actRow*16), 16+(actCol*16));
			//clearSquare(0+(actRow*16), 16+(actCol*16));
                        strip.setPixelColor(LEDMap[actRow-2][actCol+2], BLK);
                        strip.setPixelColor(LEDMap[actRow-1][actCol+1], BLK);
                        strip.setPixelColor(LEDMap[actRow][actCol+1], BLK);
                        strip.show();
			}
}

/*void scorer(int yoyo)
{
    if (yoyo > 1000) yoyo = yoyo - 1000;
    int units = yoyo;
    int tens = 0;
    int hundreds = 0;
    while(units > 100)
    {
    	units = units - 100;
    	hundreds++;
    }
    while(units > 10)
    {
    	units = units - 10;
    	tens++;
    }
	drawNum(hundreds, 160, 184);
	drawNum(tens, 160, 196);
	drawNum(units, 160, 208);
}*/

void drawBG(void)
{
	clearScreen();
	//drawRectRaw(0,160,319,163,BLU); //Blue separator
        strip.setPixelColor(LEDMap[0][10], BLU2);
        strip.setPixelColor(LEDMap[1][10], BLU2);
        strip.setPixelColor(LEDMap[2][10], BLU2);
        strip.setPixelColor(LEDMap[3][10], BLU2);
        strip.setPixelColor(LEDMap[4][10], BLU2);
        strip.setPixelColor(LEDMap[5][10], BLU2);
        strip.setPixelColor(LEDMap[6][10], BLU2);
        strip.setPixelColor(LEDMap[7][10], BLU2);
        strip.setPixelColor(LEDMap[8][10], BLU2);
        strip.setPixelColor(LEDMap[9][10], BLU2);
        strip.setPixelColor(LEDMap[10][10], BLU2);
        strip.setPixelColor(LEDMap[11][10], BLU2);
        strip.setPixelColor(LEDMap[12][10], BLU2);
        strip.setPixelColor(LEDMap[13][10], BLU2);
        strip.setPixelColor(LEDMap[14][10], BLU2);
        strip.setPixelColor(LEDMap[15][10], BLU2);
        
        strip.setPixelColor(LEDMap[0][11], BLU2);
        strip.setPixelColor(LEDMap[0][12], BLU2);
        strip.setPixelColor(LEDMap[0][13], BLU2);
        strip.setPixelColor(LEDMap[0][14], BLU2);
        strip.setPixelColor(LEDMap[0][15], BLU2);
        
        strip.setPixelColor(LEDMap[5][11], BLU2);
        strip.setPixelColor(LEDMap[5][12], BLU2);
        strip.setPixelColor(LEDMap[5][13], BLU2);
        strip.setPixelColor(LEDMap[5][14], BLU2);
        strip.setPixelColor(LEDMap[5][15], BLU2);
        
        strip.setPixelColor(LEDMap[1][15], BLU2);
        strip.setPixelColor(LEDMap[2][15], BLU2);
        strip.setPixelColor(LEDMap[3][15], BLU2);
        strip.setPixelColor(LEDMap[4][15], BLU2);
        strip.show();
	/*printString(); //print NEXT, Score, High
	scorer(score);
	int returned = 0;
	returned = readSRAM(0x0002);
	returned = returned << 8;
	returned = returned + (readSRAM(0x0000));
	higher(returned);
	high = returned;*/
        high = 0;    
}

void clearScreen(void)
{
  for(int i=0; i<256; i++)
      strip.setPixelColor(i, strip.Color(0,0,0));
  strip.show();
}

void tick()
{
	if (!pause)
	{

			if (downCheck(0))					//If there is nothing blocking it
			{
				clearPiece(aPiece+pieceOff);	//Clear current piece
				actRow++; 						//Move it down the screen
				drawPiece(aPiece+pieceOff);		//Draw new piece
			}
			else
			{
				land();							//Write piece blocks to Blocks array to keep track.
				actRow=0;						//Start from beginning
				aPiece = nPiece;
				nPiece=(((random(0,7))*4)); 	//Choose next piece randomly
				pieceOff = 0; 					//Set piece to starting position
				actCol=4;						//Set piece to middle of screen
				drawNext(nPiece);
				drawPiece(aPiece);
			}
	}
}
void controller()
{
  if (digitalRead(ButtonA))
  {
    if (pause && !gameover)
     pause = 0;
    else pause = 1;
  }
  if (digitalRead(ButtonB))
  {
    if (!pause)
    {
      if (downCheck(1))
      {
			clearPiecePlus(aPiece+pieceOff); //turn erase
			pieceOff++;
			if (pieceOff > 3) pieceOff = 0;
			drawPiece(aPiece+pieceOff);
		}
	    }
	    else
	    {
	  	clearBoard();
		score = 0;
                delaytime = 100000;
		drawBG();
		drawNext(nPiece);
		drawPiece(aPiece);
		pause = 0;
		gameover = 0;
	    }
  }
  if (digitalRead(ButtonDown)) 
  {
    moveIt('d');
  }
  if (digitalRead(ButtonLeft)) 
  {
    moveIt('l');
  }
  if (digitalRead(ButtonRight)) 
  {
    moveIt('r');
  }
  
  if( Serial.available() > 0 ) // if data is available to read
  {
    val = Serial.read();         // read it and store it in 'val'
    Serial.println(val);
  }
  else val = ' ';
  switch (val){
  case 'p': 
          if (pause && !gameover)
	    pause = 0;
	  else pause = 1;
          break;
  case 's': 
          if (!pause)
	   {
		if (downCheck(1))
		{
			clearPiecePlus(aPiece+pieceOff); //turn erase
			pieceOff++;
			if (pieceOff > 3) pieceOff = 0;
			drawPiece(aPiece+pieceOff);
		}
	    }
	    else
	    {
	  	clearBoard();
		score = 0;
                delaytime = 100000;
		drawBG();
		drawNext(nPiece);
		drawPiece(aPiece);
		pause = 0;
		gameover = 0;
	    }
            break;
  case 'w': 
          if (!pause)
		{
			if (downCheck(2))
			{
				clearPieceMinus(aPiece+pieceOff); //turn erase
				if (pieceOff == 0) pieceOff = 3;
				else pieceOff--;
				drawPiece(aPiece+pieceOff);
			}
		}
          break;
  case 'd': 
          moveIt('d');
          break;
  case 'l': 
          moveIt('l');
          break;
  case 'r': 
          moveIt('r');
          break;
  default: 
          break;
  }
}




void firework()
{
  int billy = random(1,7);
  int randx = random(0,16);
  int randy = random(0,16);
  int randr = random(0,5);
  int randg = random(0,5);
  int randb = random(0,5);
  if(billy == 1){
    randr = 0;
    strip.setPixelColor(LEDMap[randx][randy], strip.Color(0,randg,randb));
    strip.show();}
  else if (billy == 2){
    randg = 0;
    strip.setPixelColor(LEDMap[randx][randy], strip.Color(randr,0,randb));
    strip.show();}
  else if (billy == 3){
    randb = 0;
    strip.setPixelColor(LEDMap[randx][randy], strip.Color(randr,randg,0));
    strip.show();}
  else if (billy == 4){
    randr = 0;
    randg = 0;
    strip.setPixelColor(LEDMap[randx][randy], strip.Color(0,0,randb));
    strip.show();}
   else if (billy == 5){
    randg = 0;
    randb = 0;
    strip.setPixelColor(LEDMap[randx][randy], strip.Color(randr,0,0));
    strip.show();}
   else if (billy == 6){
    randr = 0;
    randb = 0;
    strip.setPixelColor(LEDMap[randx][randy], strip.Color(0,randg,0));
    strip.show();}
  else{
    strip.setPixelColor(LEDMap[randx][randy], strip.Color(randr,randg,randb));
    strip.show();}
    delay(10);
    strip.setPixelColor(LEDMap[randx][randy], strip.Color(0,0,0));
    if(randx-1 >= 0)
      strip.setPixelColor(LEDMap[randx-1][randy], strip.Color(randr,randg,randb));
    if(randx+1 < 16)
      strip.setPixelColor(LEDMap[randx+1][randy], strip.Color(randr,randg,randb));
    if(randy-1 >= 0)
      strip.setPixelColor(LEDMap[randx][randy-1], strip.Color(randr,randg,randb)); 
    if(randy+1 < 16)
      strip.setPixelColor(LEDMap[randx][randy+1], strip.Color(randr,randg,randb)); 
    strip.show(); 
    delay(30);
    
    if(randx-1 >= 0)
      strip.setPixelColor(LEDMap[randx-1][randy], strip.Color(0,0,0));
    if(randx+1 < 16)
      strip.setPixelColor(LEDMap[randx+1][randy], strip.Color(0,0,0));
    if(randy-1 >= 0)
      strip.setPixelColor(LEDMap[randx][randy-1], strip.Color(0,0,0)); 
    if(randy+1 < 16)
      strip.setPixelColor(LEDMap[randx][randy+1], strip.Color(0,0,0)); 
    
    if(randx-2 >= 0)
      strip.setPixelColor(LEDMap[randx-2][randy], strip.Color(randr,randg,randb));
    if(randx+2 < 16)
      strip.setPixelColor(LEDMap[randx+2][randy], strip.Color(randr,randg,randb));
    if(randy-2 >= 0)
      strip.setPixelColor(LEDMap[randx][randy-2], strip.Color(randr,randg,randb)); 
    if(randy+2 < 16)
      strip.setPixelColor(LEDMap[randx][randy+2], strip.Color(randr,randg,randb)); 
    if(randx-1 >= 0 && randy-1 >= 0)
      strip.setPixelColor(LEDMap[randx-1][randy-1], strip.Color(randr,randg,randb));
    if(randx+1 < 16 && randy+1 < 16)
      strip.setPixelColor(LEDMap[randx+1][randy+1], strip.Color(randr,randg,randb));
    if(randy-1 >= 0 && randx+1 < 16)
      strip.setPixelColor(LEDMap[randx+1][randy-1], strip.Color(randr,randg,randb)); 
    if(randy+1 < 16 && randx-1 >= 0)
      strip.setPixelColor(LEDMap[randx-1][randy+1], strip.Color(randr,randg,randb));
    strip.show(); 
    delay(30);
    
    if(randx-2 >= 0)
      strip.setPixelColor(LEDMap[randx-2][randy], strip.Color(0,0,0));
    if(randx+2 < 16)
      strip.setPixelColor(LEDMap[randx+2][randy], strip.Color(0,0,0));
    if(randy-2 >= 0)
      strip.setPixelColor(LEDMap[randx][randy-2], strip.Color(0,0,0)); 
    if(randy+2 < 16)
      strip.setPixelColor(LEDMap[randx][randy+2], strip.Color(0,0,0)); 
    if(randx-1 >= 0 && randy-1 >= 0)
      strip.setPixelColor(LEDMap[randx-1][randy-1], strip.Color(0,0,0));
    if(randx+1 < 16 && randy+1 < 16)
      strip.setPixelColor(LEDMap[randx+1][randy+1], strip.Color(0,0,0));
    if(randy-1 >= 0 && randx+1 < 16)
      strip.setPixelColor(LEDMap[randx+1][randy-1], strip.Color(0,0,0)); 
    if(randy+1 < 16 && randx-1 >= 0)
      strip.setPixelColor(LEDMap[randx-1][randy+1], strip.Color(0,0,0));
    strip.show();

    if(randx-1 >= 0 && randy-2 >= 0)
      strip.setPixelColor(LEDMap[randx-1][randy-2], strip.Color(randr,randg,randb));
    if(randx+2 < 16 && randy+1 < 16)
      strip.setPixelColor(LEDMap[randx+2][randy+1], strip.Color(randr,randg,randb));
    if(randx+2 < 16 && randy-1 >= 0)
      strip.setPixelColor(LEDMap[randx+2][randy-1], strip.Color(randr,randg,randb)); 
    if(randx-2 >= 0 && randy+1 < 16)
      strip.setPixelColor(LEDMap[randx-2][randy+1], strip.Color(randr,randg,randb));
    if(randx-1 >= 0 && randy+2 < 16)
      strip.setPixelColor(LEDMap[randx-1][randy+2], strip.Color(randr,randg,randb));
    if(randx+1 < 16 && randy+2 < 16)
      strip.setPixelColor(LEDMap[randx+1][randy+2], strip.Color(randr,randg,randb));
    if(randx-2 >= 0 && randy-1 >= 0)
      strip.setPixelColor(LEDMap[randx-2][randy-1], strip.Color(randr,randg,randb));
    if(randx+1 < 16 && randy-2 >= 0)
      strip.setPixelColor(LEDMap[randx+1][randy-2], strip.Color(randr,randg,randb));
    strip.show(); 
    delay(30);
    
     if(randx-3 >= 0)
      strip.setPixelColor(LEDMap[randx-3][randy], strip.Color(0,0,0));
    if(randx+3 < 16)
      strip.setPixelColor(LEDMap[randx+3][randy], strip.Color(0,0,0));
    if(randy-3 >= 0)
      strip.setPixelColor(LEDMap[randx][randy-3], strip.Color(0,0,0)); 
    if(randy+3 < 16)
      strip.setPixelColor(LEDMap[randx][randy+3], strip.Color(0,0,0)); 
    if(randx-1 >= 0 && randy-2 >= 0)
      strip.setPixelColor(LEDMap[randx-1][randy-2], strip.Color(0,0,0));
    if(randx+2 < 16 && randy+1 < 16)
      strip.setPixelColor(LEDMap[randx+2][randy+1], strip.Color(0,0,0));
    if(randx+2 < 16 && randy-1 >= 0)
      strip.setPixelColor(LEDMap[randx+2][randy-1], strip.Color(0,0,0)); 
    if(randx-2 >= 0 && randy+1 < 16)
      strip.setPixelColor(LEDMap[randx-2][randy+1], strip.Color(0,0,0));
    if(randx-1 >= 0 && randy+2 < 16)
      strip.setPixelColor(LEDMap[randx-1][randy+2], strip.Color(0,0,0));
    if(randx+1 < 16 && randy+2 < 16)
      strip.setPixelColor(LEDMap[randx+1][randy+2], strip.Color(0,0,0));
    if(randx-2 >= 0 && randy-1 >= 0)
      strip.setPixelColor(LEDMap[randx-2][randy-1], strip.Color(0,0,0));
    if(randx+1 < 16 && randy-2 >= 0)
      strip.setPixelColor(LEDMap[randx+1][randy-2], strip.Color(0,0,0));
    strip.show();   
}

