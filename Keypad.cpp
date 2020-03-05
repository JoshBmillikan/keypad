#include <Arduino.h>
#include <LiquidCrystal.h>

const int yAxis[] = {2,3,4,5};
const int xAxis[] = {6,7,8,9};
inline void SendSerial(const int& x, const int& y);
bool open = false;
class Keypad {
    const unsigned char* PASSWORD;
    unsigned char* input;
    unsigned int index = 0;
public:
    const unsigned int length;
    Keypad(char password[],int length) : PASSWORD(reinterpret_cast<const unsigned char *>(password)), length(length) {
        auto* tmp = new unsigned char[length];
        input = tmp;
        reset();
    }
    void operator += (const char& in) {
        input[index] = in;
        index++;
        if(index >= length) {
            if(memcmp(PASSWORD,input,sizeof(PASSWORD)) == 0) {
                open = true;
            }
            reset();
        }
    }
    inline void reset() {
        for(int i=0;i<length;i++)
            input[i] = '_';
        index = 0;
    }
    [[nodiscard]] unsigned char* getCurrentChars() {
        return input;
    }
};

Keypad keypad("1234",4);
LiquidCrystal lcd(A5,A4,A3,A2,A1,A0);

void setup() {
    pinMode(A5,OUTPUT);
    pinMode(A4,OUTPUT);
    pinMode(A3,OUTPUT);
    pinMode(A2,OUTPUT);
    pinMode(A1,OUTPUT);
    pinMode(A0,OUTPUT);
    for(const int& val : xAxis) {
        pinMode(val, OUTPUT);
        digitalWrite(val,HIGH);
    }
    for(const int& val : yAxis)
        pinMode(val,INPUT_PULLUP);
    Serial.begin(9600);
    lcd.begin(16,2);
}

void loop() {
    for(const int& x : xAxis) {
        digitalWrite(x,LOW);
        for(const int& y : yAxis) {
            if(digitalRead(y) == LOW)
                SendSerial(x,y);
            delay(10);
        }
        digitalWrite(x,HIGH);
    }
    lcd.clear();
    lcd.setCursor(0,0);
    if(open)
        lcd.print("Open!");
    else {
        auto c = keypad.getCurrentChars();
        for (int i=0;i<keypad.length;i++)
            lcd.write(c[i]);
    }
}
inline char getChar(const int& x, const char && a, const char && b, const char && c, const char && d) {
    switch(x) {
        case 6 : return a;
        case 7 : return b;
        case 8 : return c;
        default: return d;
    }
}

inline void SendSerial(const int& x,const int& y) {
    char out = 0;
    switch(y) {
        case 2 : out = getChar(x,'1','2','3','A'); break;
        case 3 : out = getChar(x,'4','5','6','B'); break;
        case 4 : out = getChar(x,'7','8','9','C'); break;
        default: out = getChar(x,'*','0','#','D'); break;
    }
    Serial.write(out);
    Serial.print("\n");
    keypad += out;
}