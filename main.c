#define LATCH1 2
#define LATCH2 3

#define CLOCK1 4
#define CLOCK2 5

#define DATA1 6
#define DATA2 7

#define OE1 8
#define OE2 9

#define BLINK_LIM 20

bool finished = 0;

char pl_spr = 0b10010;
char pl_spr_hand = 0b11010;

char pl_spr_leg = 0b110010;

char en_spr = 0b01000100;

char en_spr_hand = 0b01001100;

char en_spr_leg = 0b01100100;

char buf[4] = {pl_spr, 0, 0, en_spr};

char pl_attack = 0;

#define CLT 0xFF

unsigned int delcnt = 0;
#define MAX_DEL 25

char pl_pos = 0;
char en_pos = 3;
char en_attack = 0;

char pause = 0;

char pl_patt = 0;
#define ATT_CONST 15
char en_patt = 0;

char pl_blink = 0;
char en_blink = 0;

char pl_att_limit = 0;
#define ATT_CONST_CONT 5 
char en_att_limit = 0;

char pl_health  = 10, en_health = 10;

void disp(unsigned char b, bool sel)
{
  if(!sel)
  {
  //digitalWrite(LATCH1, HIGH);
    digitalWrite(OE1, HIGH);
      digitalWrite(LATCH1, LOW);
  // shift out the bits
  shiftOut(DATA1, CLOCK1, MSBFIRST, b);
  //take the latch pin high so the LEDs will light up
  
  digitalWrite(LATCH1, HIGH);

  digitalWrite(OE1, LOW);
    }
    else
  {
  //digitalWrite(LATCH2, HIGH);
    digitalWrite(OE2, HIGH);
      digitalWrite(LATCH2, LOW);
  // shift out the bits
  shiftOut(DATA2, CLOCK2, MSBFIRST, ~b);
  //take the latch pin high so the LEDs will light up
  
  digitalWrite(LATCH2, HIGH);
  digitalWrite(OE2, LOW);
    }
  
}


void setup() {
  // put your setup code here, to run once:

  for(char i = 2; i <= 9; i++)
    pinMode(i, OUTPUT);

  pinMode(10, INPUT_PULLUP);
   pinMode(11, INPUT_PULLUP);

  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);


  pinMode(A5, INPUT);

  randomSeed(analogRead(A5));
/*

 noInterrupts();//stop interrupts
//set timer1 interrupt at 1kHz
TCCR1A = 0;// set entire TCCR1A register to 0
TCCR1B = 0;// same for TCCR1B
TCNT1  = 0xFFFF;//initialize counter value to 0
// set timer count for 1khz increments
//OCR1A = 0xFFFF;// = (16*10^6) / (1000*8) - 1
//had to use 16 bit timer1 for this bc 1999>255, but could switch to timers 0 or 2 with larger prescaler
// turn on CTC mode
//TCCR1B |= (1 << WGM12);
// enable timer compare interrupt
//TIMSK1 |= (1 << OCIE1A);

TIMSK1 |= (1 << TOIE1);
interrupts();
*/

  //DDRD = (0x01 << LED);     //Configure the PORTD4 as output
  TCNT1 = CLT;   // for 1 sec at 16 MHz 
  TCCR1A = 0x00;
  
  TCCR1B = (1<<CS10) /*| (1<<CS12);*/;  // Timer mode with 1024 prescler
  TIMSK1 = (1 << TOIE1) ;   // Enable timer1 overflow interrupt(TOIE1)
sei();      

Serial.begin(9600);
}



void loop() {
  // put your main code here, to run repeatedly:

  if(pause || finished)
    return;


  pause = 1;

  
  if(!digitalRead(10))
  {
    
    if(pl_pos > 0)
    {
      pl_pos--;
       buf[pl_pos] = pl_spr;
      buf[pl_pos + 1] = 0;

    }
   }
  else if(!digitalRead(11))
  {
    
    if(pl_pos < en_pos - 1)
    {
      pl_pos++;
       buf[pl_pos] = pl_spr;
      buf[pl_pos - 1] = 0;

    }
   }


  char en_dec = random(0, 7);

  if(en_dec == 0 && en_pos > pl_pos + 1)
  {
      
        buf[en_pos - 1] = buf[en_pos];
        buf[en_pos] = 0;
        en_pos--;
    }
    else if(en_dec == 1 && en_pos < 3)
  {
      
        buf[en_pos + 1] = buf[en_pos];
        buf[en_pos] = 0;
        en_pos++;
    }
    

  if(pl_patt > 0)
    pl_patt--;


  if(!pl_patt)
  {
  if(!digitalRead(A1))
  {
      pl_attack = 1;
      buf[pl_pos] = pl_spr_hand;
      pl_patt = ATT_CONST;
    }
    else
    {
        pl_attack = 0;
        buf[pl_pos] = pl_spr;

        if(!digitalRead(A2))
        {
pl_patt = ATT_CONST;
            pl_attack = 2;
            buf[pl_pos] = pl_spr_leg;
            pl_patt = ATT_CONST;
          }
      }


  }

  if(pl_attack)
  {
      pl_att_limit++;

      if(pl_att_limit >= ATT_CONST_CONT)
      {
          pl_att_limit = 0;
          pl_attack = 0;
          buf[pl_pos] = pl_spr;
        }
    }


  if(en_patt > 0)
    en_patt--;



  if(!en_patt)
  {
      char ea = random(0, 2);

    en_patt = ATT_CONST;

      if(ea)
      {
          en_attack = 1;
          buf[en_pos] = en_spr_hand;
        }
        else
        {
            en_attack = 2;
            buf[en_pos] = en_spr_leg;
          }
  }

  if(en_attack)
  {
      en_att_limit++;

      if(en_att_limit >= ATT_CONST_CONT)
      {
          en_att_limit = 0;
          en_attack = 0;
          buf[en_pos] = en_spr;
        }
    }

if(pl_blink > 0)
  pl_blink--;

 if(en_blink > 0)
  en_blink--;

  if(en_pos -  pl_pos == 1)
  {
      if(!en_blink)
      {
          if(pl_attack && pl_attack != en_attack)
          {
              en_blink = BLINK_LIM;
              en_health--;
            }
        }

 if(!pl_blink)
      {
          if(en_attack && pl_attack != en_attack)
          {
              pl_blink = BLINK_LIM;
              pl_health--;
            }
        }
        
    }

  if(en_health <= 0)
  {
      finished = true;
    
      buf[0] = 0b1101011;
      buf[1] = 0b1110110;
      buf[2] = 0b110011;
      buf[3] = 0b110011;
    }
    else if(pl_health <= 0)
    {
        finished = true;

        buf[0] = 0b11011;
        buf[1] = 0b1011111;
        buf[2] = 0b10010;
        buf[3] = 0b110010;
      }
}

char cur_buf = 1;

char pl_show = 1;

char pl_show_cnt = 0;

#define SPR_SC 10

char en_show = 1;
char en_show_cnt = 0;

ISR(TIMER1_OVF_vect)
{
  if(++delcnt == MAX_DEL)
  {
    delcnt = 0;
    pause = 0;
  }
  
  
  TCNT1 = CLT;

  if(++pl_show_cnt >= SPR_SC)
  {
    pl_show_cnt = 0;
    pl_show ^= 1;
    }

  if(++en_show_cnt >= SPR_SC)
  {
    en_show_cnt = 0;
    en_show ^= 1;
    }

  

  if(finished || (!pl_blink || cur_buf != pl_pos || pl_show) &&
  (!en_blink || cur_buf != en_pos || en_show))
  {
    disp(1 << cur_buf, false);
    disp(buf[cur_buf], true);
  }
  else
  {

  disp(1 << cur_buf, false);
    disp(0, true);
    
    }
 

    cur_buf = (cur_buf + 1) % 4;
}

