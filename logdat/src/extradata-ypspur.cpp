
#include "ssm-log.hpp"
#include "localizer.hpp"
#include <ssm.h>
#include <signal.h>
#include <stdexcept>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "remote-log.hpp"
// #include "utility.hpp"
#include <math.h>
// cosやsinの引数はラジアン

#define gnuplotver 1  //コメントアウトしたらただ表示するだけ

//path
#define LOGPATH "/home/haselab-08/test2-intern/log/2023.0208.1329/remotelog.log"

//gnuplo範囲
#define XLANGEMIN 0.0
#define YLANGEMIN 0.0
#define YLANGEMAX 10.0



static int gShutOff = 0;
static void setSigInt(void);
static void Terminate(void);

int main(void)
{
    SSMLog<remotelog_fs> remote;

    if (!remote.open(LOGPATH))
    {
        fprintf(stderr, "Error! Caonnot open localizer,logfile");
        exit(EXIT_FAILURE);
    }

    remotelog_fs *remotedata;
    remotedata = &remote.data();

    double offset, offset1;
    int flag;
    double count;
    double bef, now, sa;
    double num[1000][2];
    int i = 0;

#ifdef gnuplotver
    FILE *gnuplot;                   // gnuplot
    gnuplot = popen("gnuplot", "w"); // gnuplot
#endif

    try
    {
        setSigInt();

        remote.read();
        offset = remote.time() - remote.getStartTime(); // 初期化
        // offset1 = local.time() - local.getStartTime(); // 初期化
        sa = 0;
        bef = 0;

        count = 0;

        while (!gShutOff)
        {
            /****************************************
             * とりあえず１秒超えたら計算する
             *
             *
             * **************************************/
unsigned long cnt = 0;
            while (remote.read())
            {
                now = remote.time() - remote.getStartTime();
                // 1秒間経過後
                if (now - offset >= 1)
                {
                    count++;
                    //printf("offset-->%lf,time--->%f,count-->%f,Hz--->%f\n", bef - offset, bef, count,count/(bef-offset)); // 出力
                    printf("ΔT-->%lf,time--->%f,count-->%f,Hz--->%f\n", now - offset, now, count,count/(now-offset)); // 出力 kore

#ifdef gnuplotver
                    num[i][0] = now;
                    num[i][1] = count / (now - offset);
                    //num[i][1] = count;
                    i++;
                    printf("%f %f\n", now, count / (now - offset)); // 確認
#else
                    //printf("%f %f\n", now, count / (now - offset)); // 出力
                    //printf("time--->%f,pos_X--->%lf\n", now, localdata->estPos[0]); // 出力
#endif
                    count = 0;                                    // 初期化
                    offset = remote.time() - remote.getStartTime(); // 次のoffsetとして保存
                }
                else
                {
                    count++; // カウント
                }
                bef = remote.time() - remote.getStartTime(); // 1つ前のデータ保存
                //printf("%ld %f %lf\n", cnt, now, remotedata->x); // 出力 データの番号　時間　中身
                //printf("mdata_size>%ld\n", local.dataSize()); // 出力
                //printf("mdata_size>%ld\n", sizeof(localizer)); // 出力
                cnt++;
            }
#ifdef gnuplotver
            for (int k = 0; k < i; k++)
            {
                fprintf(gnuplot, "set grid\n\n");
                fprintf(gnuplot, "set size ratio -1\n\n");
                fprintf(gnuplot, "set xlabel 'time [s]'\n\n");
                fprintf(gnuplot, "set ylabel 'frequency [Hz]'\n\n");
                fprintf(gnuplot, "set key below bottom\n\n");
                fprintf(gnuplot, "set size square \n\n");
                fprintf(gnuplot, "set xlabel font 'Arial,22'\n\n");
                fprintf(gnuplot, "set ylabel font 'Arial,22'\n\n");
                fprintf(gnuplot, "set xlabel offset 0,-1\n\n");
                fprintf(gnuplot, "set ylabel offset -4,0\n\n");
                fprintf(gnuplot, "set tics font 'Arial,15'\n\n");
                fprintf(gnuplot, "set grid linewidth 1.5\n\n");
                fprintf(gnuplot, "set ytics 1\n\n");
                //fprintf(gnuplot, "set grid linecolor 'dark-red'\n\n");
                fprintf(gnuplot, "set xrange[%f:%f]\nset yrange[%f:%f]\n\n", XLANGEMIN, now, YLANGEMIN, YLANGEMAX); // 範囲
                fprintf(gnuplot, "p ");
                fprintf(gnuplot, " '-' pt 7 ps 1 lc rgb 'red' t \"\" ");  // 文法的な感じ  odm 2
                fprintf(gnuplot, "\n");
                fprintf(gnuplot, "%lf, %lf\n", num[k][0], num[k][1]); //
                //fflush(gnuplot);                                    // 一つの文法の定義にひとつ必要 1
            }
            fprintf(gnuplot, "e\n"); 
            fflush(gnuplot); //必須
            while (!gShutOff)
            {
                usleep(1000);
            }
            pclose(gnuplot);
#endif

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
