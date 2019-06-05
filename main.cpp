#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <queue>
#include <future>
#include <thread>
#include <unistd.h>
#include <mutex>
#include <string.h>
#include <sstream>

#include "Ball.h"
#include "structures.h"
#include "Bat.h"

void produceNewBall(const std::atomic<bool> *pause, const std::atomic<bool> *end);
void keyDown(unsigned char key, int x, int y);

std::atomic<bool> myPause(false);
std::atomic<bool> myEndProgram(false);

int windowID = -1;

std::mutex mutexBallsVectorDrawing;
std::mutex mutexBallsVectorCheckingCollizions;

std::thread *ballProducer = new std::thread(&produceNewBall, &myPause, &myEndProgram);

Bat leftBat;
Bat rightBat;


std::atomic<int> leftScore(0);
std::atomic<int> rightScore(0);

struct BallWithOwnThread
{
    BallWithOwnThread( const std::atomic<bool> * pauseFlag, float posX, float posY, float r, float movX, float movY)
    {
        ball = new Ball( posX, posY, r, movX, movY);
        thread = new std::thread(&Ball::calculateNevCoordinate, ball, pauseFlag, &leftBat, &rightBat, &leftScore, &rightScore);
    }

    ~BallWithOwnThread()
    {
        //std::cout<<"Destructor\n";
        delete ball;
        delete thread;
    }

    Ball * ball = nullptr;
    std::thread * thread = nullptr;
};

std::vector<BallWithOwnThread*> balls;

void produceNewBall(const std::atomic<bool> *pause, const std::atomic<bool> *end)
{
    while(end->load() == false)
    {
        if(pause->load() == false)
        {
            // To change...  mutexes should be set one by one, not both in the same time.
            std::lock(mutexBallsVectorDrawing, mutexBallsVectorCheckingCollizions);
            //balls.push_back(new Ball(0,-1,0.05,0,0));
            
            try 
            {
                balls.push_back(new BallWithOwnThread(&myPause,0,0,0.025,0,0));
            }
            catch (const std::exception& e) 
            {
                std::cerr << "EXEPTION: " << e.what() << std::endl;
                return;
            }
            catch (...)
            {
                std::cerr << "EXEPTION" << std::endl;
                return;
            }

            mutexBallsVectorDrawing.unlock();
            mutexBallsVectorCheckingCollizions.unlock();
        }

        //std::cout << "DAWID DAWID \n";
        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_TO_CREATE_NEW_BALL));
    }
}

void checkCollisions(std::vector<BallWithOwnThread*> * const balls)
{
    while(myEndProgram.load() == false )
    {
        // mutexBallsVectorCheckingCollizions.lock();
        std::lock_guard<std::mutex> lg(mutexBallsVectorCheckingCollizions);

        try
        {
            int i = 0, j = 0;
            for (std::vector<BallWithOwnThread*>::iterator itFirstBall = balls->begin() ; itFirstBall  != balls->end(); ++itFirstBall)
            {   
                i++;
                float firstX = (*itFirstBall)->ball->getX();
                float firstY = (*itFirstBall)->ball->getY();
                float firstR = (*itFirstBall)->ball->getR();

                for (std::vector<BallWithOwnThread*>::iterator itSecondBall = itFirstBall +1 ; itSecondBall  != balls->end(); ++itSecondBall)
                {   
                    j++;
                    float secondX = (*itSecondBall)->ball->getX();
                    float secondY = (*itSecondBall)->ball->getY();
                    float secondR = (*itSecondBall)->ball->getR();

                    if ( firstX + firstR + secondR > secondX && 
                         (firstX - firstR) - secondR < secondX &&
                         firstY + firstR + secondR > secondY &&
                         (firstY - firstR) - secondR < secondY )
                    {
                        // time_t my_time = time(NULL); 

                        // std::cout<< my_time << " :Detected collizion : "<< i << "  " << (i+j) << "\n";

                       // Ball::handleCillizion((*itFirstBall)->ball, (*itSecondBall)->ball);

                    }
                }
            }
        }
        catch(...)
        {
            std::cout<<"Thrown exception line: " << __LINE__ << " File: " << __FILE__ << ".\n";
        }


        //mutexBallsVectorCheckingCollizions.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / FPS));
    }
    //std::cout << "END\n";
}

void DrawCircle(Ball *ball) 
{
    float ballX = ball->getX();
    float ballY = ball->getY();
    float r     = ball->getR();
    float theta=0;

    const Color * const color = ball->getColor();

    //std::cout << "Draw " << ballX  << " | " << ballY << "\n";

    glColor3b(color->r, color->g, color->b);

	glBegin(GL_POLYGON);

	while(theta<2*M_PI)
    {
		glVertex3f(ballX,ballY,0);
		glVertex3f(ballX+(r*cos(theta)),ballY+(r*sin(theta)),0);
		theta+=0.5;
		glVertex3f(ballX+(r*cos(theta)),ballY+(r*sin(theta)),0);
		glVertex3f(ballX,ballY,0);
	}

    glEnd();

}

void main_loop_function() 
{
    if(myPause == false)
    {    
        //
        // Check if game should end.
        //

        if (leftScore.load() > 99 || rightScore.load() > 99)
        {
            keyDown('e', 0, 0);
        }


        //
        // Clean buffer.
        //

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear frame
        glClearColor(0.2, 0.2, 0.2, 0);


        // 
        // Draw top and bottom line
        //

        glColor3f(1.0, 1.0, 1.0);

        glLineWidth(2);

        glBegin(GL_LINES);
        glVertex2f(-1, TOP_LINE_Y);
        glVertex2f(1, TOP_LINE_Y);
        glEnd();

        glBegin(GL_LINES);
        glVertex2f(-1, BOTTOM_LINE_Y);
        glVertex2f(1, BOTTOM_LINE_Y);
        glEnd();

        //
        // Draw left Bat.
        //

        glColor3f(leftBat.getColor().r, leftBat.getColor().g, leftBat.getColor().b);

        glLineWidth(leftBat.getThickness());

        glBegin(GL_LINES);
        glVertex2f(-1, leftBat.getBottomEdgeYPossition());
        glVertex2f(-1, leftBat.getBottomEdgeYPossition() + leftBat.getLength());
        glEnd();

        //
        // Draw right Bat.
        //

        glColor3f(rightBat.getColor().r, rightBat.getColor().g, rightBat.getColor().b);

        glLineWidth(rightBat.getThickness());

        glBegin(GL_LINES);
        glVertex2f(1, rightBat.getBottomEdgeYPossition());
        glVertex2f(1, rightBat.getBottomEdgeYPossition() + rightBat.getLength());
        glEnd();

        //
        // Draw text.
        //

        std::string score;
        std::stringstream scoreStream;

        scoreStream << " " << leftScore.load() <<  " : " << rightScore.load();

        score = scoreStream.str();

        const char* pointerScore = score.c_str();

        int w = glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24,  (const unsigned char*)pointerScore);

        glRasterPos2f(-0.05, 0.7);

        int len = strlen((const char *)pointerScore);

        for (int i = 0; i < len; i++)
        {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *pointerScore);
            pointerScore++;
        }

        //
        //  Use async.
        //

        mutexBallsVectorDrawing.lock();
        for (std::vector<BallWithOwnThread*>::iterator it = balls.begin() ; it != balls.end(); ++it)
        {   
            DrawCircle((*it)->ball);
        }
        mutexBallsVectorDrawing.unlock();

        glutSwapBuffers();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000/FPS));
    }
    else
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/FPS));
    }
    
}

void keyDown(unsigned char key, int x, int y)
{
    if(key == 'p' || key == 'P')
	{
        myPause.store(!myPause.load());
    }

    if(key == 'n' || key == 'N')
	{
        std::lock(mutexBallsVectorDrawing, mutexBallsVectorCheckingCollizions);
       
        try 
        {
            balls.push_back(new BallWithOwnThread(&myPause,0,0,0.025,0,0));
        }
        catch (const std::exception& e) 
        {
            std::cerr << "EXEPTION: " << e.what() << std::endl;
            return;
        }
        catch (...)
        {
            std::cerr << "EXEPTION" << std::endl;
            return;
        }

        mutexBallsVectorDrawing.unlock();
        mutexBallsVectorCheckingCollizions.unlock();
    }

    if(key == 'e' || key == 'E')
	{
        myEndProgram.store(true);
        leftBat.endThread();
        rightBat.endThread();

        std::lock(mutexBallsVectorDrawing, mutexBallsVectorCheckingCollizions);
        for (std::vector<BallWithOwnThread*>::iterator it = balls.begin() ; it != balls.end(); ++it)
        {   
            (*it)->ball->setEndLoop();
        }

        while (balls.empty() == false)
        {
            balls.back()->thread->join();
            delete balls.back();
            balls.pop_back();
        }

        mutexBallsVectorDrawing.unlock();
        mutexBallsVectorCheckingCollizions.unlock();

        glutDestroyWindow(windowID);
        exit (0);

    }

    //
    // Drive of left bat.
    //

    if(key == 'z' || key == 'Z')
    {
        //leftBatDirectionsOfMoving.changeDirect;

        leftBat.changeDirect(BOTTOM);
        // leftBatDirectionsOfMoving.store(BOTTOM);
    }

    if(key == 'a' || key == 'A')
    {
        //leftBatDirectionsOfMoving.store(TOP);
        leftBat.changeDirect(TOP);
    }

    //
    // Drive of right bat.
    //

    if(key == 'm' || key == 'M')
    {
        //rightBatDirectionsOfMoving.store(BOTTOM);
        rightBat.changeDirect(BOTTOM);
    }

    if(key == 'k' || key == 'K')
    {
        //rightBatDirectionsOfMoving.store(TOP);
        rightBat.changeDirect(TOP);
    }
}

void keyUp(unsigned char key, int x, int y)
{
            //std::cout<< "DAWID" << std::endl;

    if((key == 'z' || key == 'Z') &&  leftBat.getDirect() == BOTTOM )
    {
        //leftBatDirectionsOfMoving.store(STOP);
        leftBat.changeDirect(STOP);
    }
    else if ((key == 'a' || key == 'A' ) &&  leftBat.getDirect() == TOP )
    {
        //leftBatDirectionsOfMoving.store(STOP);
        leftBat.changeDirect(STOP);
    }

    if((key == 'm' || key == 'M' ) && rightBat.getDirect() == BOTTOM )
    {
        //rightBatDirectionsOfMoving.store(STOP);
        rightBat.changeDirect(STOP);
    }
    else if ((key == 'k' || key == 'K') && rightBat.getDirect() == TOP )
    {
        //rightBatDirectionsOfMoving.store(STOP);
        rightBat.changeDirect(STOP);
    }
}


int main(int argc, char** argv)
{
    std::cout << "Process id : " << getpid() << std::endl;

    std::thread *threadCheckingCollizion = new std::thread(&checkCollisions, &balls);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    windowID = glutCreateWindow("First");

    glutIdleFunc(main_loop_function);
    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutMainLoop();
    return 0;
}