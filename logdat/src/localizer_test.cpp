// c++系
#include <iostream>
#include <iomanip>
// c系
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <ssm.hpp>
#include <time.h>

// localizer
#include "localizer.hpp"

// 配列の番号
#define _ROLL 0
#define _PITCH 1
#define _YAW 2
#define _X 0
#define _Y 1
#define _Z 2

// DSSMでUDPで通信するときに確保するバッファ
#define _BUFSIZE 512

using namespace std;

static int gShutOff = 0;
static void setSigInt(void);
static void ctrlC(int aStatus);
static void setupSSM(void);
static void Terminate(void);

//static unsigned int ssmdT = 500; //
//static double dT = 0.4750;//いい感じ　200Hz
static double dT = 0.4750;//いい感じ　200Hz
//static double dT = 100;//いい感じ　200Hz

static SSMApi<localizer> *LOCAL;

int main(int aArgc, char *aArgv[])
{
    int sensor_id = 0;

    SSMApi<localizer> localizer(LOCALIZER_SNAME, 0); // 移し替える
    LOCAL = &localizer;
    /********/
    try
    {
        // printf("確認\n");

        setupSSM(); // セットアップ
        setSigInt();

        printf("Start localizer_test\n");

        while (!gShutOff)
        {
/*          localizer.data.estPos[_X] = 1;
            localizer.data.estPos[_Y] = 1;
            localizer.data.estPos[_Z] = 1;
            localizer.data.estAng[_ROLL] = 1;
            localizer.data.estAng[_YAW] = 1;
            localizer.data.estAng[_PITCH] = 1;
            localizer.data.status = 1; // fix or float
*/
            localizer.data.estPos[_X] += 0.01;
            localizer.data.estPos[_Y] += 0.01;
            localizer.data.estPos[_Z] += 0.01;
            localizer.data.estAng[_ROLL] += 0.01;
            localizer.data.estAng[_YAW] += 0.01;
            localizer.data.estAng[_PITCH] += 0.01;
            localizer.data.status = 1; // fix or float

            localizer.write();
            usleepSSM(dT * 10000);
        }

        Terminate(); // 切断
    }
    catch (runtime_error const &error)
    {
        cout << error.what() << endl;
    }
    catch (...)
    {
        cout << "An unknown fatal error has occured. Aborting." << endl;
    }
    endSSM();
    return EXIT_SUCCESS;
}
static void ctrlC(int aStatus)
{
    signal(SIGINT, NULL);
    gShutOff = 1;
}
static void setSigInt()
{
    struct sigaction sig;
    memset(&sig, 0, sizeof(sig));
    sig.sa_handler = ctrlC;
    sigaction(SIGINT, &sig, NULL);
}
static void setupSSM(void)
{
    /**********************/
    // ssmを開く
    std::cerr << "initializing ssm ... ";
    if (!initSSM())
        throw std::runtime_error("[\033[1m\033[31mERROR\033[30m\033[0m]:fail to initialize ssm.");
    else
        std::cerr << "OK.\n";

	// create localizer
	std::cerr << "create localizer ... ";
	if( !LOCAL->create( 1, ( double )dT/1000.0 ) ){
		std::runtime_error( "[\033[1m\033[31mERROR\033[30m\033[0m]: Failed to create localizer on ssm.\n" );
	} else {
		std::cerr << "OK.\n";
	}
    //*********************//
    setSigInt();
}
static void Terminate(void)
{
    LOCAL->release();
    endSSM();
    printf("\nend\n");
}
