
#include "ssm-log.hpp"
#include "localizer.hpp"
#include <ssm.h>
#include <signal.h>
#include <stdexcept>
#include <stdlib.h>
// #include "utility.hpp"
#include <math.h>
// cosやsinの引数はラジアン

static int gShutOff = 0;
static void setSigInt(void);
static void Terminate(void);

int main(void)
{
    SSMLog<localizer> local;

    if (!local.open("/home/haselab-08/SnowRemoval_log/202304/20230402/log_am6/2023.0402.0749/localizer.log"))
    {
        fprintf(stderr, "Error! Caonnot open localizer,logfile");
        exit(EXIT_FAILURE);
    }

    localizer *localdata;
    localdata = &local.data();

    double offset, offset1;
    int flag;
    int count;
    double bef, now, sa;

    try
    {
        setSigInt();

        local.read();
        offset = local.time() - local.getStartTime();  // 初期化
        offset1 = local.time() - local.getStartTime(); // 初期化
        sa=0;

        count = 0;

        while (!gShutOff)
        {
            while (local.read())
            {

                now = local.time() - local.getStartTime();
/*
                if(sa<1){
                sa+=(now-bef);
                }

                if(sa>1){
                    printf("sa-->%lf,count--->%d\n",sa,count);
                    sa=0;
                    count=0;
                }
                else{
                    count++;
                }

*/
                // 1秒間経過後

                if ((local.time() - local.getStartTime()) - offset >= 1)
                {
                    printf("offset-->%lf,time--->%f,Hz--->%d\n", (local.time() - local.getStartTime()) - offset, local.time() - local.getStartTime(), count); // 出力
                    count = 0;                                                                                                                                // 初期化
                    offset = local.time() - local.getStartTime();                                                                                             // 次のoffsetとして保存
                }
                else
                {
                    count++; // カウント
                }
    
                

                //               printf("time--->%fdir--->%lf\n",(local.time()-local.getStartTime())-offset,localdata->estPos[0]);
                //               offset=local.time()-local.getStartTime();
                //printf("offset--->%f\n",now-bef);
                bef = local.time() - local.getStartTime(); // 1つ前のデータ保存
            }

            gShutOff = 1;
        }
    }
    catch (std::runtime_error const &error)
    {
        std::cout << error.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "An unknown fatal error has occured. Aborting." << std::endl;
    }
    Terminate();
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
static void Terminate(void)
{
    printf("\nend\n");
}
