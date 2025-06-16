#ifndef PWMNOTEPLAYER_H
#define PWMNOTEPLAYER_H

#include "mbed.h"

/**
 * @def C4_FREQUENCY
 * @brief 中央ハ (C4) の周波数 (Hz)
 */
#define C4_FREQUENCY 261.626f

/**
 * @class PwmNotePlayer
 * @brief PWM を使用して特定の周波数の音を出力する機能をカプセル化したクラス
 *
 * このクラスは、音楽の音階に基づいてPWM信号を生成し、
 * スピーカーなどに出力することで様々な音を鳴らすことができます。
 */
class PwmNotePlayer {
private:
  PwmOut _pwmOut;         /**< @brief PWM出力を制御するmbedのPwmOutオブジェクト */
  float _baseFrequency;   /**< @brief 基準となる周波数 (通常はC4の周波数) */

  /**
   * @brief 指定された周波数 (Hz) で PWM を出力するプライベート関数
   *
   * @param frequency 出力する周波数 (Hz)。0 以下の場合は PWM 出力を停止します。
   */
  void outputPwm(float frequency);

public:
  /**
   * @brief PwmNotePlayerクラスのコンストラクタ
   *
   * @param pwm_pin PWM 出力に使用するピン名 (例: D3, PA_5など)。
   * @param baseFreq 基準となる周波数 (Hz)。デフォルトはC4の周波数 (261.626 Hz)。
   */
  PwmNotePlayer(PinName pwm_pin, float baseFreq = C4_FREQUENCY);

  /**
   * @brief 中央ハ (C4, 基準周波数) の PWM を出力する関数
   */
  void putC4();

  /**
   * @brief 基準周波数に 2 の oct 乗を掛けた周波数で PWM を出力する関数
   *
   * @param oct オクターブ数 (整数)。正の値でオクターブが上がり、負の値で下がります。
   */
  void putC4Oct(int oct);

  /**
   * @brief 基準周波数に 2 の (note / 12) 乗を掛けた周波数で PWM を出力する関数
   *
   * @param note 基準音からの半音の数 (整数)。正の値で音が高くなり、負の値で低くなります。
   * 例えば、C4を0とした場合、12でC5、-12でC3になります。
   * 7でG4（完全5度上）など。
   */
  void putNote(int note);

  /**
   * @brief 基準周波数に 2 の (note / 12 + oct) 乗を掛けた周波数で PWM を出力する関数
   *
   * @param note 基準音からの半音の数 (整数)。
   * @param oct オクターブ数 (整数)。
   */
  void putNoteOct(int note, int oct);

  void stop();
};

#endif 

// noteの０～１１までの一覧
// ド	          C	        0
// ド# / レb	  C# / Db	  1
// レ         	D	        2
// レ# / ミb	  D# / Eb	  3
// ミ	          E	        4
// ファ       	F	        5
// ファ# / ソb	F# / Gb	  6
// ソ           G	        7
// ソ# / ラb	  G# / Ab	  8
// ラ	          A	        9
// ラ# / シb	  A# / Bb	  10
// シ         	B	11
