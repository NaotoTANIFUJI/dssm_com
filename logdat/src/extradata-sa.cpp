/*************
 *
 * datから標準偏差を求めるプログラム
 *
 *
 ***************************/

#include "ssm-log.hpp"
#include "localizer.hpp"
#include <ssm.h>
#include <signal.h>
#include <stdexcept>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
// #include "utility.hpp"
#include <math.h>
// cosやsinの引数はラジアン

#define gnuplotver 1 // コメントアウトしたらただ表示するだけ  1681458218.245851 1681457982.483280

// path remote_localizer
#define LOGPATH "/home/haselab-08/test_viewer/logdat/log/ntp/2023.0414.1640/remote_localizer.log"
//#define LOGPATH "/home/haselab-08/test_viewer/logdat/log/ntp/2023.0414.1639/localizer.log"
// path localizer
#define DATLOGPATH "/home/haselab-08/test_viewer/logdat/log/ntp/PC2.dat"

// gnuplo範囲
#define XLANGEMIN 0.0
//#define XLANGEMAX 26872.0 // pc1のデータの数
#define YLANGEMIN 0.0
#define YLANGEMAX 0.4

// offset データを片方のPC時間の開始に合わせる　PC2のlocalizerに
#define OFFSET 1681457982.483280

#define NUM 200000

#define PI 3.14159265358979323846

static int gShutOff = 0;
static void setSigInt(void);
static void Terminate(void);
static int ReturnArray(double (*array)[3]);
double array[NUM][3];

double rand_normal(double mean, double std_dev)
{
    double u1, u2, z;
    u1 = (double)rand() / RAND_MAX;
    u2 = (double)rand() / RAND_MAX;
    z = sqrt(-2.0 * log(u1)) * cos(2 * PI * u2);
    return mean + std_dev * z;
}

int main(void)
{
    srand((unsigned int)time(NULL));

    // dat failを開く
    double sum = (double)ReturnArray(array); // 戻り値総数

    SSMLog<localizer> local;

    if (!local.open(LOGPATH))
    {
        fprintf(stderr, "Error! Caonnot open localizer,logfile");
        exit(EXIT_FAILURE);
    }

    localizer *localdata;
    localdata = &local.data();

    double offset, offset1;
    int flag;
    double count;
    double bef, now, sa;
    double num[1000][2];
    double offsettt[NUM] = {};
    int i = 0;

#ifdef gnuplotver
    FILE *gnuplot;                   // gnuplot
    gnuplot = popen("gnuplot", "w"); // gnuplot
#endif

    try
    {
        setSigInt();

        local.read();
        offset = local.time() - local.getStartTime(); // 初期化
        // offset1 = local.time() - local.getStartTime(); // 初期化
        sa = 0;
        bef = 0;

        count = 0;
        int count2 = 0;

        double time_offset = 0;

        while (!gShutOff)
        {
            unsigned long cnt = 0;
            while (local.read())
            {
                /****************************************/
                // とりあえず１秒超えたら計算する

                now = local.time() - local.getStartTime();
                // 1秒間経過後
                if (now - offset >= 1)
                {
                    count++;
                    // printf("offset-->%lf,time--->%f,count-->%f,Hz--->%f\n", bef - offset, bef, count,count/(bef-offset)); // 出力
                    // printf("ΔT-->%lf,time--->%f,count-->%f,Hz--->%f\n", now - offset, now, count,count/(now-offset)); // 出力 kore

#ifdef gnuplotver
                    num[i][0] = now;
                    num[i][1] = count / (now - offset);
                    i++;
                    //printf("%f %f\n", now, count / (now - offset)); // 確認
#else
                    // printf("%f %f\n", now, count / (now - offset)); // 出力
                    // printf("time--->%f,pos_X--->%lf\n", now, localdata->estPos[0]); // 出力
#endif
                    count = 0;                                    // 初期化
                    offset = local.time() - local.getStartTime(); // 次のoffsetとして保存
                }
                else
                {
                    count++; // カウント
                }
                /**************************************/

                bef = local.time() - local.getStartTime(); // 1つ前のデータ保存
                // printf("%ld %f %lf\n", cnt, local.time() - OFFSET, localdata->estPos[0]); // 出力 データの番号　時間　中身　offset 片方の時間
                // printf("%ld %f %lf\n", cnt, local.time()-local.getStartTime(), localdata->estPos[0]); // 出力 データの番号　時間　中身  offset 最初の時間
                //printf("%ld %f %lf\n", cnt, local.time(), localdata->estPos[0]); // 出力 データの番号　時間　中身

                // 中身が同じデータを探して　時間の差をとる
                for (int i = 0; i < sum; i++)
                {
                    if (localdata->estPos[0] - array[i][2] <= 0) //==だとなぜかだめ
                    {
                        time_offset += (local.time() - OFFSET) - array[i][1];     // offset 蓄積
                        offsettt[count2] = (local.time() - OFFSET) - array[i][1]; // offset を記録
                        count2++;
                        break;
                    }
                }

                cnt++;
                // break;
            }
            
            //　平均
            double ave = time_offset / count2; // 平均
            printf("平均-->%f\n", ave); // 平均

            // 分散の計算
            double hensa = 0;
            double bunsan = 0;
            for (int f = 0; f < count2; f++)
            {
                hensa += pow(offsettt[f] - ave, 2); // 自乗
            }
            bunsan = hensa / count2;
            printf("分散-->%f\n", bunsan); // 分散

            // 標準偏差
            double SD = 0;
            SD = sqrt(bunsan);
            printf("標準偏差-->%f\n", SD); // 標準偏差

#define XLANGEMAX (double)count2 // pc1のデータの数

#ifdef gnuplotver
            for (int k = 0; k < count2; k++)
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
                fprintf(gnuplot, "set label '平均=%lf\n分散=%lf\n標準偏差=%lf'\n\n",ave,bunsan,SD);
                // fprintf(gnuplot, "set grid linecolor 'dark-red'\n\n");
                fprintf(gnuplot, "set xrange[%f:%f]\nset yrange[%f:%f]\n\n", XLANGEMIN, XLANGEMAX, YLANGEMIN, YLANGEMAX); // 範囲
                fprintf(gnuplot, "p ");
                fprintf(gnuplot, " '-' pt 7 ps 1 lc rgb 'red' t \"\" "); // 文法的な感じ  odm 2
                fprintf(gnuplot, "\n");
                fprintf(gnuplot, "%d, %lf\n", k, offsettt[k]); //
                // fflush(gnuplot);                                    // 一つの文法の定義にひとつ必要 1
            }
            fprintf(gnuplot, "e\n");
            fflush(gnuplot); // 必須
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
int ReturnArray(double (*array)[3])
{
    // ＷＰファイルを開いて移し替える
    FILE *fp;
    if ((fp = fopen(DATLOGPATH, "r")) == NULL)
    {
        printf(" error \n");
        exit(EXIT_FAILURE);
    }

    double num, time, x;
    int count = 0;
    while (fscanf(fp, "%lf %lf %lf", &num, &time, &x) != EOF)
    {
        // printf("%lf,%lf,%lf\n", num, time, x);
        array[count][0] = num;
        array[count][1] = time;
        array[count][2] = x;
        count++;
    }
    fclose(fp);

    return count;
}
