/**
 * @file PwmNotePlayer.cpp
 * @brief PwmNotePlayerクラスの実装ファイル
 */

#include "PwmNotePlayer.h" // 対応するヘッダーファイルをインクルード

/**
 * @brief PwmNotePlayerクラスのコンストラクタ
 *
 * @param pwm_pin PWM 出力に使用するピン名。
 * @param baseFreq 基準となる周波数。
 */
PwmNotePlayer::PwmNotePlayer(PinName pwm_pin, float baseFreq)
    : _pwmOut(pwm_pin), _baseFrequency(baseFreq) {}

void PwmNotePlayer::outputPwm(float frequency)
{
  if (frequency > 0)
  {
    _pwmOut.period(1.0 / frequency); // period (秒) = 1 / frequency (Hz)
    _pwmOut.write(0.5);              // デューティ比 50% (調整可能)
  }
  else
  {
    _pwmOut.write(0.0); // 周波数が無効な場合は PWM を停止
  }
}

void PwmNotePlayer::putC4()
{
  outputPwm(_baseFrequency);
}

void PwmNotePlayer::putC4Oct(int oct)
{
  outputPwm(_baseFrequency * pow(2.0, oct));
}

void PwmNotePlayer::putNote(int note)
{
  outputPwm(_baseFrequency * pow(2.0, static_cast<float>(note) / 12.0));
}

void PwmNotePlayer::putNoteOct(int note, int oct)
{
  outputPwm(_baseFrequency * pow(2.0, static_cast<float>(note) / 12.0 + oct));
}

void PwmNotePlayer::stop()
{
  _pwmOut.write(0.0);
}