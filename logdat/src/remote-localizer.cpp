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


// クライアント側（DSSM）
#include "ssm-proxy-client-child.hpp"

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

//dT*1000

//static unsigned int ssmdT = 500;   //
//static double dT = 5; // 単位　s   -> 200Hz
static double dT = 1; // 単位　s   -> 1000Hz
//static double dT = 0.1; // 単位　s   ->10000Hz
//static double dT = 20; // 単位　s   ->50Hz
//static double dT = 0.01; // 単位　s   ->100000Hz



static PConnectorClient<localizer> *REMOLOCAL;
static SSMApi <localizer> *LOCAL_REMOTE;

int main(int aArgc, char *aArgv[])
{
    int sensor_id = 0;

    // PConnectorClient<localizer> localizer(LOCALIZER_SNAME, 0, "192.168.8.165");
    PConnectorClient<localizer> remolocalizer(LOCALIZER_SNAME, 0,"10.8.0.1"); // opnevpn
    //PConnectorClient<localizer> remolocalizer(LOCALIZER_SNAME, 0); // ループバック
    REMOLOCAL = &remolocalizer;
    /********/
    SSMApi<localizer> mylocalizer(REMOTE_LOCALIZER_SNAME, 1); // 移し替える
    LOCAL_REMOTE = &mylocalizer;
    /********/
    try
    {
        // printf("確認\n");

        setupSSM(); // セットアップ
        setSigInt();

        printf("Start remote localizer\n");
        printf("buffar size ---> %d\n", _BUFSIZE);

        while (!gShutOff)
        {

            int num = 0;
            num = remolocalizer.getTID_topBuf(0) - remolocalizer.timeId;
            if (num > 0)
            { // numが０以下 データが更新されているか
                remolocalizer.readLastBuf();//bafから
                //                cout << localizer.data.estPos[_X] << endl;
                mylocalizer.data.estPos[_X] = remolocalizer.data.estPos[_X];
                mylocalizer.data.estPos[_Y] = remolocalizer.data.estPos[_Y];
                mylocalizer.data.estPos[_Z] = remolocalizer.data.estPos[_Z];
                mylocalizer.data.estAng[_ROLL] = remolocalizer.data.estAng[_ROLL];
                mylocalizer.data.estAng[_YAW] = remolocalizer.data.estAng[_YAW];
                mylocalizer.data.estAng[_PITCH] = remolocalizer.data.estAng[_PITCH];
                mylocalizer.data.status = remolocalizer.data.status; // fix or float
                mylocalizer.write();
            }
            usleep(dT * 1000); // 
        }

        Terminate();//切断
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

    REMOLOCAL->initSSM();//proxyが相手側で起動していなとAPIの関数内でエラーがおきる    ここで相手との通信接続をかける
  


    /**********************/
    // ssmを開く
    std::cerr << "initializing ssm ... ";
    if (!initSSM())
        throw std::runtime_error("[\033[1m\033[31mERROR\033[30m\033[0m]:fail to initialize ssm.");
    else
        std::cerr << "OK.\n";

    // localizer_remote を開く
	std::cerr << "open localizer_remote ... ";
	if( !LOCAL_REMOTE->create( 1, ( double )dT/1000.0 )){
		throw std::runtime_error( "[\033[1m\033[31mERROR\033[30m\033[0m]:fail to open localizer_remote on ssm.\n" );
	} else {
		std::cerr << "OK.\n";
	}
    //*********************//
    setSigInt();

    int dssmflag = 0;

    while (!dssmflag == 1)
    {
        // 通信先のSSMでlocalizerのデータが流れるまで
        if (dssmflag == 0)
        {
            //printf("確認\n");
            if (!REMOLOCAL->open(SSM_EXCLUSIVE))
            { // バッファを使用した読み込み
                std::cout << "remote localizer can not open\n"
                          << std::endl;
                //REMOLOCAL->terminate();
                usleep(2000000); // sleep2秒
            }
            else
            {
                std::cout << "remote localizer open success\n"
                          << std::endl;
                dssmflag = 1;

                // ストリームの作成
                if (!REMOLOCAL->UDPcreateDataCon())
                {
                    // REMOLOCAL->terminate();
                    // return 1;
                }
            }
        }
    }

    REMOLOCAL->readyRingBuf(_BUFSIZE); // buf確保 2のべき乗が最適らしい
}
static void Terminate(void)
{
    LOCAL_REMOTE->release();
    REMOLOCAL->release();
    endSSM();
    printf("\nend\n");
}
