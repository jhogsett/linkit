#include<iostream>

#include <cstdlib>

using namespace std;

#define ARG_MARKER (0x00)
#define NUM_SUB_ARGS (3)

int sub_args[NUM_SUB_ARGS];
unsigned char bytes[20];

bool fits_in_byte(int value){
    char char_value = (char)value;
    return value == (int)char_value;
}

int encode_args(unsigned char *bytes, int write_index, int *sub_args)
{
    unsigned char marker = 0;
    int index = write_index + 1;
    unsigned char markers[3];

    for(int i = 0; i < NUM_SUB_ARGS; i++){
        int arg = sub_args[i];

        if(arg != 0){
            if(fits_in_byte(arg)){
                markers[i] = 0x01;
                bytes[index] = (unsigned char)arg;
                index += 1;
            } else {
                markers[i] = 0x02;
                unsigned short * word = (unsigned short *)&bytes[index];
                *word = (unsigned short)arg;
                index += 2;
            }
        }
    }

    for(int i = NUM_SUB_ARGS-1; i >= 0; i--){
        marker |= markers[i];
        if(i > 0){
            marker = marker << 2;
        }
    }

    bytes[write_index] = (unsigned char)marker | ARG_MARKER;

    return index - write_index;
}

void decode_args(unsigned char *bytes, int read_index, int *sub_args)
{
    unsigned char marker = bytes[read_index];
    int index = read_index + 1;

    for(int i = 0; i < NUM_SUB_ARGS; i++){
        int count = marker & 0x03;
    
        //cout << count << "\n";

        if(count == 1){
            sub_args[i] = (int)((char)bytes[index]);
            index += 1;
        } else if(count == 2){
            short * word = (short *)&bytes[index];
            sub_args[i] = (int)*word;
            index += 2;
        } else {
            sub_args[i] = 0;
        }
        marker = marker >> 2;
    }
}






void test(int arg0, int arg1, int arg2){
    unsigned char bytes[8];
    memset(bytes, 0, 8);

    sub_args[0] = arg0;
    sub_args[1] = arg1;
    sub_args[2] = arg2;

    cout << "testing: " << arg0 << " " << arg1 << " " << arg2 << "\n";

    int count = encode_args(bytes, 0, sub_args);

    cout << count << "\n\n";

    for(int i = 0; i < 8; i++){
       cout << (int)bytes[i] << "\n";
    }
}

int random_arg(){
    return (rand() % 65536) - 32768;
}

bool test2(){
    int arg0 = random_arg();
    int arg1 = random_arg();
    int arg2 = random_arg();

    //cout << "encoded args: " << arg0 << " " << arg1 << " " << arg2 << "\n";

    sub_args[0] = arg0;
    sub_args[1] = arg1;
    sub_args[2] = arg2;

    unsigned char bytes[8];
    memset(bytes, 0, 8);

    encode_args(bytes, 0, sub_args);

    //cout << "encoded bytes: ";
    //for(int i = 0; i < 8; i++){
    //    cout << (int)bytes[i] << " ";
    //}
    //cout << "\n";

    sub_args[0] = 0;
    sub_args[1] = 0;
    sub_args[2] = 0;

    decode_args(bytes, 0, sub_args);

    //cout << "decoded args: " << sub_args[0] << " " << sub_args[1] << " " << sub_args[2] << "\n\n";

    return arg0 == sub_args[0] && arg1 == sub_args[1] && arg2 == sub_args[2];
}

void run_test(int times){
    for(int i = 0; i < times; i++){
        if(!test2()){
            cout << "Failed!\n";

            cout << "encoded bytes: ";
            for(int i = 0; i < 8; i++){
                cout << (int)bytes[i] << " ";
            }
            cout << "\n";

            return;
        }
    }
}


int main()
{
    srand (time(NULL));
    // cout << rand();


    unsigned char bytes[20];
    memset(bytes, 0, 20);

 //  test(0,0,0);
 //  test(-1,-1,-1);
 //  test(50,500,5000);
 //  test(-50,-500,-5000);

 //   if(!test2()){
 //       cout << "Failed!\n";
 //   }

    run_test(100000000);

    return 0;
}

