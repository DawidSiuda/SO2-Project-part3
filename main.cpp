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

#include "Ball.h"
#include "structures.h"
 
std::atomic<bool> myPause(false);
int endProgram = 0;
int windowID = -1;

std::mutex mutexBallsVectorDrawing;
std::mutex mutexBallsVectorCheckingCollizions;

struct BallWithOwnThread
{
    BallWithOwnThread( const std::atomic<bool> * const pauseFlag, float posX, float posY, float r, float movX, float movY)
    {
        ball = new Ball( posX, posY, r, movX, movY);
        thread = new std::thread(&Ball::calculateNevCoordinate, ball, pauseFlag);
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

void checkCollisions(std::vector<BallWithOwnThread*> * const balls)
{
    while(1) // <-- to change should be pause and exit
    {
        mutexBallsVectorCheckingCollizions.lock();

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
                        time_t my_time = time(NULL); 

                        std::cout<< my_time << " :Detected collizion : "<< i << "  " << (i+j) << "\n";

                        Ball::handleCillizion((*itFirstBall)->ball, (*itSecondBall)->ball);

                    }
                }
            }
        }
        catch(...)
        {
            std::cout<<"Thrown exception line: " << __LINE__ << " File: " << __FILE__ << ".\n";
        }


        mutexBallsVectorCheckingCollizions.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / FPS));
    }
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear frame
        glClearColor(0.2, 0.2, 0.2, 0);

        //
        //  Use async
        //
        #ifdef USE_ASYNC
        {
            std::queue<std::future<int>*> myThreads;

            for (std::vector<Ball*>::iterator it = balls.begin() ; it != balls.end(); ++it)
            {
                myThreads.push(new std::future<int> (std::async(&Ball::calculateNevCoordinate, *it)));
            }

            while(myThreads.empty() != true)
            {
                std::future<int> *temp = myThreads.front();
                //delete temp;
                myThreads.pop();
                temp->get();
            }

            for (std::vector<Ball*>::iterator it = balls.begin() ; it != balls.end(); ++it)
            {   
                DrawCircle(*it);
            }
        }
        #else // Use thread
        {
            mutexBallsVectorDrawing.lock();
            for (std::vector<BallWithOwnThread*>::iterator it = balls.begin() ; it != balls.end(); ++it)
            {   
                DrawCircle((*it)->ball);
            }
            mutexBallsVectorDrawing.unlock();
        }
        #endif

        glutSwapBuffers();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000/FPS));
    }
    else
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/FPS));
    }
    
}

void keyboard(unsigned char key, int x, int y)
{
    if(key=='p' || key=='P')
	{
		//myPause = myPause ? 0 : 1;

        myPause.store(!myPause.load());
        
    }

    if(key=='n' || key=='N')
	{
        // To change...  mutexes should be set one by one, not both in the same time.
        std::lock(mutexBallsVectorDrawing, mutexBallsVectorCheckingCollizions);
		//balls.push_back(new Ball(0,-1,0.05,0,0));
        
        try 
        {
            balls.push_back(new BallWithOwnThread(&myPause,0,-1,0.05,0,0));
            //myThreads.push(new std::thread(&Ball::calculateNevCoordinate, (balls.back()), &myPause));
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

    if(key=='e' || key=='E')
	{
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

}


int main(int argc, char** argv)
{
    std::cout << "Process id : " << getpid() << std::endl;

    std::thread *threadCheckingCollizion = new std::thread(&checkCollisions, &balls);
    //checkCollisions(std::vector<BallWithOwnThread*> * const balls)

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    
    glutInitWindowSize(640, 640);
    glutInitWindowPosition(100, 100);
    windowID = glutCreateWindow("First");
    //glutDisplayFunc(RenderScene);
    glutIdleFunc(main_loop_function);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}