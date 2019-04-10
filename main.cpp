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
 
int myPause = 0;
int endProgram = 0;
int windowID = -1;

std::queue<std::thread*> myThreads;

std::vector<Ball*> balls;

std::mutex mutexBallsVector;

void DrawCircle(Ball *ball) 
{
    float ballX = ball->getX();
    float ballY = ball->getY();

    //std::cout << "Draw " << ballX  << " | " << ballY << "\n";

    glBegin(GL_LINE_LOOP);

    for (int ii = 0; ii < 100; ii++)   {
        float theta = 2.0f * 3.1415926f * float(ii) / float(100);//get the current angle 
        float x = ball->getR() * cosf(theta);//calculate the x component 
        float y = ball->getR() * sinf(theta);//calculate the y component 
        glVertex2f(x + ballX, y + ballY);//output vertex 
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
            mutexBallsVector.lock();
            for (std::vector<Ball*>::iterator it = balls.begin() ; it != balls.end(); ++it)
            {   
                DrawCircle(*it);
            }
            mutexBallsVector.unlock();
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
		myPause = myPause ? 0 : 1;
    }

    if(key=='n' || key=='N')
	{
        mutexBallsVector.lock();
		balls.push_back(new Ball(0,-1,0.05,0,0));
        
        try 
        {
            myThreads.push(new std::thread(&Ball::calculateNevCoordinate, (balls.back())));
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

        mutexBallsVector.unlock();
    }

    if(key=='e' || key=='E')
	{
        mutexBallsVector.lock();
        for (std::vector<Ball*>::iterator it = balls.begin() ; it != balls.end(); ++it)
        {   
            (*it)->setEndLoop();
        }

        while (myThreads.empty()== false)
        {   
            // myThreads.back()->join();
            myThreads.front()->join();
            myThreads.pop();
        }

        while(balls.empty() == false)
        {   
            delete balls.back();
            balls.pop_back();
        }

        mutexBallsVector.unlock();

        glutDestroyWindow(windowID);
        exit (0);

    }

}


int main(int argc, char** argv)
{
    std::cout << "Process id : " << getpid() << std::endl; 

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