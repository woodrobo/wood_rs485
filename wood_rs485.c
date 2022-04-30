#include "wood_rs485.h"

void woodRS485Init(WoodRS485Manager *manager, uint8_t address){
    manager->counter = 0;
    manager->address = address;
}
    
bool woodRS485AddByte(WoodRS485Manager *manager, uint8_t data){
    manager->buf[manager->counter] = data;
    manager->counter++;
    if(data == 0){
        return true;
    }else if(manager->counter == WOODRS485_DATA_SIZE){
        manager->counter = 0;
    }
    return false;
}

bool woodRS485GetData(WoodRS485Manager *manager, WoodRS485Data *result_data){
    //for decode
    uint8_t result_buf[WOODRS485_DATA_SIZE + 2];
    int result_size;
    
    result_data->size=-1;
    
    if(manager->buf[manager->counter - 1] == 0){
        woodRS485CobsDecode(result_buf, &result_size, manager->buf, manager->counter);
        manager->counter=0;
        if(result_size >= 2){
            //データを格納
            result_data->receiver_address = result_buf[0];
            result_data->sender_address = result_buf[1];
            result_data->size = result_size - 2;
            
            int i;
            for(i=0;i<result_data->size;i++){
                result_data->data[i] = result_buf[i+2];
            }
            
            //このデバイス向けのデータならtrueを返す
            if(result_buf[0] == manager->address){
                return true;
            }
        }
    }
    return false;
}

void woodRS485MakeData(WoodRS485Manager *manager, uint8_t result_buf[], int *result_size, WoodRS485Data data){
    //for encode
    uint8_t data_buf[WOODRS485_DATA_SIZE + 2];
    
    data_buf[0] = data.receiver_address;
    data_buf[1] = manager->address;
    
    int i;
    for(i=0;i<data.size;i++){
        data_buf[i+2] = data.data[i];
    }
    
    woodRS485CobsEncode(result_buf, result_size, data_buf, data.size+2);
}

void woodRS485CobsEncode(uint8_t return_data[], int *return_size, uint8_t data[], int size){
    if (size > 254)
    {
        //data length must be less than 255
        *return_size = 0;
        return;
    }
    *return_size = size + 2;

    int zero_index = *return_size - 1;
    return_data[*return_size - 1] = 0;
    
    int i;
    for (i = *return_size - 2; i > 0; i--)
    {
        if (data[i - 1] == 0)
        {
            return_data[i] = (uint8_t)(zero_index - i);
            zero_index = i;
        }
        else
        {
            return_data[i] = data[i - 1];
        }
    }
    return_data[0] = (uint8_t)zero_index;
}

void woodRS485CobsDecode(uint8_t return_data[], int *return_size, uint8_t packet[], int size){
    *return_size = 0;
    if(size > 256){
        //packet length must be less than 257
        return ;
    }

    int len = size;

    int zero_index = (int)(packet[0] & 0xff);

    int i;
    for(i=1;i<len-1;i++){
        if(i == zero_index){
            return_data[i-1] = 0;
            zero_index = i + (int)(packet[i] & 0xff);
        }else{
            return_data[i-1] = packet[i];
        }
    }
    if((packet[len-1] == 0) && (zero_index == len-1)){
        *return_size = len - 2;
        return;
    }else{
        //zero_index error
        return;
    }
}