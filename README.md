# wood_rs485
RS485の独自プロトコルver1.00   
アドレスを用いた複数バイト通信をサポートする。

## マニュアル  
実際の使用に関してはheaderファイルの説明や、実際にこのライブラリが使用されているコードを参照の事。
  
WoodRS485Data型のデータをやり取りする。  
receiver_addressは送信先のアドレスでsender_addressは送信元のアドレス、ユーザーが送りたいデータはdata配列に、そのサイズをsizeに入れる。送信時はsender_addressはWoodRS485Managerより自動的に代入されるようになっている。ただしアドレスは0-7は使用禁止である。(機能追加を見据えて)
~~~
typedef struct{
    uint8_t receiver_address;
    uint8_t sender_address;
    uint8_t data[WOODRS485_DATA_SIZE];
    int size;
}WoodRS485Data;
~~~
  
内部ではWoodRS485Managerで状態を管理している。これは受信したデータをためることや、自身のアドレスを設定することで自身宛のアドレスにデータが来た時にだけ、それを知らせることができる。
~~~
typedef struct{
    uint8_t buf[WOODRS485_DATA_SIZE+4];
    int counter;
    uint8_t address;
}WoodRS485Manager;
~~~
## バイナリデータ
COBSを用いた多バイト通信を行っている。まずエンコードを行う前の元データは以下の通りである。ここではNバイトのデータを送信するとする。
  
| バイト数 | データ |
----|---- 
| 1 | 送信先アドレス |
| 2 | 送信元アドレス |
| 3 ~ N+2 | データ |

実際に通信路に流れるデータは以下の通りである。
  
| バイト数 | データ |
----|---- 
| 1 | COBS関連 |
| 2 | 送信先アドレス(エンコード済) |
| 3 | 送信元アドレス(エンコード済) |
| 4 ~ N+3 | データ(エンコード済) |
| N+4 | COBS関連(0) |