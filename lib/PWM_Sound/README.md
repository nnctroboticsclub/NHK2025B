#サンプルコード

```c
/**
 * @file main.cpp
 * @brief PwmNotePlayerクラスの動作確認用メインファイル
 */

#include "mbed.h"
#include "PwmNotePlayer.h" // 作成したクラスのヘッダーファイルをインクルード

    // PWM 出力ピン (ご自身の環境に合わせて変更してください)
    // Nucleo-F446RE の D2 ピンを使用

    /**
     * @brief メイン関数 (動作確認用)
     *
     * PwmNotePlayerクラスのインスタンスを作成し、
     * 各音階とオクターブの PWM 出力をテストします。
     * 出力ピンは PWM_OUTPUT_PIN で定義されています。
     */
    int
    main()
{

    // PwmNotePlayer のインスタンスを作成
    // PWM_OUTPUT_PIN を使用してPWM出力を初期化
    PwmNotePlayer player(D2);
    // 一通りのテスト用
    printf("player.putC4(): %f Hz\n", C4_FREQUENCY);
    player.putC4();
    ThisThread::sleep_for(500ms);

    printf("player.putC4Oct(1): %f Hz\n", C4_FREQUENCY * pow(2.0, 1));
    player.putC4Oct(1);
    ThisThread::sleep_for(500ms);

    printf("player.putC4Oct(-1): %f Hz\n", C4_FREQUENCY * pow(2.0, -1));
    player.putC4Oct(-1);
    ThisThread::sleep_for(500ms);

    printf("player.putNote(12): %f Hz (C5)\n", C4_FREQUENCY * pow(2.0, 12.0 / 12.0));
    player.putNote(12);
    ThisThread::sleep_for(500ms);

    printf("player.putNote(-12): %f Hz (C3)\n", C4_FREQUENCY * pow(2.0, -12.0 / 12.0));
    player.putNote(-12);
    ThisThread::sleep_for(500ms);

    printf("player.putNoteOct(0, 1): %f Hz (C5)\n", C4_FREQUENCY * pow(2.0, 0.0 / 12.0 + 1));
    putNoteOct(0, 1);
    ThisThread::sleep_for(500ms);

    printf("player.putNoteOct(0, -1): %f Hz (C3)\n", C4_FREQUENCY * pow(2.0, 0.0 / 12.0 - 1));
    player.putNoteOct(0, -1);
    ThisThread::sleep_for(500ms);

    printf("player.putNoteOct(7, 0): %f Hz (G4)\n", C4_FREQUENCY * pow(2.0, 7.0 / 12.0 + 0));
    player.putNoteOct(7, 0);
    ThisThread::sleep_for(500ms);

    player.stop(); // PWM を停止

    // ファイナルファンタジーより勝利のファンファーレ
    player.putNoteOct(2, 1);
    ThisThread::sleep_for(140ms);
    player.stop();
    player.putNoteOct(2, 1);
    ThisThread::sleep_for(140ms);
    player.stop();
    player.putNoteOct(2, 1);
    ThisThread::sleep_for(140ms);
    player.stop();
    player.putNoteOct(2, 1);
    ThisThread::sleep_for(500ms);
    player.putNoteOct(10, 0);
    ThisThread::sleep_for(500ms);
    player.putNoteOct(0, 1);
    ThisThread::sleep_for(500ms);
    player.putNoteOct(2, 1);
    ThisThread::sleep_for(125ms);
    player.stop();
    ThisThread::sleep_for(250ms);
    player.putNoteOct(0, 1);
    ThisThread::sleep_for(125ms);
    player.putNoteOct(2, 1);
    ThisThread::sleep_for(1s);
    player.stop();
    ThisThread::sleep_for(1s);

    return 0;
}
```