//
// Created by 西澤佳祐 on 2017/03/06.
//

#ifndef MELODYMEMO_WINDOW_FUNCTION_H
#define MELODYMEMO_WINDOW_FUNCTION_H

#endif //MELODYMEMO_WINDOW_FUNCTION_H
void Hanning_window(double w[], int N)
{
    int n;

    if (N % 2 == 0) /* Nが偶数のとき */
    {
        for (n = 0; n < N; n++)
        {
            w[n] = 0.5 - 0.5 * cos(2.0 * M_PI * n / N);
        }
    }
    else /* Nが奇数のとき */
    {
        for (n = 0; n < N; n++)
        {
            w[n] = 0.5 - 0.5 * cos(2.0 * M_PI * (n + 0.5) / N);
        }
    }
}
