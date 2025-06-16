#ifndef  _LPF_H_
#define  _LPF_H_

class lpf{
    public:
        lpf(float,float);                       //コンストラクタ  引数1:制御周期(s)  引数2:何秒で目標値に到達してほしいか(s)
        float path_value(float);                //ローパスフィルタ通過後の値を返す
        void change_time_constant(float);       //時定数を変更するための関数
        void reset(void);                       //一定の処理が終わった時に前回の偏差を0に戻す関数
        
        float one_before_output;                //前回の出力値を保管
        
    private:
        float system_cycle_second;              //制御周期
        float f_t_const;                        //フィルタリング時定数（filtering_time_constant）
        float output;


};
#endif