#include <cstdlib>
#include <cstdio>

#include "ari.h"


unsigned long long STEP = 3;

unsigned long long ROBUSTNESS_B1 = 1380 * STEP_B;
unsigned long long ROBUSTNESS_B2 = 2040 * STEP_B;
using namespace std;

FILE *infile;
unsigned long long Roboustness;



unsigned long long orig_size_arifm = 0;


unsigned long long output_size;



long long lasts;
unsigned result_byte;
unsigned masknum;

unsigned long long *probs = new unsigned long long[CH_CARD + 2];
unsigned long long freq_low [CH_CARD + 2];
unsigned long long freq_high [CH_CARD + 2];
unsigned long long *cur_frame = new unsigned long long[CH_CARD + 2];
unsigned long long *prev_frame = new unsigned long long[CH_CARD + 2];
unsigned long long *freq =  new unsigned long long[CH_CARD + 2];
unsigned long long freqdistrib1[] = {0, 8, 1, 1, 1, 25, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1,
                                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 19, 1, 1, 1, 1, 1, 1, 1, 1,
                                     1, 1, 1, 3, 1, 2, 1, 3, 1, 2, 1, 1, 3, 1, 1, 2, 1, 1, 2, 1, 2,
                                     3, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                     1, 1, 1, 4, 4, 5, 2, 1, 1, 1, 2, 1, 1, 1, 2, 2, 1, 1, 2, 1, 1,
                                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                     1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 2, 4, 2, 3, 6, 1, 2, 6, 2, 3, 4,
                                     3, 5, 8, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 50,
                                     21, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                     1, 1, 1, 1, 1, 1};//RUSSIAN distrib

unsigned long long freqdistrib_norm[] ={
        0,  28,   1,   1,   1, 101,   1,   1,   1,   1,   1,   2,   1,
        1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        1,   1,   1,   1,   1,   1,   1,  21,   1,   1,   1,   1,   1,
        1,   1,   1,   1,   1,   1,   3,   1,   2,   1,   8,   2,   4,
        2,   4,   9,   2,   2,   7,   2,   4,   5,   4,   6,  11,   3,
        5,   6,   7,   3,   1,   1,   1,   2,   1,   1,   1,   2,   3,
        1,   1,   3,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1
};//TEST2 DISTRIB
unsigned tabe_of_last = 300;
unsigned  STEP_slow = 2;
unsigned  STEP_fast = 3;
unsigned diff_th = 6900;
unsigned landmark = 300;//700*5

void model_init(unsigned Roboust) {//initializing the ArjfmComp model
    tabe_of_last = 300;
    STEP_slow = 3;
    STEP_fast = 4;
    diff_th = 6900;
    landmark = 700*3;//700*5
    freq[0] = 0;
    freq_low[0] = 0;
    freq_high[0] = 0;
    cur_frame[0] = 0;
    orig_size_arifm = 0;

    if(Roboust == 3)
    {
        for (unsigned i = 1; i <= CH_CARD + 1; i++)//TODO: try default distrib
        {
            freq[i] = freqdistrib1[i];
        }
    } else {
        STEP = 2;
        for (unsigned i = 1; i <= CH_CARD + 1; i++)//TODO: try default distrib
        {

            freq_low[i] = 1;
            freq_high[i] = 1;
            cur_frame[i] = 1;
            freq[i]= 1;
        }
    }

    for (int idx = CH_CARD + 1, freq_acc = 0; idx >= 0; idx--) {
        probs[idx] = freq_acc;
        freq_acc += freq[idx];
    }
}

void model_upd(int symbol, unsigned Roboust) {
    orig_size_arifm++;

    if (Roboust > 1) {
        STEP = STEP_A;
        if (Roboust % 2) {
            Roboustness = ROBUSTNESS_A1;
        } else {
            Roboustness = ROBUSTNESS_A2;
        }

        if (probs[0] >=
            Roboustness) {//   Roboustness variable less - more roubust for data structure changes
            for (int idx = CH_CARD + 1, freq_acc = 0; idx >= 0; idx--) {
                freq[idx] = (freq[idx] + 1) / 2;// ceil
                probs[idx] = freq_acc;
                freq_acc += freq[idx];
            }
        }

        for (freq[symbol] += STEP; symbol > 0; probs[symbol] += STEP)
            symbol--;
    } else {
        //////////
        if (Roboust % 2) {
            Roboustness = ROBUSTNESS_B1;
        } else {
            Roboustness = ROBUSTNESS_B2;
        }
        unsigned long long norma_low = 0;
        unsigned long long norma_high = 0;
        for (int i = 0; i <= CH_CARD + 1; i++) {
            norma_high += freq_high[i];
            norma_low += freq_low[i];
        }


        if (probs[0] >= Roboustness * STEP/STEP_B ) {//Roboustness variable less
            for (int idx = CH_CARD + 1, freq_acc = 0; idx >= 0; idx--) {//
                freq[idx] = (freq[idx] + 1) / 2;// - more roubust for data structure changes
            }
        }

        if (norma_high >= (Roboustness-1800) * STEP/STEP_B ) {
            for (int idx = CH_CARD + 1, freq_acc = 0; idx >= 0; idx--) {
                freq_high[idx] = (freq_high[idx] + 1) / 2;// ceil
            }
        }


        if (norma_low >= (Roboustness+1800) * STEP/STEP_B) {
            for (int idx = CH_CARD + 1, freq_acc = 0; idx >= 0; idx--) {
                freq_low[idx] = (freq_low[idx] + 1) / 2;// ceil
            }
        }





        if (orig_size_arifm % landmark == 0 && orig_size_arifm > landmark) {

            unsigned long long norma_cur = 0;
            unsigned long long norma_prev = 0;
            for (int i = 0; i <= CH_CARD + 1; i++) {
                norma_cur += cur_frame[i];
                norma_prev += prev_frame[i];
            }
            unsigned long long diff = 0;

            for (int i = 0; i <= CH_CARD + 1; i++) {
                diff += abs((long long) (10000 * cur_frame[i] / norma_cur -
                                         10000 * prev_frame[i] / norma_prev));
            }
            //csv_file << diff << ", ";
            //cout<<diff<<endl;
            /*if (diff > 5860) {
                for (int i = 0; i <= CH_CARD + 1; i++)
                 freq[i] = freq_high[i];

                STEP = 3;
            }
            else {
                //if(diff <  6000) {
                for (int i = 0; i <= CH_CARD + 1; i++)
                  freq[i] = freq_low[i];
                STEP = 2;
               // }
            }*/
        }
        if (orig_size_arifm % landmark == 0) {
            for (int i = 0; i <= CH_CARD + 1; i++) {
                prev_frame[i] = cur_frame[i];
                cur_frame[i] = 0;
            }
        }


        freq_low[symbol] += STEP;
        freq_high[symbol] += STEP;//only last frame
        cur_frame[symbol]++;
        freq[symbol] += STEP;
        freq[symbol]+=4*STEP;
        if(tabe_of_last !=300)
            freq[tabe_of_last]-=STEP;
        tabe_of_last = symbol;
        for (int idx = CH_CARD + 1, freq_acc = 0; idx >= 0; idx--) {
            probs[idx] = freq_acc;
            freq_acc += freq[idx];
        }
    }


}



void ArifmComp(unsigned Roboust, smart_ostream * smart_out, FILE* infile) {

    model_init(Roboust);
    unsigned ch;
    int symbol;

    smart_out->result_byte = 0;
    smart_out->masknum = 0;
    unsigned long long low = 0l;
    unsigned long long high = HIGH_MAX;
    smart_out->lasts = 0l;

    smart_out->output_bit(Roboust > 1);
    smart_out->output_bit(Roboust % 2);

    //needed to print EOF
    int previous_EOF = 0;
    while (true) {
        ch = fgetc(infile);
        if (ch == EOF) {
            previous_EOF = 1;
            symbol = CH_CARD + 1;
        } else symbol = ch+1;
        unsigned long long range;
        range = high - low + 1;
        high = low + (range * probs[symbol - 1]) / probs[0] - 1;
        low = low + (range * probs[symbol]) / probs[0];
        while (true) {
            if (low >= 2 * QTR) {
                low -= 2 * QTR;
                high -= 2 * QTR;
                smart_out->smartprint(1);
            } else if (high < 2 * QTR) {
                smart_out->smartprint(0);
            } else if ((high < 3 * QTR) && (low >= QTR)) {
                low -= QTR;
                high -= QTR;
                (smart_out->lasts)++;
            } else {
                break;
            }
            low <<= 1;
            high = (high << 1) + 1;//11..1 in the end
        }
        if (!previous_EOF)
        {
            model_upd(symbol, Roboust);
        }
        else
            break;
    }
    (smart_out->lasts)++;

    smart_out->smartprint(low >= QTR);
    putc(smart_out->result_byte >> (8 - smart_out->masknum), smart_out->outfile );
}


void ArifmDecomp(FILE *Outfile, smart_in *Smart_in)
{



    unsigned ch, symbol;
    unsigned long long range, cur_freq;

    Smart_in->masknum = 8;
    //garbage_bits = 0;
    unsigned i;

    unsigned long long value = 0l;
    //Roboustness check
    unsigned Roboust = Smart_in->input_bit();//input_bit();
    Roboust = (Roboust<< 1u) + Smart_in->input_bit() ;//input_bit();
    //printf("\nkind:   %d\n",Roboust );
    model_init(Roboust);
    for (i = 1; i <= 32; i++)
        value = 2 * value + Smart_in->input_bit();
    unsigned long long low = 0l;
    unsigned long long high = HIGH_MAX;
    while (true) {
        range = high - low + 1;
        cur_freq = ((value - low + 1) * probs[0] - 1) / range;
        for (symbol = 1; probs[symbol] > cur_freq; symbol++);//freq to symbol

        high = low + (range * probs[symbol - 1]) / probs[0] - 1;
        low = low + (range * probs[symbol]) / probs[0];

        while (true) {
            if (low >= (2 * QTR)) {
                value -= (2 * QTR);
                low -= (2 * QTR);
                high -= (2 * QTR);
            } else if (high < (2 * QTR));
            else if (low >= QTR && high < (3 * QTR)) {
                value -= QTR;
                low -= QTR;
                high -= QTR;
            } else
                break;
            low = 2 * low;
            high = 2 * high + 1;
            value = 2 * value + Smart_in->input_bit();
        }


        if (symbol == CH_CARD + 1)
            break;
        ch = symbol-1;
        putc(ch, Outfile);
        model_upd(symbol, Roboust);
    }
}



void compress_ari(char *inputfilename, char *outputfilename) {
    unsigned saved_num = 0;
    FILE *infile = fopen(inputfilename, "r+b");
    smart_ostream Smart_out = smart_ostream (outputfilename);

    Smart_out.output_size = 0;
    ArifmComp(0, &Smart_out,infile);
    fclose(Smart_out.outfile );
    fclose(infile);
    unsigned long long output_size_saved = Smart_out.output_size;

    infile = fopen(inputfilename, "r+b");
    Smart_out.outfile  = fopen(outputfilename, "w+b");
    Smart_out.output_size = 0;
    ArifmComp(1, &Smart_out, infile);
    fclose(infile);
    fclose(Smart_out.outfile );
    if (output_size_saved > Smart_out.output_size) {
        saved_num = 1;
        output_size_saved = Smart_out.output_size;
    }


    infile = fopen(inputfilename, "r+b");
    Smart_out.outfile  = fopen(outputfilename, "w+b");
    Smart_out.output_size = 0;
    ArifmComp(2, &Smart_out, infile);
    fclose(infile);
    fclose(Smart_out.outfile );
    if (output_size_saved > Smart_out.output_size) {
        saved_num = 2;
        output_size_saved = Smart_out.output_size;
    }

    infile = fopen(inputfilename, "r+b");
    Smart_out.outfile  = fopen(outputfilename, "w+b");
    Smart_out.output_size = 0;
    ArifmComp(3, &Smart_out, infile);
    fclose(infile);
    fclose(Smart_out.outfile );
    if (output_size_saved > Smart_out.output_size) {
        saved_num = 3;
        output_size_saved=Smart_out.output_size;
    }

    infile = fopen(inputfilename, "r+b");
    Smart_out.outfile = fopen(outputfilename, "w+b");
    ArifmComp(saved_num, &Smart_out, infile);
    /*for(int i = 0; i <= CH_CARD + 1; i++)
        cout <<" "<< freq[i]<<",";*/
    //cout << saved_num;

    fclose(infile);
}
/*
void compress_ari(char *inputfilename, char *outputfilename) {


    smart_ostream Smart_out = smart_ostream (outputfilename);


    infile = fopen(inputfilename, "r+b");
    Smart_out.outfile = fopen(outputfilename, "w+b");

    //  printf("\n%d\n",saved_num);
    ArifmComp(1, &Smart_out, infile);
    fclose(infile);
    fclose(Smart_out.outfile);




}
*/
void decompress_ari(char *inputfilename, char *outputfilename) {
    smart_in Smart_in = smart_in();
    Smart_in.infile = fopen(inputfilename, "r+b");
    FILE *outfile = fopen(outputfilename, "w+b");
    //printf("d");
    ArifmDecomp(outfile, &Smart_in);

    fclose(Smart_in.infile);
    fclose(outfile);
}
