#include <graphics.h>
#include <conio.h>
#include <vector>
#include <cmath>

struct Brick
{
    int x, y, w, h;
    bool visible;
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
const int BRICK_ROWS = 5, BRICK_COLS = 10;
const int BRICK_W = 60, BRICK_H = 20;
const int PADDLE_W = 100, PADDLE_H = 15;
const float BALL_RADIUS = 10.0f;

void drawBricks(const std::vector<Brick> &bricks)
{
    for (const auto &b : bricks)
    {
        if (b.visible)
        {
            setfillstyle(SOLID_FILL, RED);
            bar(b.x, b.y, b.x + b.w, b.y + b.h);
        }
    }
}

void drawPaddle(const Paddle &paddle)
{
    setfillstyle(SOLID_FILL, BLUE);
    bar(paddle.x, paddle.y, paddle.x + paddle.w, paddle.y + paddle.h);
}

void drawBall(const Ball &ball)
{
    setfillstyle(SOLID_FILL, YELLOW);
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
        // Initialize bricks
        std::vector<Brick> bricks;
        int offsetX = (SCREEN_W - (BRICK_COLS * BRICK_W)) / 2;
        for (int r = 0; r < BRICK_ROWS; ++r)
        {
            for (int c = 0; c < BRICK_COLS; ++c)
            {
                bricks.push_back({offsetX + c * BRICK_W, 50 + r * BRICK_H, BRICK_W - 2, BRICK_H - 2, true});
            }
        }

        Paddle paddle = {SCREEN_W / 2 - PADDLE_W / 2, SCREEN_H - 40, PADDLE_W, PADDLE_H};
        Ball ball = {SCREEN_W / 2.0f, SCREEN_H / 2.0f, 4.0f, -4.0f, BALL_RADIUS};

        bool running = true;
        while (running)
        {
            cleardevice();

            // Input
            if (GetAsyncKeyState(VK_LEFT))
                paddle.x -= 8;
            if (GetAsyncKeyState(VK_RIGHT))
                paddle.x += 8;
            if (paddle.x < 0)
                paddle.x = 0;
            if (paddle.x + paddle.w > SCREEN_W)
                paddle.x = SCREEN_W - paddle.w;

            // Ball movement
            ball.x += ball.dx;
            ball.y += ball.dy;

            // Wall collision
            if (ball.x - ball.radius < 0 || ball.x + ball.radius > SCREEN_W)
                ball.dx = -ball.dx;
            if (ball.y - ball.radius < 0)
                ball.dy = -ball.dy;

            // Paddle collision
            if (checkPaddleCollision(ball, paddle))
            {
                ball.dy = -fabs(ball.dy);
                // ball.dy = -abs(ball.dy);
            }

            // Brick collision
            for (auto &brick : bricks)
            {
                if (checkCollision(ball, brick))
                {
                    brick.visible = false;
                    ball.dy = -ball.dy;
                    break;
                }
            }

            // Lose condition - Game Over if ball touches the ground
            if (ball.y + ball.radius > SCREEN_H)
            {
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
                outtextxy(SCREEN_W / 2 - 50, SCREEN_H / 2, "You Win!");
                outtextxy(SCREEN_W / 2 - 80, SCREEN_H / 2 + 20, "Press any key to restart");
                running = false;
            }

            drawBricks(bricks);
            drawPaddle(paddle);
            drawBall(ball);

            delay(16);
        }

        // Wait for user input to restart or exit
        getch();
    }

    closegraph();
    return 0;
}