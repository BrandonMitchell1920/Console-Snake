/*
 * Name: Brandon Mitchell
 * Description: This bad boy is a snake game I made in C++.  The apple
 *              spawns randomly.  Run into one to grow!  Started on 24
 *              November, 2019 and finished on 29.
 */

#include <iostream>   // I/O stuff
#include <vector>     // Snake is a vector of SnakeSegment
#include <time.h>     // Time since epoch used as seed
#include <Windows.h>  // Console stuff

unsigned short WIDTH  = 50;
unsigned short HEIGHT = 25;

unsigned short W_INCRE = 35;
unsigned short H_INCRE = 2;

unsigned short WALL_COLOR       = BACKGROUND_RED  | BACKGROUND_INTENSITY;
unsigned short SNAKE_COLOR      = BACKGROUND_BLUE | BACKGROUND_INTENSITY;
unsigned short BACKGROUND_COLOR = BACKGROUND_GREEN;

unsigned short MAX_SNAKE      = 100;
unsigned short INIT_SNAKE_LEN = 5;

static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

// This and the once below were stolen from a StackExchange post, though
// I left hOut out of the functions as it only needs to be defined once
void setCursorPosition(unsigned short x, unsigned short y)
{
    std::cout.flush();
    COORD coord = {x, y};
    SetConsoleCursorPosition(hOut, coord);
}

void setConsoleColor(unsigned short color)
{
    std::cout.flush();
    SetConsoleTextAttribute(hOut, color);
}

// Called once to draw the background
void drawBackground()
{
    char letter;
    for (int x = 0; x < WIDTH; x++)
    {
        for (int y = 0; y < HEIGHT; y++)
        {
            if (x == 0 || y == 0 || x == WIDTH - 1 || y == HEIGHT - 1)
            {
                setConsoleColor(WALL_COLOR);
                setCursorPosition(W_INCRE + x, H_INCRE + y);
                letter = '#';
            }
            else
            {
                setConsoleColor(BACKGROUND_COLOR);
                setCursorPosition(W_INCRE + x, H_INCRE + y);
                letter = ' ';
            }
            std::cout << letter;
        }
        std::cout << std::endl;
    }
}

// Displays a death message to the console
void yourDed()
{
    setConsoleColor(WALL_COLOR);
    setCursorPosition(W_INCRE, HEIGHT + H_INCRE);
    std::cout << "You dead, man ...                                 ";
    Sleep(3000);
}

// Random x coordinate in the playing area
unsigned short getRandX()
{
    return (rand() % (WIDTH  - 2)) + W_INCRE + 1;
}

// Random y coordinate in the playing area
unsigned short getRandY()
{
    return (rand() % (HEIGHT - 2)) + H_INCRE + 1;
}

// The snake is made of these bad boys, and the apple is a child of this
// class, has various useful methods
class SnakeSegment
{
public:
    unsigned short x, y;

    SnakeSegment(unsigned short passedX = 0, unsigned short passedY = 0)
    {
        x = passedX; y = passedY;
    }

    void drawSegment()
    {
        setConsoleColor(SNAKE_COLOR);
        setCursorPosition(x, y);
        std::cout << '?';
    }

    // All segments except head are given the position of the segment in front
    void setSegment(unsigned short newX, unsigned short newY)
    {
        x = newX; y = newY;
    }

    // Method only called for snake's head
    void updateSegment(unsigned short newX, unsigned short newY)
    {
        x += newX; y += newY;
    }

    unsigned short getSegmentX()
    {
        return x;
    }

    unsigned short getSegmentY()
    {
        return y;
    }

    // Returns true if SnakeSegment and another SnakeSegment share coords,
    // used for apple and snake collisions
    bool sameCoords(SnakeSegment other)
    {
        return (getSegmentX() == other.getSegmentX() &&
                getSegmentY() == other.getSegmentY());
    }
};

// Child of SnakeSegment, new constructor and drawSegment method
class Apple: public SnakeSegment
{
public:
    Apple(unsigned short newX, unsigned short newY)
    {
        x = newX; y = newY;
    }

    void drawSegment()
    {
        setConsoleColor(WALL_COLOR);
        setCursorPosition(x, y);
        std::cout << '@';
    }
};

// The snake itself, make of a vector of SnakeSegments, many methods are private
// and are used to keep things clean inside the public methods
class aSlitheryHissingThing
{
private:

    std::vector<SnakeSegment> snake;

    void updateSnakeHead(unsigned short xIncre, unsigned short yIncre)
    {
        snake[snake.size() - 1].updateSegment(xIncre, yIncre);
    }

    unsigned short getSnakeTailX()
    {
        return snake[0].getSegmentX();
    }

    unsigned short getSnakeTailY()
    {
        return snake[0].getSegmentY();
    }

    unsigned short getSnakeHeadX()
    {
        return snake[snake.size() - 1].getSegmentX();
    }

    unsigned short getSnakeHeadY()
    {
        return snake[snake.size() - 1].getSegmentY();
    }

    // Checks that the snake's head is not colliding with its body, skips
    // the tail due to addSegment being called each time whether it is needed
    // or not
    bool isTheSnakeEatingItself()
    {
        for (unsigned short i = 0; i < snake.size() - 1; i++)
        {
            if (snake[i].sameCoords(snake[snake.size() - 1])) {return true;}
        }
        return false;
    }

    // Checks that the snake head is in the play area
    bool notInsidePlayArea()
    {
        return (getSnakeHeadX() < W_INCRE + 1 || getSnakeHeadX() > WIDTH  + W_INCRE - 2 ||
                getSnakeHeadY() < H_INCRE + 1 || getSnakeHeadY() > HEIGHT + H_INCRE - 2);
    }

    // Removes the blue trail the snake would leave otherwise
    void clearTrail()
    {
        setConsoleColor(BACKGROUND_COLOR);
        setCursorPosition(getSnakeTailX(), getSnakeTailY());
        std::cout << ' ';
    }

public:

    aSlitheryHissingThing()
    {
        for (unsigned short i = 0; i < INIT_SNAKE_LEN + 1; i++)
        {
            snake.insert(snake.begin(), SnakeSegment(WIDTH / 2 + W_INCRE - i, HEIGHT / 2 + H_INCRE));
        }
    }

    void addSegment()
    {
        snake.insert(snake.begin(), SnakeSegment(getSnakeTailX(), getSnakeTailY()));
    }

    void drawTheWholeDarnSnake()
    {
        for (unsigned short i = 0; i < snake.size(); i++)
        {
            snake[i].drawSegment();
        }
        clearTrail();
    }

    void updateTheSnake(unsigned short xIncre, unsigned short yIncre)
    {
        for (unsigned short i = 0; i < snake.size(); i++)
        {
            if (i == snake.size() - 1) {updateSnakeHead(xIncre, yIncre);}
            else {snake[i].setSegment(snake[i + 1].getSegmentX(), snake[i + 1].getSegmentY());}
        }
    }

    bool areYouDeadYet()
    {
        return (isTheSnakeEatingItself() || notInsidePlayArea());
    }

    bool ateApple(Apple apple)
    {
        return (apple.sameCoords(snake[snake.size() - 1]));
    }

    // Sets the position of the apple
    Apple setApple()
    {
        Apple apple = Apple(getRandX(), getRandY());

        unsigned short snakeCount = 0;

        while (true)
        {
            for (unsigned short i = 0; i < snake.size(); i++)
            {
                if (!apple.sameCoords(snake[i])) {snakeCount++;}
            }
            if (snakeCount == snake.size()) {break;}
            else {apple = Apple(getRandX(), getRandY());}
        }
        return apple;
    }

    // What does this do?
    void drawSnakeHead()
    {
        snake[snake.size() - 1].drawSegment();
        clearTrail();
    }

};

int main()
{
    // Random seed, time since epoch
    srand(time(0));

    drawBackground();

    // Initialize instance of classes
    aSlitheryHissingThing snake = aSlitheryHissingThing();
    Apple apple = snake.setApple();

    // Draw the apple and snake
    snake.drawTheWholeDarnSnake();
    apple.drawSegment();

    // Move to right by default
    unsigned short xIncre = 1, yIncre = 0;

    while (true)
    {
        if      (GetAsyncKeyState(0x57)) {xIncre =  0; yIncre = -1;} // w
        else if (GetAsyncKeyState(0x53)) {xIncre =  0; yIncre =  1;} // s
        else if (GetAsyncKeyState(0x41)) {xIncre = -1; yIncre =  0;} // a
        else if (GetAsyncKeyState(0x44)) {xIncre =  1; yIncre =  0;} // d

        snake.updateTheSnake(xIncre, yIncre);

        // What do you think this does?
        if (snake.areYouDeadYet()) {yourDed(); break;}

        // The apple is set through a method of the snake so it know where all the
        // segments are and can avoid them, extra segment is removed, is not very efficient
        if (snake.ateApple(apple)) {apple = snake.setApple(); apple.drawSegment(); snake.addSegment();}

        // Only the head is drawn, also clears the trail
        snake.drawSnakeHead();

        Sleep(100);
    }
    return 0;
}
