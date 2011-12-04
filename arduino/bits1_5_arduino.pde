
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

const int chiave = 2;
const int cicalino = 3;
const int rele = 4;

void setup() {
  Serial.begin(9600);
  pinMode(chiave,INPUT);
  pinMode(cicalino,OUTPUT);
  pinMode(rele,OUTPUT);
  digitalWrite(rele, LOW); // Ci assicuriamo che il rele non si accenda a caso
  lcd.begin(16, 2);
  lcd.print("BITS Loading...");
}
void getstring(char *s,int n){
  for (int i=0;i<n;i++)
  {
    while(Serial.available()==0) ;
    char c = Serial.read();
    
    if(c=='\n' || c=='\r')
    {
      s[i]='\0';
      break;
    }
    else 
      s[i] = c;
  }
}

void loop() {

    char line[32];
    getstring(line,32);
    if(!strcmp(line,"poul key"))
    {
      if(digitalRead(chiave))
       Serial.println("1");
      else
       Serial.println("0"); 
    }
    else if(!strcmp(line,"poul sound"))
    {
      tone(cicalino,2048,200);
    }
    else if(!strcmp(line,"poul version"))
    {
      Serial.println("v1.50");
    }
    else if(!strcmp(line,"poul temp"))
    {
      Serial.println(analogRead(0));
    }
    else if(!strcmp(line,"poul door open"))
    {
      digitalWrite(rele, HIGH);
    }
    else if(!strcmp(line,"poul door close"))
    {
      digitalWrite(rele, LOW);
    }
    else if(strstr(line,"poul lcd1")!=NULL)
    {
      lcd.setCursor(0, 1);
      lcd.print(line+9);
    }
    else if(strstr(line,"poul lcd0")!=NULL)
    {
      lcd.setCursor(0, 0);
      lcd.print(line+9);
    }
    else if(strstr(line,"poul sounder")!=NULL)
    {
      tone(cicalino,2048,atoi(line+12));
    }
    
    
    
  
    //lcd.setCursor(0, 1);
}

