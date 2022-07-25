#pragma once



enum : long long {
    HIGH_MAX = (((long long) 1 << 32) - 1),//Max value of high
    QTR = (HIGH_MAX / 4 + 1),//fou
    CH_CARD = 256,//cardinality of the alphabet
    STEP_B = 3,//70
    STEP_A = 70,//70

    ROBUSTNESS_A1=95900*STEP_A,//text
    ROBUSTNESS_A2=178500*STEP_A
};









extern long long lasts;
extern unsigned result_byte;
extern unsigned masknum;


extern unsigned long long output_size;

class smart_in{
public:
    unsigned masknum = 0;
    unsigned result_byte = 0;
    FILE *infile;

    bool input_bit() {
        bool red_bit;
        if (masknum == 8) {
            result_byte = fgetc(infile);
            masknum = 0;
        }
        red_bit = result_byte & 1u;
        masknum++;
        result_byte >>= 1;

        return red_bit;
    }
};





class smart_ostream
{
public:
    unsigned    result_byte = 0;
    unsigned          masknum = 0;
    unsigned long long   output_size = 0;
    long long    lasts;
    FILE *outfile;

    explicit smart_ostream(char* filename)
    {
        outfile = fopen(filename, "w+b");
    }
    ~smart_ostream()
    {
        fclose(outfile);
    }
    void output_bit(int bit) {
        result_byte >>= 1u;
        if (bit)
            result_byte = (1u << 7u) | result_byte;
        masknum++;
        if (masknum == 8) {
            putc(result_byte, outfile);
            masknum = 0;
            output_size++;
        }
    }


    void smartprint(int bit) {
        output_bit(bit);
        bit = !bit;
        for (; lasts > 0; lasts--)
            output_bit(bit);
    }
    void encode_last(unsigned long long &low) {
        lasts++;
        smartprint(low >= QTR);
        putc(result_byte >> (8 - masknum), outfile);
    }
};

void model_init();
void model_upd(int symbol, unsigned Roboust);
void ArifmComp(unsigned Roboust, smart_ostream * smart_out, FILE* infile);
void ArifmDecomp(FILE *Outfile, smart_in *Smart_in);

void compress_ari(char *inputfilename, char *outputfilename);
void decompress_ari(char *inputfilename, char *outputfilename);
