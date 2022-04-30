/* 
 * File: wood_rs485.h
 * Author: wood
 * Comments: wood_rs485 protocol file
 * Revision history: ver1.00 2022/04/24
 */

/*
 * wood RS485とは独自規格のRS485通信のプロトコルです。
 * データはCOBSを用いて送信側でエンコードし、受信側でデコードします。
 * 送るデータは以下の通りです。
 * 1バイト目    :送信先アドレス
 * 2バイト目    :送信元アドレス
 * 3バイト目～  :ユーザーが送りたいデータ(最大252バイト)
 * 
 * ただしアドレスの0-7は使用禁止(機能追加を見据えて)
 */

#ifndef WOOD_RS485_H
#define	WOOD_RS485_H

#include <stdbool.h>    //use bool
#include <stdint.h>     //use uint8_t

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#define WOODRS485_DATA_SIZE  32     //ユーザーが送りたいデータのバイト数
    
typedef struct{
    uint8_t buf[WOODRS485_DATA_SIZE+4];
    int counter;
    uint8_t address;
}WoodRS485Manager;

typedef struct{
    uint8_t receiver_address;
    uint8_t sender_address;
    uint8_t data[WOODRS485_DATA_SIZE];
    int size;
}WoodRS485Data;

////////////////////////////////////////////////////////////////////////////////////////////////////
//user function
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * 初期化関数
 * 
 * 引数
 * *manager:WoodRS485Manager
 * address:アドレス
 * 
 * 返り値
 * 無し
*/
void woodRS485Init(WoodRS485Manager *manager, uint8_t address);

/*
 * UARTの受信関数等で受信したデータをこの関数に渡す
 * 
 * 引数
 * manager:WoodRS485Manager
 * data:データ
 * 
 * 返り値
 * 必要なデータが揃ったと思われる場合にtrueが返る
*/
bool woodRS485AddByte(WoodRS485Manager *manager, uint8_t data);

/*
 * woodRS485AddByteでtrueが返された場合、この関数でデータを取り出す(内部でデコード処理を行っている)
 * 
 * 引数
 * manager:WoodRS485Manager
 * result_data:受け取ったデータ(sizeが-1の場合は受信失敗を表す)
 * 
 * 返り値
 * managerに設定されたアドレスの場合trueが返る
*/
bool woodRS485GetData(WoodRS485Manager *manager, WoodRS485Data *result_data);

/*
 * 送信する用のデータを作製する(エンコード処理をする)
 * 
 * 引数
 * manager:WoodRS485Manager
 * result_buf:エンコードしたデータ
 * result_size:エンコードしたデータのバイト数で、アドレスのバイトも含む(0の場合はエンコード失敗)
 * data:送りたいデータ(送信元アドレスは初期化関数の値から自動でセットされるので設定しなくても良い)
 * 
 * 返り値
 * エンコードに成功した場合はtrueが返る
*/
void woodRS485MakeData(WoodRS485Manager *manager, uint8_t result_buf[], int *result_size, WoodRS485Data data);

////////////////////////////////////////////////////////////////////////////////////////////////////
//internal function
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * COBSを用いたエンコード関数
 * 
 * 引数
 * return_data:エンコード結果のデータ
 * return_size:エンコード結果のデータサイズ(0の場合はエンコード失敗)
 * data:変換したいデータ
 * size:変換したいデータのバイト数
 * 
 * 返り値
 * 無し
*/
void woodRS485CobsEncode(uint8_t return_data[], int *return_size, uint8_t data[], int size);

/*
 * COBSを用いたデコード関数
 * 
 * 引数
 * return_data:デコード結果のデータ
 * return_size:デコード結果のデータサイズ(0の場合はデコード失敗)
 * packet:変換したいデータ
 * size:変換したいデータのバイト数
 * 
 * 返り値
 * 無し
*/
void woodRS485CobsDecode(uint8_t return_data[], int *return_size, uint8_t packet[], int size);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* WOOD_RS485_H */

