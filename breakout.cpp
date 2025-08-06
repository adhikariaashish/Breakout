#include <graphics.h>
#include <conio.h>
#include <vector>
#include <cmath>
#include <algorithm>

struct Brick
{
    int x, y, w, h;
    bool visible;
    int row; // Add row information for color coding
};

struct Ball
{
    float x, y, dx, dy, radius;
};

struct Paddle
{
    int x, y, w, h;
};

const int SCREEN_W = 800, SCREEN_H = 600;
const int BRICK_ROWS = 4, BRICK_COLS = 8;
const int BRICK_W = 100, BRICK_H = 40;
const int PADDLE_W = 100, PADDLE_H = 15;
const float BALL_RADIUS = 10.0f;

void drawBricks(const std::vector<Brick> &bricks)
{
    for (const auto &b : bricks)
    {
        if (b.visible)
        {
           
            if (b.row == 0)
            {
                setfillstyle(SOLID_FILL, RED);
            }
            else if (b.row == 1)
            {
                setfillstyle(SOLID_FILL, LIGHTRED);
            }
            else if (b.row == 2)
            {
                setfillstyle(SOLID_FILL, LIGHTGREEN);
            }
            else if (b.row == 3)
            {
                setfillstyle(SOLID_FILL, YELLOW);
            }
            bar(b.x, b.y, b.x + b.w, b.y + b.h);
        }
    }
}

void drawPaddle(const Paddle &paddle)
{
    setfillstyle(SOLID_FILL, LIGHTCYAN);
    bar(paddle.x, paddle.y, paddle.x + paddle.w, paddle.y + paddle.h);
}

void drawBall(const Ball &ball)
{
    setfillstyle(SOLID_FILL, WHITE);
    fillellipse((int)ball.x, (int)ball.y, (int)ball.radius, (int)ball.radius);
}

bool checkCollision(const Ball &ball, const Brick &brick)
{
    if (!brick.visible)
    
        return false;
    return ball.x + ball.radius > brick.x &&
           ball.x - ball.radius < brick.x + brick.w &&
           ball.y + ball.radius > brick.y &&
           ball.y - ball.radius < brick.y + brick.h;
}

bool checkPaddleCollision(const Ball &ball, const Paddle &paddle)
{
    return ball.x + ball.radius > paddle.x &&
           ball.x - ball.radius < paddle.x + paddle.w &&
           ball.y + ball.radius > paddle.y &&
           ball.y - ball.radius < paddle.y + paddle.h;
}

int main()
{
    initwindow(SCREEN_W, SCREEN_H, "Breakout Game");

    bool playAgain = true;
    while (playAgain)
    {
        // Clear screen at the start of each new game
        cleardevice();

        // Initialize bricks
        std::vector<Brick> bricks;
        for (int r = 0; r < BRICK_ROWS; ++r)
        {
            for (int c = 1; c < BRICK_COLS - 1; ++c) // Skip first (c=0) and last (c=BRICK_COLS-1) columns
            {
                bricks.push_back({c * BRICK_W, 50 + r * BRICK_H, BRICK_W - 2, BRICK_H - 2, true, r});
            }
        }

        Paddle paddle = {SCREEN_W / 2 - PADDLE_W / 2, SCREEN_H - 40, PADDLE_W, PADDLE_H};
        std::vector<Ball> balls; // Use vector to support multiple balls
        balls.push_back({SCREEN_W / 2.0f, SCREEN_H / 2.0f, 4.0f, -4.0f, BALL_RADIUS});

        int yellowBricksHit = 0; // Track yellow brick hits

        bool running = true;
        while (running)
        {
            // Clear only the moving objects' previous positions instead of entire screen
            setfillstyle(SOLID_FILL, BLACK);

            // Clear all balls' previous position areas
            for (const auto &ball : balls)
            {
                bar((int)(ball.x - ball.dx - ball.radius - 2), (int)(ball.y - ball.dy - ball.radius - 2),
                    (int)(ball.x - ball.dx + ball.radius + 2), (int)(ball.y - ball.dy + ball.radius + 2));
            }

            // Input
            int oldPaddleX = paddle.x; // Store current position before input
            if (GetAsyncKeyState(VK_LEFT))
                paddle.x -= 8;
            if (GetAsyncKeyState(VK_RIGHT))
                paddle.x += 8;
            if (paddle.x < 0)
                paddle.x = 0;
            if (paddle.x + paddle.w > SCREEN_W)
                paddle.x = SCREEN_W - paddle.w;

            // Clear paddle's previous position if it moved
            if (oldPaddleX != paddle.x)
            {
                bar(oldPaddleX, paddle.y, oldPaddleX + paddle.w, paddle.y + paddle.h);
            }

            // Ball movement
            for (auto &ball : balls)
            {
                ball.x += ball.dx;
                ball.y += ball.dy;
            }

            // Wall collision
            for (auto &ball : balls)
            {
                if (ball.x - ball.radius < 0 || ball.x + ball.radius > SCREEN_W)
                    ball.dx = -ball.dx;
                if (ball.y - ball.radius < 0)
                    ball.dy = -ball.dy;
            }

            // Paddle collision
            for (auto &ball : balls)
            {
                if (checkPaddleCollision(ball, paddle))
                {
                    ball.dy = -fabs(ball.dy);
                }
            }

            // Brick collision
            for (auto &ball : balls)
            {
                for (auto &brick : bricks)
                {
                    if (checkCollision(ball, brick))
                    {
                        brick.visible = false;
                        // Clear the destroyed brick area
                        setfillstyle(SOLID_FILL, BLACK);
                        bar(brick.x, brick.y, brick.x + brick.w, brick.y + brick.h);

                        // Check if it's a yellow brick (row 3)
                        if (brick.row == 3)
                        {
                            yellowBricksHit++;
                            // Add 2 more balls when 3rd yellow brick is hit
                            if (yellowBricksHit == 3 && balls.size() == 1)
                            {
                                balls.push_back({ball.x, ball.y, -ball.dx, ball.dy, BALL_RADIUS});
                                balls.push_back({ball.x, ball.y, ball.dx, -ball.dy, BALL_RADIUS});
                            }
                        }

                        ball.dy = -ball.dy;
                        break;
                    }
                }
            }

            // Remove balls that hit the ground
            balls.erase(std::remove_if(balls.begin(), balls.end(),
                                       [](const Ball &ball)
                                       { return ball.y + ball.radius > SCREEN_H; }),
                        balls.end());

            // Lose condition - Game Over if no balls left
            if (balls.empty())
            {
                cleardevice();
                outtextxy(SCREEN_W / 2 - 50, SCREEN_H / 2, "Game Over!");
                outtextxy(SCREEN_W / 2 - 80, SCREEN_H / 2 + 20, "Press any key to restart");
                running = false;
            }
            // Win condition
            bool win = true;
            for (const auto &brick : bricks)
                if (brick.visible)
                    win = false;
            if (win)
            {
                cleardevice();
                outtextxy(SCREEN_W / 2 - 50, SCREEN_H / 2, "You Win!");
                outtextxy(SCREEN_W / 2 - 80, SCREEN_H / 2 + 20, "Press any key to restart");
                running = false;
            }

            drawBricks(bricks);
            drawPaddle(paddle);
            for (const auto &ball : balls)
            {
                drawBall(ball);
            }

            delay(16);
        }

        delay(1000);

        while (kbhit())
            getch();

        getch();
    }

    closegraph();
    return 0;
}

