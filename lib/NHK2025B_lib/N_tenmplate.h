/**
 * @file N_template.h
 * @author 高野 絆(takanokiduna@gmail.com)
 * @brief NHK2025Bのテンプレートクラス
 * @version 0.1
 * @date 2025-06-06
 *
 * @copyright Copyright (c) 2025
 */
#ifndef NHK2025B_TEMPLATE_H
#define NHK2025B_TEMPLATE_H

#include <mbed.h>

#define NUM_OF_PARAM 1 // たいてい definitions.h にある.(NUM_OF_ROBOMASとかNUM_OF_SERVOとか)

class TemplateParameter
{
public:
    int template_int = 10;
    float template_float = 0.1;
};

class NHK2025B_Template
{
public:
    /**
     * @brief コンストラクタ
     *
     * @param param パラメータの配列
     * @param num param[]の要素数
     */
    NHK2025B_Template(std::array<TemplateParameter, NUM_OF_PARAM> param = {TemplateParameter()})
    {
        for (int i = 0; (i < param.size()); i++)
        {
            // 配列の0番目から順にデバイス番号0から割り振られていく
            template_data[i].parameter = param[i];
        }
    }

    /**
     * @brief セットアップ関数
     */
    void setup()
    {
        ;
    }

    /**
     * @brief whileループする
     */
    void update()
    {
        ;
    }

    /**
     * @brief ts間隔(1ms)でループする(重い処理は入れてはならない)
     */
    void update_ts()
    {
        ;
    }

    /**
     * @brief パラメータをセットする
     *
     * @param num デバイス番号 (0 <= num < NUM_OF_PARAM)
     * @param param パラメータ
     */
    void setTemplateParameter(int num, TemplateParameter param)
    {
        template_data[num].parameter = param;
    }

    /**
     * @brief パラーメータを取得する
     *
     * @param num デバイス番号 (0 <= num < NUM_OF_PARAM)
     *
     * @return パラメータ
     */
    TemplateParameter getParam(int num)
    {
        return template_data[num].parameter;
    }

    /**
     * @brief 関数1
     */
    void func1()
    {
        ;
    }

    /**
     * @brief 関数2
     *
     * @param cmd 指令値
     */
    void func2(int cmd)
    {
        ;
    }

    /**
     * @brief 関数3
     *
     * @return [単位] センサー値
     */
    void func3()
    {
        ;
    }

    /**
     * @brief デバッグ用関数
     */
    void print_debug()
    {
        ;
    }

private:
    struct
    {
        struct
        {
            ;
        } cmd;
        struct
        {
            ;
        } state;
        TemplateParameter parameter;
    } template_data[NUM_OF_PARAM];
};

#endif // NHK2025B_TEMPLATE_H
