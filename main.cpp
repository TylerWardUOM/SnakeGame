#include "mbed.h"
#include "Joystick.h"
#include "C12832.h"
#include "led.h"

class Fruit{
    private:
        int x;
        int y;
    public:
        Fruit():x(rand()%128),y(rand()%32){}

        void move(){
            x=rand()%128;
            y=rand()%32;
        }

        int get_x(){
            return x;
        }

        int get_y(){
            return y;
        }

        void draw(C12832& lcd) {
            //lcd.re(); // Clear the buffer before drawing

            lcd.pixel(x, y, 1); // Set pixel in buffer

            lcd.copy_to_lcd(); // Transfer the buffer to the actual LCD
        }
};

class Player{
    private:
        int size;
        const int maxsize;
        int cords[300][2];
        int velocity[2];

        void movebody() {
            // Shift all segments
            for (int i = size - 1; i > 0; --i) {
                cords[i][0] = cords[i - 1][0];
                cords[i][1] = cords[i - 1][1];
            }
        }

    public:
        Player():size(1),maxsize(300){
            cords[0][0]=20;
            cords[0][1]=20;
            velocity[0]=0;
            velocity[1]=0;
        };

        int getX(){
            return cords[0][0];
        }

        int getY(){
            return cords[0][1];
        }

        void grow() {
            if (size < maxsize) {
                // Copy the position of the last segment for the new segment
                cords[size][0] = cords[size - 1][0];
                cords[size][1] = cords[size - 1][1];
                size+=5; // Increase the size of the snake
            }
        }

        void setmove(Joystick joystick) {
            if (joystick.upPressed() && velocity[1] != 1) {
                velocity[1] = -1; velocity[0] = 0;
            } else if (joystick.downPressed() && velocity[1] != -1) {
                velocity[1] = 1; velocity[0] = 0;
            } else if (joystick.leftPressed() && velocity[0] != 1) {
                velocity[0] = -1; velocity[1] = 0;
            } else if (joystick.rightPressed() && velocity[0] != -1) {
                velocity[0] = 1; velocity[1] = 0;
            }
        }

        void move(){
            movebody();
            cords[0][0]+=velocity[0];
            cords[0][1]+=velocity[1];
        }

        void draw(C12832& lcd) {
            //lcd.re(); // Clear the buffer before drawing

            // Draw the snake segments in the buffer
            for (int i = 0; i < size; i++) {
                if (cords[i][0] >= 0 && cords[i][0] < 128 && cords[i][1] >= 0 && cords[i][1] < 64) {
                    lcd.pixel(cords[i][0], cords[i][1], 1); // Set pixel in buffer
                }
            }
            
            lcd.copy_to_lcd(); // Transfer the buffer to the actual LCD
        }

        bool check_collision_self(){
            for (int i = 1; i < size; i++) {
                if (cords[0][0] == cords[i][0] && cords[0][1] == cords[i][1]){
                    return true;
                }
            }
            return false;

        }

        bool check_collision_fruit(Fruit fruit){
            if (cords[0][0]==fruit.get_x() && cords[0][1]==fruit.get_y()){
                return true;
            }
            else {
                return false;
            }
        }
    
};

class Game{
    private:
    public:
    Game();
    void main(){

    }


};

int main() {
    srand(time(NULL));
    Joystick joystick(A2,A3,A4,A5,D4);
    Player player;
    C12832 lcd(D11, D13, D12, D7, D10);
    RGBLed led(D5,D9,D8); //for debug 
    led.setOff();
    Fruit fruit;
    int score = 0;
    while(1) {
        lcd.cls();
        player.setmove(joystick);
        player.move();
        player.draw(lcd);
        fruit.draw(lcd);
        lcd.locate(120, 3);
        lcd.printf("%d",score);
        wait(0.1);
        //led.setOff();
        if (joystick.firePressed()){
            player.grow();
            //led.setWhite();
        }
        if (player.check_collision_self()){
            break;
        }
        if (player.check_collision_fruit(fruit)){
            player.grow();
            fruit.move();
            score++;
        }

    };
};