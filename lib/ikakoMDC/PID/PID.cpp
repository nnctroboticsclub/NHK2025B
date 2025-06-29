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

/**
 * Includes
 */
#include "PID.h"

PID::PID(float Kc, float tauI, float tauD, float interval) {

    usingFeedForward = false;
    inAuto           = false;

    //Default the limits to the full range of I/O: 3.3V  デフォルトでは、I/O の全範囲に対する制限:3.3V
    //Make sure to set these to more appropriate limits for  これらの制限を適切な制限に設定してください。
    //your application.
    setInputLimits(0.0, 3.3);
    setOutputLimits(0.0, 3.3);

    tSample_ = interval;

    setTunings(Kc, tauI, tauD);

    setPoint_             = 0.0;
    processVariable_      = 0.0;
    prevProcessVariable_  = 0.0;
    controllerOutput_     = 0.0;
    prevControllerOutput_ = 0.0;

    accError_ = 0.0;
    bias_     = 0.0;
    
    realOutput_ = 0.0;

}

void PID::setInputLimits(float inMin, float inMax) {

    //Make sure we haven't been given impossible values.  不可能な値が与えられていないことを確認してください。
    if (inMin >= inMax) {
        return;
    }

    //Rescale the working variables to reflect the changes.  作業変数を再スケールして変更を反映します。
    prevProcessVariable_ *= (inMax - inMin) / inSpan_;
    accError_            *= (inMax - inMin) / inSpan_;

    //Make sure the working variables are within the new limits.  作業変数が新しい制限内であることを確認します。
    if (prevProcessVariable_ > 1) {
        prevProcessVariable_ = 1;
    } else if (prevProcessVariable_ < 0) {
        prevProcessVariable_ = 0;
    }

    inMin_  = inMin;
    inMax_  = inMax;
    inSpan_ = inMax - inMin;

}

void PID::setOutputLimits(float outMin, float outMax) {

    //Make sure we haven't been given impossible values.  不可能な値が与えられていないことを確認してください。
    if (outMin >= outMax) {
        return;
    }

    //Rescale the working variables to reflect the changes.
    prevControllerOutput_ *= (outMax - outMin) / outSpan_;

    //Make sure the working variables are within the new limits.  作業変数を再スケールして変更を反映します。
    if (prevControllerOutput_ > 1) {
        prevControllerOutput_ = 1;
    } else if (prevControllerOutput_ < 0) {
        prevControllerOutput_ = 0;
    }

    outMin_  = outMin;
    outMax_  = outMax;
    outSpan_ = outMax - outMin;

}

void PID::setTunings(float Kc, float tauI, float tauD) {

    //Verify that the tunings make sense.  チューニングが意味をなしていることを確認します。
    if (Kc == 0.0 || tauI < 0.0 || tauD < 0.0) {
        return;
    }

    //Store raw values to hand back to user on request.  要求に応じてユーザーに返す生の値を格納します。
    pParam_ = Kc;
    iParam_ = tauI;
    dParam_ = tauD;

    float tempTauR;

    if (tauI == 0.0) {
        tempTauR = 0.0;
    } else {
        tempTauR = (1.0 / tauI) * tSample_;
    }

    //For "bumpless transfer" we need to rescale the accumulated error.  「バンプレス転送」の場合、蓄積された誤差を再スケールする必要があります。
    if (inAuto) {
        if (tempTauR == 0.0) {
            accError_ = 0.0;
        } else {
            accError_ *= (Kc_ * tauR_) / (Kc * tempTauR);
        }
    }

    Kc_   = Kc;
    tauR_ = tempTauR;
    tauD_ = tauD / tSample_;

}

void PID::reset(void) {

    float scaledBias = 0.0;

    if (usingFeedForward) {
        scaledBias = (bias_ - outMin_) / outSpan_;
    } else {
        scaledBias = (realOutput_ - outMin_) / outSpan_;
    }

    prevControllerOutput_ = scaledBias;
    prevProcessVariable_  = (processVariable_ - inMin_) / inSpan_;

    //Clear any error in the integral.  整数のエラーをクリアします。
    accError_ = 0;

}

void PID::setMode(int mode) {

    //We were in manual, and we just got set to auto.  私たちは手動で、私たちはちょうど自動に設定されました。
    //Reset the controller internals.  コントローラの内部をリセットします。
    if (mode != 0 && !inAuto) {
        reset();
    }

    inAuto = (mode != 0);

}

void PID::setInterval(float interval) {

    if (interval > 0) {
        //Convert the time-based tunings to reflect this change.  時間ベースのチューニングを変換して、この変更を反映します。
        tauR_     *= (interval / tSample_);
        accError_ *= (tSample_ / interval);
        tauD_     *= (interval / tSample_);
        tSample_   = interval;
    }

}

void PID::setSetPoint(float sp) {

    setPoint_ = sp;

}

void PID::setProcessValue(float pv) {

    processVariable_ = pv;

}

void PID::setBias(float bias){

    bias_ = bias;
    usingFeedForward = 1;

}

float PID::compute() {

    //Pull in the input and setpoint, and scale them into percent span.  入力と設定点をプルし、パーセントスパンにスケールします。
    float scaledPV = (processVariable_ - inMin_) / inSpan_;

    if (scaledPV > 1.0) {
        scaledPV = 1.0;
    } else if (scaledPV < 0.0) {
        scaledPV = 0.0;
    }

    float scaledSP = (setPoint_ - inMin_) / inSpan_;
    if (scaledSP > 1.0) {
        scaledSP = 1;
    } else if (scaledSP < 0.0) {
        scaledSP = 0;
    }

    float error = scaledSP - scaledPV;

    //Check and see if the output is pegged at a limit and only  出力が制限でペギングされ、唯一のペギングされているかどうかを確認します。
    //integrate if it is not. This is to prevent reset-windup.  統合されていない場合は統合します。これは、リセット巻き上げを防ぐためです。
    if (!(prevControllerOutput_ >= 1 && error > 0) && !(prevControllerOutput_ <= 0 && error < 0)) {
        accError_ += error;
    }
    //Compute the current slope of the input signal.  入力信号の現在の傾きを計算します。
    float dMeas = (scaledPV - prevProcessVariable_) / tSample_;

    float scaledBias = 0.0;

    if (usingFeedForward) {
        scaledBias = (bias_ - outMin_) / outSpan_;
    }

    //Perform the PID calculation.  PID 計算を実行します。
    controllerOutput_ = scaledBias + Kc_ * (error + (tauR_ * accError_) - (tauD_ * dMeas));

    //Make sure the computed output is within output constraints.  計算された出力が出力制約内であることを確認します。
    if (controllerOutput_ < 0.0) {
        controllerOutput_ = 0.0;
    } else if (controllerOutput_ > 1.0) {
        controllerOutput_ = 1.0;
    }

    //Remember this output for the windup check next time.  次回の巻き上げチェックの出力を覚えておいてください。
    prevControllerOutput_ = controllerOutput_;
    //Remember the input for the derivative calculation next time.  次回の微分計算の入力を覚えておいてください。
    prevProcessVariable_  = scaledPV;

    //Scale the output from percent span back out to a real world number.  パーセントスパンからの出力を実世界の数値にスケールバックします。
    return ((controllerOutput_ * outSpan_) + outMin_);

}

float PID::getInMin() {

    return inMin_;

}

float PID::getInMax() {

    return inMax_;

}

float PID::getOutMin() {

    return outMin_;

}

float PID::getOutMax() {

    return outMax_;

}

float PID::getInterval() {

    return tSample_;

}

float PID::getPParam() {

    return pParam_;

}

float PID::getIParam() {

    return iParam_;

}

float PID::getDParam() {

    return dParam_;

}
