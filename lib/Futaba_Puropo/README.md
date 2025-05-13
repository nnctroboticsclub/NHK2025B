# 概要
sbus（futabaプロポ）のデータを受信して使いやすくするライブラリ

# sbus概要
https://rikei-tawamure.com/entry/2020/03/17/120606

# サンプルコード
(更新2024/08/22) 試してないから動くかわからんけど動くやろ
```c++
#include <mbed.h>
#include "puropo.h"

Puropo puropo(PA_0, PA_1);

int main()
{
    puropo.start();
    while(1)
    {
        printf("\n");
        for(int i=0; i<16; i++)
            printf("%0.2f ", puropo.get(i));
    }
}
```