#include <Arduboy2.h>
#include <avr/wdt.h>
#include <Wire.h>
#include <math.h>

constexpr int8_t TARGET_ADDRESS = 0x10;
const int PLAYER_INIT_BALLX=2700;
const int ENEMY_INIT_BALLX=9700;
const int INIT_BALLY=3200;
const int PLAYER0_INITX=2700;
const int PLAYER1_INITX=9700;
const int PLAYER2_INITX=2700;
const int PLAYER3_INITX=9700;
const int PLAYER_INITY=3200;
const unsigned char title_1[] PROGMEM = {
 0xff, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
 0xff, 0x00, 0x00, 0xff, 0x01, 0xbd, 0x25, 0xa5,
 0xa5, 0xa5, 0x99, 0x42, 0x3c, 0x00, 0x00, 0x00,
 0xff, 0x01, 0xbd, 0xa5, 0xa5, 0xa5, 0xbd, 0x01,
 0xff, 0x00, 0x00, 0xff, 0x01, 0xff, 0x00, 0x00,
 0xff, 0x00, 0x00, 0xff, 0x80, 0xff, 0x04, 0x09,
 0x12, 0x24, 0x48, 0x90, 0xa0, 0xc0, 0x00, 0x00,
 0xff, 0x80, 0xff, 0x00, 0x00, 0x00, 0xff, 0x80,
 0xff, 0x00, 0x00, 0xff, 0x80, 0xbf, 0xa0, 0xa0,
 0xff, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
};
const unsigned char title_2[] PROGMEM = {
 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0xff,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x01,
 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x06, 0x09, 0x32, 0xc4, 0x18, 0x60, 0x18,
 0xc4, 0x32, 0x09, 0x06, 0x00, 0x00, 0x00, 0xff,
 0xa0, 0xa0, 0xa0, 0xe0, 0x00, 0x00, 0xff, 0x80,
 0xbf, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xe0, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x80, 0xff,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xff,
};

enum Scene {
  TITLE,
  WAITING,
  GAME,
  RESULT,
};

Scene scene;

enum Characters {
  PLAYER,
  ENEMY,
};

Characters winflag;
byte selectY[2]={0,0};
byte selected[2]={0,0};
bool playMode=0;
bool gameMode=0;
int count=0;
int serveCount=0;
int serveTime=0;

enum Role : uint8_t {
  CONTROLLER,
  TARGET,
  NONE
};
Role role = NONE;
#define rad(a) (a * PI / 180)
Arduboy2 arduboy;
BeepPin1 beep;
byte totalscore;
byte menu_scene;
struct Vec {
  float x;
  float y;

};



struct Player {
  bool hit;
  float x;
  float y;
  int fx;
  int fy;
  int homeX;
  int homeY;
  float r;
  float l;
  float x0;
  float y0;
  float x1;
  float y1;
  int cnt = 0;
  int score = 0;
  bool sec1, sec2, sec3, sec4;
  bool attack;
  bool serve;
  int interval;
};

struct Ball {
  bool active;
  int turn;
  float x;
  float y;
  int fx;
  int fy;
  float v=70;
  float r;
  float dx;
  float dy;
  int cnt;
  bool flag;
  void Vec(float _x, float _y) {
    fx = _x;
    fy = _y;
  }
  void move(float _dx, float _dy) {
    fx += _dx;
    fy += _dy;
  }
};

Player player[4];
Ball ball;

void Collide() {
if(winflag==ENEMY) player[1].hit=0;player[2].hit=0;
if(winflag==PLAYER) player[0].hit=0;player[3].hit=0;
if(player[0].hit==0 && player[3].hit==0) {
  if (player[0].sec1 || player[0].sec2 || player[0].sec3 || player[0].sec4) {
    beep.tone(beep.freq(600), 1);
    ball.cnt = 90 - player[0].cnt;
    ball.v+=5;
    player[0].hit=1;
  }
    if (player[3].sec1 || player[3].sec2 || player[3].sec3 || player[3].sec4) {
    beep.tone(beep.freq(600), 1);
    ball.cnt = 90 - player[3].cnt;
    ball.v+=5;
    player[3].hit=1;
  }
}
  if ((player[0].sec1 || player[0].sec2 || player[3].sec1 || player[3].sec2 ||ball.turn == 1)) {
    ball.turn = 1;
    ball.move(-ball.v * cos(rad(ball.cnt)), ball.v * sin(rad(ball.cnt)));
  } else if ((player[0].sec3 || player[0].sec4 || player[3].sec3 || player[3].sec4 || ball.turn == 2)) {
    ball.turn = 2;
    ball.move(ball.v * cos(rad(ball.cnt)), -ball.v * sin(rad(ball.cnt)));
  }


  if(player[1].serve==0 ||player[2].serve==0 || (serveCount==0 || serveCount>250)) {
if(player[1].hit==0 && player[2].hit==0) {
    if (player[1].sec1 || player[1].sec2 || player[1].sec3 || player[1].sec4) {
    beep.tone(beep.freq(600), 1);
    ball.cnt = 90 - player[1].cnt;
    ball.v+=5;
    player[1].hit=1;
    }
    if (player[2].sec1 || player[2].sec2 || player[2].sec3 || player[2].sec4) {
    beep.tone(beep.freq(600), 1);
    ball.cnt = 90 - player[2].cnt;
    ball.v+=5;
    player[1].hit=1;
    }
}
  if ((player[1].sec3 || player[1].sec4 || player[2].sec3 || player[2].sec4 || ball.turn == 3)) {
    ball.turn = 3;
    ball.move(-ball.v * cos(rad(ball.cnt)), -ball.v * sin(rad(ball.cnt)));
  } else if ((player[1].sec1 || player[1].sec2 || player[2].sec1 || player[2].sec2 || ball.turn == 4)) {
    ball.turn = 4;
    ball.move(ball.v * cos(rad(ball.cnt)), ball.v * sin(rad(ball.cnt)));
  } else {
    ball.move(0, 0);
  }
  }
}


void Scoring() {
      if ((ball.x < 64 && ball.x > 0) && (ball.y < 54 && ball.y > 10)) {
    winflag = ENEMY;
  } else if ((ball.x < 128 && ball.x > 64) && (ball.y < 54 && ball.y > 10)) {
    winflag = PLAYER;
  }



  if ((ball.x < 128 && ball.x > 64) && (ball.y < 0 || ball.y > 64) || ball.x > 128) {
    ball.cnt=0;
    if (winflag == PLAYER) {
      player[0].score += 1;
    } else {
      player[1].score += 1;
      player[1].attack=1;
      player[2].attack=1;
    }   
     winflag = ENEMY;
    
  switch(totalscore) {
      case 0:
      case 3:
      case 4:
      case 7:
      case 8:
      case 11:
      case 12:
      case 15:
      case 16:
      case 19:
      case 20:
      player[3].serve=1;
    ball.Vec(2700, 3200);  

      break;
      default:
    
    player[1].serve=1;
        player[2].serve=1;
    serveTime=random(270,311);
      ball.Vec(9700, 3200);

        
    }   
    player[0].hit=0;
    player[1].hit=0;
      ball.turn=0;
      ball.v=70;
    ball.active = 1;
  } else if ((ball.x < 64 && ball.x > 0) && (ball.y < 0 || ball.y > 64) || ball.x < 0) {

    if (winflag == ENEMY) {
      player[1].score += 1;
    } else {
      player[0].score += 1;
    }
    winflag = ENEMY;

  switch(totalscore) {
      case 0:
      case 3:
      case 4:
      case 7:
      case 8:
      case 11:
      case 12:
      case 15:
      case 16:
      case 19:
      case 20:
      player[3].serve=1;
    ball.Vec(2700, 3200);  
  
      break;
      default:
    player[1].serve=1;
        player[2].serve=1;
    serveTime=random(270,311);
      ball.Vec(9700, 3200);   
    }   
        player[0].hit=0;
    player[1].hit=0;
  ball.turn=0;
  ball.v=70;
    ball.active = 1;
  }    

}
Role ball_start_side = CONTROLLER;

// must be volatile because it is changed in the callback
volatile bool handshake_completed = false;

// target callbacks
void data_receive(int bytes) {  // callback for when controller sends a message
  while (Wire.available()) {
    // update globals
    player[CONTROLLER].fx = (Wire.read() << 8) | Wire.read();
    player[CONTROLLER].fy = (Wire.read() << 8) | Wire.read();
    player[CONTROLLER].cnt =(Wire.read() << 8) | Wire.read();
ball.fx = (Wire.read() << 8) | Wire.read();
ball.fy = (Wire.read() << 8) | Wire.read();
    player[CONTROLLER].score = Wire.read();
    player[TARGET].score = Wire.read();

  }
}
void data_request() {  // callback for when controller requests a message
    Wire.write(player[role].fx>>8);
    Wire.write(player[role].fx & 0xFF);
    Wire.write(player[role].fy>>8);
    Wire.write(player[role].fy & 0xFF);
            Wire.write(player[role].cnt >> 8);
Wire.write(player[role].cnt & 0xFF);

}

void handshake_request() {
  Wire.write(true);
  handshake_completed = true;
}

void setup() {
  // put your setup code here, to run once:
  arduboy.begin();
    arduboy.audio.on();
  power_twi_enable();
    arduboy.clear();
  beep.begin();
  Serial.begin(9600);
  arduboy.clear();
  arduboy.setFrameRate(30);
  player[0].fx=150;
  player[0].fy=320;
  player[0].r = 3;
  player[0].l = 5;
  player[1].x=108;
  player[1].y=32;
  player[1].homeX=108;
  player[1].homeY=32;
  player[1].r = 3;
  player[1].l = 5;
  player[1].interval=30;
  player[2].x=120;
  player[2].y=32;  
  player[2].r = 3;
  player[2].l = 5;
  player[2].interval=30;
  player[3].x=30;
  player[3].y=32;
  player[3].r = 3;
  player[3].l = 5;
  player[3].serve=1;
  player[3].interval=30;
  ball.Vec(2700,3200); 

  ball.active = 1;
  ball.r = 1;
  scene=TITLE;
}


void GameScene() {

player[1].homeX=108-0.5*(56-player[0].x)-2*(player[1].score-player[0].score);
Collide(); 



Scoring();

    ball.x=ball.fx/100;
    ball.y=ball.fy/100;

if(playMode==0) {
if(ball.x==97 && ball.y==32) {serveCount+=1;}else{serveCount=0;}

  if(arduboy.pressed(A_BUTTON)) {
          if (player[0].y < ball.y && player[0].cnt < 136) {
        player[0].cnt += 2;
      } else if (player[0].y > ball.y && player[0].cnt > -136) {
        player[0].cnt -= 2;
      }
    } else if (player[0].cnt % 360 != 0) {
      if (player[0].cnt <= 136 && player[0].cnt >= 0) {
        player[0].cnt -= 2;
      } else if (player[0].cnt >= -136 && player[0].cnt <= 0) {
        player[0].cnt += 2;
      }
  }
   if (arduboy.pressed(LEFT_BUTTON)) {
   player[0].x -= 0.5;
 } else if (arduboy.pressed(RIGHT_BUTTON) && player[0].x + player[0].r + player[0].l < 64) {
   player[0].x += 0.5;
 } 

  if (arduboy.pressed(UP_BUTTON)) {
   player[0].y -= 0.5;
 } else if (arduboy.pressed(DOWN_BUTTON)) {
   player[0].y += 0.5;
 }

if(serveCount==0) {
 if(!(winflag==ENEMY &&(ball.y<10 ||ball.y>54))) {
if(player[1].y>=ball.y+5) {
    player[1].y-=0.5;
  }else if(player[1].y<=ball.y-5){
    player[1].y+=0.5;
  }else if(player[1].x-ball.x<10&&player[1].y<=ball.y+8&&player[1].cnt<0) {
    player[1].y+=0.5;
  }else if(player[1].x-ball.x<10&&player[1].y>=ball.y-8&&player[1].cnt>0) {
    player[1].y-=0.5;
  } 
 }

if(ball.turn==1 || ball.turn==2) {
    if(player[1].x>ball.x+20 && player[1].x-player[1].r-player[1].l>64 &&
   ((ball.cnt>45 && ball.cnt<90)||(ball.cnt>225 && ball.cnt<270)||(ball.cnt>-90 && ball.cnt<-45)||(ball.cnt>90 && ball.cnt<135))) {
    player[1].x-=0.5;
  }
  
}else if(player[1].x>player[1].homeX && player[1].x-player[1].r-player[1].l>64) {
    player[1].x-=0.5;
  }else {
    if(player[1].x<player[1].homeX && player[1].x+player[1].r<128) {
    player[1].x+=0.5;
  }  
}
  
  }

  if(ball.x==97 && ball.y==32) {
  if(gameMode==0) {
  if(player[1].x<97+8) {
    player[1].x+=0.5;
  }else if(player[1].x>97+8) {
    player[1].x-=0.5;
  }
  }

  if(player[1].y<32+5) {
    player[1].y+=0.5;
  }else if(player[1].y>32+5) {
    player[1].y-=0.5;
  }
  }

  
if(!(ball.x==97 && ball.y==32) || gameMode==1) {
  if(ball.x<64) {player[1].serve=0;}
if(player[1].x-ball.x<player[1].interval&&player[1].x-ball.x>5  && player[1].serve==0) {
    player[1].attack=1;
    if(player[1].y<ball.y && player[1].cnt<136 && player[1].attack==1) {
      player[1].cnt+=2;
    }else if(player[1].y>ball.y && player[1].cnt>-136&& player[1].attack==1){
      player[1].cnt-=2;
    }
  }else if(player[1].cnt%360!=0){
    if(player[1].cnt<=136 && player[1].cnt>=0 ) {
      player[1].cnt-=2;
    }else if(player[1].cnt>=-136 && player[1].cnt<=0) {
      player[1].cnt+=2;
    }
  }else {
    player[1].attack=0;
  }
}else  {
  if(serveCount<150) {
if(player[1].cnt%360!=0){
    if(player[1].cnt<=136 && player[1].cnt>=0 ) {
      player[1].cnt-=2;
    }else if(player[1].cnt>=-136 && player[1].cnt<=0) {
      player[1].cnt+=2;
    }
}
  }else if(serveCount>150 && serveCount<250) {
    if(player[1].cnt>-136) {
      player[1].cnt-=2;

    }
  }else if(serveCount>300) {

    player[1].x-=1.0;
  }

      if(serveCount>serveTime) {
      player[1].cnt+=2;
    }
  }
}else if(playMode==1) {
   if (role == CONTROLLER) {
    Wire.requestFrom(TARGET_ADDRESS, 6);
    while (Wire.available()) {
          player[TARGET].fx =(Wire.read() << 8) | Wire.read();
              player[TARGET].fy =(Wire.read() << 8) | Wire.read();
    player[TARGET].cnt =(Wire.read() << 8) | Wire.read();

    }
        Wire.beginTransmission(TARGET_ADDRESS);
        Wire.write(player[CONTROLLER].fx >> 8);
                Wire.write(player[CONTROLLER].fx & 0xFF);
        Wire.write(player[CONTROLLER].fy >> 8);
        Wire.write(player[CONTROLLER].fy & 0xFF);
        Wire.write(player[CONTROLLER].cnt >> 8);
Wire.write(player[CONTROLLER].cnt & 0xFF);
Wire.write(ball.fx >> 8);
Wire.write(ball.fx & 0xFF);
Wire.write(ball.fy >> 8);
Wire.write(ball.fy & 0xFF);
        Wire.write(player[CONTROLLER].score);
        Wire.write(player[TARGET].score);     
        Wire.endTransmission();
  } 


 if (arduboy.pressed(UP_BUTTON)) {
   player[role].fy -= 5;
 } else if (arduboy.pressed(DOWN_BUTTON)) {
   player[role].fy += 5;
 }

player[CONTROLLER].x=player[CONTROLLER].fx/10;
player[CONTROLLER].y=player[CONTROLLER].fy/10;
     player[TARGET].x=player[TARGET].fx/10;
player[TARGET].y=player[TARGET].fy/10;
  if (role == CONTROLLER) {
  // player[0]操作
  if(arduboy.pressed(A_BUTTON)) {
          if (player[0].y < ball.y && player[0].cnt < 136) {
        player[0].cnt += 2;
      } else if (player[0].y > ball.y && player[0].cnt > -136) {
        player[0].cnt -= 2;
      }
    } else if (player[0].cnt % 360 != 0) {
      if (player[0].cnt <= 136 && player[0].cnt >= 0) {
        player[0].cnt -= 2;
      } else if (player[0].cnt >= -136 && player[0].cnt <= 0) {
        player[0].cnt += 2;
      }
  }
   if (arduboy.pressed(LEFT_BUTTON)) {
   player[0].fx -= 5;
 } else if (arduboy.pressed(RIGHT_BUTTON) && player[0].x + player[0].r + player[0].l < 64) {
   player[0].fx += 5;
 } 
} else {
  // player[1]操作
  if(arduboy.pressed(A_BUTTON)) {
      if (player[1].y < ball.y && player[1].cnt < 136) {
        player[1].cnt += 2;
      } else if (player[1].y > ball.y && player[1].cnt > -136) {
        player[1].cnt -= 2;
      }
      } else if (player[1].cnt % 360 != 0) {
      if (player[1].cnt <= 136 && player[1].cnt >= 0) {
        player[1].cnt -= 2;
      } else if (player[1].cnt >= -136 && player[1].cnt <= 0) {
        player[1].cnt += 2;
      }

  }
   if (arduboy.pressed(LEFT_BUTTON) && player[1].x - player[1].r - player[1].l > 64) {
   player[1].fx -= 5;
 } else if (arduboy.pressed(RIGHT_BUTTON)) {
   player[1].fx += 5;
 } 
}

}  

if(gameMode==1) {
  if(serveCount==0) {
 if(!(winflag==ENEMY  &&(ball.y<10 ||ball.y>54))) {
if(player[2].y>ball.y+5) {
    player[2].y-=0.5;
  }else if(player[2].y<ball.y-5){
    player[2].y+=0.5;
  }else if(player[2].x-ball.x<10&&player[2].y<=ball.y+5) {
    player[2].y+=0.5;
  }else if(player[2].x-ball.x<10&&player[2].y>ball.y-5) {
    player[2].y-=0.5;
  } 
 }

  if(playMode==0) {
    if(player[2].x>ball.x+70 && player[2].x-player[2].r-player[2].l>64) {
    player[2].x-=0.5;
  }
  if(player[2].x<player[0].x+60) {
    player[2].x+=0.5;
  }    
  }

  if(playMode==1) {
  if(arduboy.pressed(B_BUTTON) && (ball.turn==3 || ball.turn==4) &&player[2].x-player[2].r-player[2].l>64) {
    player[2].x-=0.5;
  }

  if(arduboy.pressed(B_BUTTON) && (ball.turn==1 || ball.turn==2) ) {
    player[2].x+=0.5;
  }
  }else {
     if(player[2].x<player[0].x+30) {
    player[2].x+=0.5;
  } 



  }

  }

  if(ball.fx==9700 && ball.fy==3200 && playMode==0) {
  if(player[2].x<97+8) {
    player[2].x+=0.5;
  }else if(player[2].x>97+8) {
    player[2].x-=0.5;
  }

  if(player[2].y<32+5) {
    player[2].y+=0.5;
  }else if(player[2].y>32+5) {
    player[2].y-=0.5;
  }
  }

if(!(ball.fx==9700 && ball.fy==3200)) {
  if(ball.x<64) {player[2].serve=0;}
if(player[2].x-ball.x<player[2].interval&&player[2].x-ball.x>5  && player[2].serve==0) {
    player[2].attack=1;
    if(player[2].y<ball.y && player[2].cnt<136 && player[2].attack==1) {
      player[2].cnt+=2;
    }else if(player[2].y>ball.y && player[2].cnt>-136&& player[2].attack==1){
      player[2].cnt-=2;
    }
  }else if(player[2].cnt%360!=0){
    if(player[2].cnt<=136 && player[2].cnt>=0 ) {
      player[2].cnt-=2;
    }else if(player[2].cnt>=-136 && player[2].cnt<=0) {
      player[2].cnt+=2;
    }
  }else {
    player[2].attack=0;
  }
}else  {

  if(serveCount<150) {
if(player[2].cnt%360!=0){
    if(player[2].cnt<=136 && player[2].cnt>=0 ) {
      player[2].cnt-=2;
    }else if(player[2].cnt>=-136 && player[2].cnt<=0) {
      player[2].cnt+=2;
    }
    }  
}else if(serveCount>150 && serveCount<250) {
    if(player[2].cnt>-136) {
      player[2].cnt-=2;

    }
  }else if(serveCount>300) {

    player[2].x-=1.0;
  }

      if(serveCount>serveTime) {
      player[2].cnt+=2;
    }



  }



  
   if(!(winflag==PLAYER  &&(ball.y<10 ||ball.y>54))) {
if(player[3].y>ball.y+5) {
    player[3].y-=0.5;
  }else if(player[3].y<ball.y-5){
    player[3].y+=0.5;
  }else if(player[3].x-ball.x>-5&&player[3].y<=ball.y+5) {
    player[3].y+=0.5;
  }else if(player[3].x-ball.x>-5&&player[3].y>ball.y-5) {
    player[3].y-=0.5;
  } 
 }
  if(arduboy.pressed(B_BUTTON) && (ball.turn==1 ||ball.turn==2 ) && player[3].x+player[3].r+player[3].l<64) {
    player[3].x+=0.5;
  }
  if(arduboy.pressed(B_BUTTON) && (ball.turn==3 ||ball.turn==4 )) {
    player[3].x-=0.5;
  }

  if(ball.x>64) {player[3].serve=0;}
if(ball.x-player[3].x<player[3].interval&&ball.x-player[3].x>5 && player[3].serve==0) {
    player[3].attack=1;
    if(player[3].y<ball.y && player[3].cnt<136 && player[3].attack==1) {
      player[3].cnt+=2;
    }else if(player[3].y>ball.y && player[3].cnt>-136&& player[3].attack==1){
      player[3].cnt-=2;
    }
  }else if(player[3].cnt%360!=0){
    if(player[3].cnt<=136 && player[3].cnt>=0 ) {
      player[3].cnt-=2;
    }else if(player[3].cnt>=-136 && player[3].cnt<=0) {
      player[3].cnt+=2;
    }
  }else {
    player[3].attack=0;
  }

}


  if (ball.active == 1) {

    arduboy.drawCircle(ball.x, ball.y, ball.r, WHITE);
  }

  arduboy.drawRect(10, 10, 108, 44, WHITE);
  arduboy.drawLine(64, 10, 64, 52, WHITE);
  for(byte i=0;i<4;i++) {
    if(i<2 || gameMode==1) {
      arduboy.drawCircle(player[i].x, player[i].y, player[i].r, WHITE);
      arduboy.drawLine(player[i].x0, player[i].y0, player[i].x1, player[i].y1, WHITE);      
    }

      
  }

  arduboy.setCursor(10, 0);
  arduboy.print(player[0].score);
  arduboy.setCursor(112, 0);
  arduboy.print(player[1].score);
  arduboy.display();
}


void WinCheck() {
  arduboy.setCursor(48, 10);
  arduboy.print(player[0].score);
    arduboy.print("-");
  arduboy.print(player[1].score);
  if(playMode==0) {
      if(player[0].score>player[1].score) {
        arduboy.setCursor(40, 32);
  arduboy.print("You Win!!");
    }else {
    arduboy.setCursor(40, 32);
  arduboy.print("You Lose!!");      
    }
  
  }
  else if(playMode==1) {
  if(player[0].score>player[1].score) {
    if(role==CONTROLLER) {
        arduboy.setCursor(40, 32);
  arduboy.print("You Win!!");
    }else {
    arduboy.setCursor(40, 32);
  arduboy.print("You Lose!!");      
    }
  }else {
    if(role==CONTROLLER) {
        arduboy.setCursor(40, 32);
  arduboy.print("You Lose!!");
    }else {
    arduboy.setCursor(40, 32);
  arduboy.print("You Win!!");      
    }
    }    
  }

  if(arduboy.justPressed(A_BUTTON)) {
    wdt_enable(WDTO_15MS);
    while (1) {}
  }
  arduboy.display(); 
}
void loop() {
  // put your main code here, to run repeatedly:
  if (!(arduboy.nextFrame())) return;
  arduboy.clear();
  beep.timer();
  arduboy.pollButtons();
  count+=1;
  if(scene==TITLE) {
  arduboy.drawBitmap(32, 0, title_1, 32, 32, WHITE);
    arduboy.drawBitmap(64, 0, title_2, 32, 32, WHITE);
    arduboy.fillRect(32,42+selectY[0],2,2,WHITE);


       
if(selected[0]==0) {
      if(arduboy.pressed(UP_BUTTON) && selectY[0]>0) {
      selectY[0]-=12;
    }else if(arduboy.pressed(DOWN_BUTTON) && selectY[0]<12) {
      selectY[0]+=12;
    }
}else {
      if(arduboy.pressed(UP_BUTTON) && selectY[1]>0) {
      selectY[1]-=12;
    }else if(arduboy.pressed(DOWN_BUTTON) && selectY[1]<12) {
      selectY[1]+=12;
    }
}


    if(selected[0]==0 || playMode==0) {
    arduboy.setCursor(36, 40);
    arduboy.print("1P");
    }
    if(selected[0]==0 || playMode==1) {
        arduboy.setCursor(36, 52);
    arduboy.print("2P");
    }
    if(selected[0]==1) {
    arduboy.fillRect(50,42+selectY[1],2,2,WHITE);
    arduboy.setCursor(55, 40);    
    arduboy.print("Singles");
    arduboy.setCursor(55, 52);
    arduboy.print("Doubles"); 
    }
  arduboy.display();
      if(arduboy.justPressed(A_BUTTON)) {
        if(selected[0]==1) {
          selected[1]=1;
          gameMode=(selectY[1]==0) ? 0:1;
        }        
        if(selected[0]==0) {
          randomSeed(count);
          selected[0]=1;
          playMode=(selectY[0]==0) ? 0:1;
        }




      }

    if(arduboy.pressed(B_BUTTON) && selected[0]==1) {
      selected[0]=0;
      selectY[1]=0;

  }
if(selected[1]==1) {
    if(playMode==0 && gameMode==0) {
     scene=GAME;
  }else if(playMode==0 && gameMode==1) {
     scene=GAME;
  }else if(playMode==1 && gameMode==0) {
     scene=WAITING;
  }else if(playMode==1 && gameMode==1) {
    scene=WAITING;
  }
arduboy.clear();
}

  }

  if(scene==WAITING) {
      arduboy.print("Waiting for other\nplayer...");
  arduboy.display();
  Wire.begin();
  Wire.requestFrom(TARGET_ADDRESS, 1);
  if (Wire.available()) {  // already a target here
    role = CONTROLLER;
    handshake_completed = Wire.read();
  } else {
    role = TARGET;
    Wire.begin(TARGET_ADDRESS);
    Wire.onRequest(handshake_request);
  }
  while (!handshake_completed) {}

  // setup callbacks
  if (role == TARGET) {
    Wire.onRequest(data_request);
    Wire.onReceive(data_receive);
  }
  scene=GAME;
  }
switch(scene) {
  case GAME:
  GameScene();
  break;
  case RESULT:
  WinCheck();
  break;
}


  for(byte i=0;i<4;i++) {
    if(i==0 || i==3) {

    player[i].sec1 = player[i].x0 <= ball.x + 1 && player[i].x1 >= ball.x - 1 && player[i].y0 > ball.y && player[i].y1 < ball.y;
    player[i].sec2 = player[i].x0 >= ball.x - 1 && player[i].x1 <= ball.x + 1 && player[i].y0 > ball.y && player[i].y1 < ball.y;
    player[i].sec3 = player[i].x0 >= ball.x - 1 && player[i].x1 <= ball.x + 1 && player[i].y0 < ball.y && player[i].y1 > ball.y;
    player[i].sec4 = player[i].x0 <= ball.x + 1 && player[i].x1 >= ball.x - 1 && player[i].y0 < ball.y && player[i].y1 > ball.y;

      player[i].x0 = player[i].x + player[i].r * cos(rad(player[i].cnt));
  player[i].y0 = player[i].y + player[i].r * sin(rad(player[i].cnt));
  player[i].x1 = player[i].x + (player[i].r+ player[i].l)* cos(rad(player[i].cnt));
  player[i].y1 = player[i].y + (player[i].r + player[i].l)* sin(rad(player[i].cnt));
    }else if(i==1 || i==2) {
          player[i].sec1 = player[i].x0 <= ball.x + 1 && player[i].x1 >= ball.x - 1 && player[i].y0 > ball.y && player[i].y1 < ball.y;
    player[i].sec2 = player[i].x0 >= ball.x - 1 && player[i].x1 <= ball.x + 1 && player[i].y0 > ball.y && player[i].y1 < ball.y;
    player[i].sec3 = player[i].x0 >= ball.x - 1 && player[i].x1 <= ball.x + 1 && player[i].y0 < ball.y && player[i].y1 > ball.y;
    player[i].sec4 = player[i].x0 <= ball.x + 1 && player[i].x1 >= ball.x - 1 && player[i].y0 < ball.y && player[i].y1 > ball.y;

      player[i].x0 = player[i].x - player[i].r*cos(rad(player[i].cnt));
  player[i].y0 = player[i].y + player[i].r* sin(rad(player[i].cnt));
  player[i].x1 = player[i].x - (player[i].r + player[i].l)* cos(rad(player[i].cnt));
  player[i].y1 = player[i].y + (player[i].r + player[i].l)* sin(rad(player[i].cnt));
    }

  }



    

totalscore=player[0].score+player[1].score;
if(player[0].score==11 || player[1].score==11) {
  scene=RESULT;
}



 
}
