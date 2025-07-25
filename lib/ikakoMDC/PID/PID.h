/**
 * @author Aaron Berk
 *
 * @section LICENSE
 *
 * Copyright (c) 2010 ARM Limited
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @section DESCRIPTION
 * 
 * A PID controller is a widely used feedback controller commonly found in
 * industry.
 *
 * This library is a port of Brett Beauregard's Arduino PID library:
 *
 *  http://www.arduino.cc/playground/Code/PIDLibrary
 *
 * The wikipedia article on PID controllers is a good place to start on
 * understanding how they work:
 *
 *  http://en.wikipedia.org/wiki/PID_controller
 *
 * For a clear and elegant explanation of how to implement and tune a
 * controller, the controlguru website by Douglas J. Cooper (who also happened
 * to be Brett's controls professor) is an excellent reference:
 *
 *  http://www.controlguru.com/
 */

#ifndef PID_H
#define PID_H

/**
 * Includes
 */
#include "mbed.h"

/**
 * Defines
 */
#define MANUAL_MODE 0
#define AUTO_MODE   1

/**
 * Proportional-integral-derivative controller.
 */
class PID {

public:

    /**
     * Constructor.
     *
     * Sets default limits [0-3.3V], calculates tuning parameters, and sets    

     * manual mode with no bias.
     *
     * @param Kc - Tuning parameter
     * @param tauI - Tuning parameter
     * @param tauD - Tuning parameter
     * @param interval PID calculation performed every interval seconds.
     
     コンス トラクター。
     
      デフォルトの制限を設定 [0-3.3V], チューニングパラメータを計算, セット
     バイアスなしの手動モード。
     
      @param Kc - チューニングパラメータ
     @paramタウI - チューニングパラメータ
      @paramタウD - チューニングパラメータ
      @param間隔の PID 計算は、間隔の秒ごとに実行されます。
     


     */
    PID(float Kc, float tauI, float tauD, float interval);
    PID(){}

    /**
     * Scale from inputs to 0-100%.
     *
     * @param InMin The real world value corresponding to 0%.
     * @param InMax The real world value corresponding to 100%.
     
     入力から 0 ~ 100% にスケールします。
     *
     * @param InMin 0%に対応する実世界の値。
     * @param InMax 100% に対応する実世界値。
     */
    void setInputLimits(float inMin , float inMax);

    /**
     * Scale from outputs to 0-100%.
     *
     * @param outMin The real world value corresponding to 0%.
     * @param outMax The real world value corresponding to 100%.

     出力から 0 ~ 100% にスケールします。
     *
     * @param outMin 0%に対応する実世界の値。
     * @paramマックス 100% に対応する実世界の値。     
     */
    void setOutputLimits(float outMin, float outMax);

    /**
     * Calculate PID constants.
     *
     * Allows parameters to be changed on the fly without ruining calculations.
     *
     * @param Kc - Tuning parameter
     * @param tauI - Tuning parameter
     * @param tauD - Tuning parameter
     
     PID定数を計算します。
     *
     *計算を台無しにすることなく、その場でパラメータを変更することができます。
     *
     * @param Kc - チューニングパラメータ
     * @paramタウI - チューニングパラメータ
     * @paramタウD - チューニングパラメータ
     */
    void setTunings(float Kc, float tauI, float tauD);

    /**
     * Reinitializes controller internals. Automatically
     * called on a manual to auto transition.
     
     コントローラの内部を再初期化します。自動的に
     自動遷移にマニュアルで呼び出されます。
     */
    void reset(void);
    
    /**
     * Set PID to manual or auto mode.
     *
     * @param mode        0 -> Manual
     *             Non-zero -> Auto
     
     PID を手動モードまたは自動モードに設定します。
     
      @paramモード 0 - > マニュアル
                  ゼロ以外の -> オート
     */
    void setMode(int mode);
    
    /**
     * Set how fast the PID loop is run.
     *
     * @param interval PID calculation peformed every interval seconds.
     
     PID ループの実行速度を設定します。
     @param間隔 PID 計算は、間隔の秒ごとに変更されます。
     */
    void setInterval(float interval);
    
    /**
     * Set the set point.
     *
     * @param sp The set point as a real world value.
     
     設定点を設定します。
     @param sp設定点を実世界値として指定します。
     */
    void setSetPoint(float sp);
    
    /**
     * Set the process value.
     *
     * @param pv The process value as a real world value.
     
     プロセス値を設定します。
     @param pv 実世界値としてのプロセス値。
     */
    void setProcessValue(float pv);
    
    /**
     * Set the bias.
     *
     * @param bias The bias for the controller output.
     
     バイアスを設定します。
     @paramバイアス コントローラ出力のバイアス。
     */
    void setBias(float bias);

    /**
     * PID calculation.
     *
     * @return The controller output as a float between outMin and outMax.
     
     PID 計算。
     @return outMin と outMax の間の浮動小数点としてコントローラ出力。
     */
    float compute(void);

    //Getters.
    float getInMin();
    float getInMax();
    float getOutMin();
    float getOutMax();
    float getInterval();
    float getPParam();
    float getIParam();
    float getDParam();

private:

    bool usingFeedForward;
    bool inAuto;

    //Actual tuning parameters used in PID calculation. PID 計算で使用される実際のチューニング パラメータ。
    float Kc_;
    float tauR_;
    float tauD_;
    
    //Raw tuning parameters.
    float pParam_;
    float iParam_;
    float dParam_;
    
    //The point we want to reach.  私たちが到達したいポイント。
    float setPoint_;         
    //The thing we measure.  私たちが測定する事。
    float processVariable_;  
    float prevProcessVariable_;
    //The output that affects the process variable.  プロセス変数に影響する出力。
    float controllerOutput_; 
    float prevControllerOutput_;

    //We work in % for calculations so these will scale from  計算に対して % を使用するので、これらの値は % から拡張されます。
    //real world values to 0-100% and back again.  実世界の値は 0 から 100% に、再び戻ります。
    float inMin_;
    float inMax_;
    float inSpan_;
    float outMin_;
    float outMax_;
    float outSpan_;

    //The accumulated error, i.e. integral.  累積誤差、すなわち積分。
    float accError_;
    //The controller output bias.  コントローラの出力バイアス。
    float bias_;

    //The interval between samples.  サンプル間の間隔。
    float tSample_;          

    //Controller output as a real world value.  実世界値としてのコントローラ出力。
    volatile float realOutput_;

};

#endif /* PID_H */
