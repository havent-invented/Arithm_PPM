#include <cstdlib>
#include <cstdio>

#include "ppm.h"


#include <map>
#include <cstdio>
#include <iostream>
#include <cassert>

#include <fstream>
#include <cstdio>

#ifdef __unix__
int unix_platform = 1;
#else
int unix_platform = 0;
#endif

using namespace std;

unsigned long long ROBUSTNESS_mas_bin[] = {500,310,450,280,590,5000}; //{560,310,333,280,590,5000};// {500,310,450,280,590,5000};
unsigned long long ROBUSTNESS_mas_rus[] = {600,400,300,300,300,800};
unsigned long long *ROBUSTNESS_mas  =ROBUSTNESS_mas_rus ;


enum : long long {
    HIGH_MAX = (((long long) 1 << 32) - 1),//MAX VALUE OF
    QTR = (HIGH_MAX / 4 + 1),
    CH_CARD = 256,//cardinality of the alphabet
    MODEL_RATE = 5,//maximum model rate
    //ROBUSTNESS_A = 147447,
    ROBUSTNESS_P = 600, //947for 600
};

int rate = MODEL_RATE;


unsigned debo = 0;

unsigned long long size_count=0 ;
unsigned long long orig_size=0 ;

uint16_t* CM0_freq;


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

    }
    void output_bit(int bit) {
        result_byte >>= 1u;
        if (bit)
            result_byte = (1u << 7u) | result_byte;
        masknum++;
        if (masknum == 8) {
            putc(result_byte, outfile);
            masknum = 0;
            size_count++;

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



//PPM zone
class contextModel {

public:
    uint16_t *probs;
    uint16_t *freq;
    int order;
    explicit contextModel(int lastModel, int model_order) {
        order = model_order;
        freq = new uint16_t[CH_CARD + 2];
        probs = new uint16_t[CH_CARD + 2];

        freq[0] = 0;
        for (unsigned i = 1; i <= CH_CARD + 1; i++)
            freq[i] = lastModel;

        freq[CH_CARD + 1] = 1;
        unsigned long long freq_acc = 0;
        for (int idx = CH_CARD + 1; idx >= 0; idx--) {
            probs[idx] = freq_acc;
            freq_acc += freq[idx];
        }
    }

    ~contextModel() {
        delete [] freq;
        delete [] probs;
    }

    void model_upd(unsigned symbol, bool Roboust, map<string, contextModel *> & PPMmodel) {
        unsigned long long Roboustness = ROBUSTNESS_P;

            if(orig_size == 256)
        {
            //unsigned long long diff_eng = CM0_freq[33]
            unsigned long long diff =CM0_freq[5]+ CM0_freq[33]+CM0_freq[209]+ CM0_freq[210];
            unsigned long long diff_eng = 2*CM0_freq[33] + CM0_freq[98] + CM0_freq[102] + CM0_freq[109]  + CM0_freq[11] + CM0_freq[116]  + CM0_freq[117];

            //cout<< endl<< endl<<diff_eng<< endl;

            //cout << " 5: "<<CM0_freq[5] <<" 33: "<<CM0_freq[33] <<" 209: " <<CM0_freq[209] << " 210: " << CM0_freq[210] <<endl;

            //cout<< diff<<endl;
            //exit(0);
            if(diff>26|| diff_eng >=73 )
            {
                ROBUSTNESS_mas = ROBUSTNESS_mas_rus;
            }
            else {
                ROBUSTNESS_mas = ROBUSTNESS_mas_bin;
            }
        }


        if(order >= 0)
        {
            Roboustness = ROBUSTNESS_mas[order];
        }

        // freq rescaling if needed
        if (probs[0] >= Roboustness) {//Roboustness less - more roubust for data structure changes
            unsigned long long freq_acc = 0;
            for (int idx = CH_CARD + 1; idx >= 0; idx--) {
                freq[idx] = (freq[idx] + 1) / 2;// ceil
                probs[idx] = freq_acc;
                freq_acc += freq[idx];
            }
        }

        for (freq[symbol]++; symbol > 0; probs[symbol]++)
            symbol--;

    }
};




contextModel *
findCM(int &rate, int &counted, string &cur_str,map<string, contextModel *> &PPMmodel) {

    contextModel *curCM;

    for (; rate >= -1; rate--) {

        if (counted < rate)
            continue;

        auto curCMpair = PPMmodel.find("000000");//just init

        if (rate == -1) {
            //curCMpair = PPMmodel.find("000000");
        } else {
            string substring = cur_str.substr(cur_str.length() - rate);
            curCMpair = PPMmodel.find(substring);
        }




        if (curCMpair == PPMmodel.end()) {
            auto CM_new = new contextModel(0, rate);
            string substring = cur_str.substr(cur_str.length() - rate);
            PPMmodel.insert(make_pair(substring, CM_new));
            curCM = CM_new;
        } else {
            curCM = curCMpair->second;
            break;
        }
    }

    return curCM;
}

void encode_sym(unsigned symbol, contextModel *curCM, unsigned long long &high, unsigned long long &low,
                int EOF_passed,
                unsigned *mask, smart_ostream * smart_out,map<string, contextModel *> & PPMmodel) {
    unsigned long long freqs_masked[258];
    unsigned long long probs_masked[258];
    unsigned long long freq_acc = 0;


    for (int idx = 257; idx >= 0; idx--) {
        freqs_masked[idx] = mask[idx] * curCM->freq[idx];
        probs_masked[idx] = freq_acc;
        freq_acc += mask[idx] *  curCM->freq[idx];
    }

    unsigned long long range = high - low + 1;;
    high = low + (range * probs_masked[symbol - 1]) / probs_masked[0] - 1;
    low = low + (range * probs_masked[symbol]) / probs_masked[0];

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
        } else
            break;
        low <<= 1;
        high = (high << 1) + 1;//11..1 in the end
    }
    if (!EOF_passed && symbol == CH_CARD + 1)
        curCM->model_upd(CH_CARD + 1, 0, PPMmodel);
}


void UPD_PPM(unsigned symbol, string full_str, unsigned counted,map<string, contextModel *> &PPMmodel) {
    string substring = full_str.substr(full_str.length() - counted + 1);
    for (int i = counted - 1; i >= 0; i--) {
        substring = full_str.substr(full_str.length() - i);
        contextModel *CM_underupd = PPMmodel[substring];
        CM_underupd->model_upd(symbol, 0, PPMmodel);
    }
}



void PPMcomp_n(smart_ostream * smart_out, FILE* infile) {
    map<string, contextModel *> PPMmodel;
    auto CMm1 = new contextModel(1, -1);
    auto CM0 = new contextModel(0,0);
    CM0_freq = CM0->freq;
    PPMmodel.insert(std::make_pair("000000", CMm1));
    PPMmodel.insert(std::make_pair("", CM0));
    string cur_str = "rndnw";//std::string cur_str = "randn";


    smart_out->result_byte = 0;
    smart_out->masknum = 0;
    smart_out->lasts = 0l;


    int counted = 0;
    int ch = -1;
    unsigned symbol;

    unsigned long long low = 0l;
    unsigned long long high = HIGH_MAX;
    int previous_EOF = 0;
    auto curCM = PPMmodel["000000"];

    while (previous_EOF == 0) {
        rate = MODEL_RATE;
        if (ch != -1) {
            cur_str.erase(0, 1);
            cur_str.push_back(ch);
        }
        ch = fgetc(infile);
        orig_size ++;
        if (ch == EOF) {
            previous_EOF = 1;
            symbol = CH_CARD + 1;
        } else {
            symbol = ch + 1;
        }

        curCM = findCM(rate, counted, cur_str,  PPMmodel);
        if (counted < MODEL_RATE)
            counted++;

        unsigned mask[290];
        for (int i = 0; i < 258; i++)//////mask to findCM
            mask[i] = 1;

        int escSym = 0;

        do {
            escSym = (curCM->freq[symbol] == 0) || previous_EOF;
            if (escSym && rate > -1)//esc
            {
                encode_sym(CH_CARD + 1, curCM, high, low, previous_EOF, mask, smart_out,PPMmodel );
                rate--;
                for (int i = 0; i < 257; i++) {
                    mask[i] = mask[i] && (curCM->freq[i] == 0);
                }
                curCM = findCM(rate, counted, cur_str, PPMmodel);
            } else {
                encode_sym(symbol, curCM, high, low, previous_EOF, mask, smart_out,PPMmodel);
                break;
            }
        } while (true);

        UPD_PPM(symbol, cur_str, counted, PPMmodel);
    }
    smart_out->encode_last(low);
}

unsigned decode_sym(contextModel *curCM, unsigned long long &high, unsigned long long &low,
                    unsigned long long &value, int EOF_passed,
                    unsigned *mask,smart_in *Smart_in, FILE * Outfile, map<string, contextModel *> & PPMmodel) {
    unsigned long long freqs_masked[258];
    unsigned long long probs_masked[258];
    unsigned long long freq_acc = 0;

    for (int idx = 257; idx >= 0; idx--) {
        freqs_masked[idx] = mask[idx] * curCM->freq[idx];
        probs_masked[idx] = freq_acc;
        freq_acc += mask[idx] * curCM->freq[idx];
    }//////////testing


    unsigned symbol;
    unsigned long long range = high - low + 1;

    unsigned long long cur_freq = ((value - low + 1) * probs_masked[0] - 1) / range;
    for (symbol = 1; probs_masked[symbol] > cur_freq; symbol++);//freq to symbol

    high = low + (range * probs_masked[symbol - 1]) / probs_masked[0] - 1;
    low = low + (range * probs_masked[symbol]) / probs_masked[0];

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


    int ch = symbol - 1;
    if (symbol != CH_CARD + 1) {
        putc(ch, Outfile);
        orig_size ++;

    } else {
        if (!EOF_passed)
            curCM->model_upd(CH_CARD + 1, 0, PPMmodel);
    }

    return symbol;
}


void PPMdecomp(FILE *Outfile, smart_in *Smart_in) {
    map<string, contextModel *> PPMmodel;
    Smart_in->masknum = 8;
    unsigned long long range, cur_freq;

    unsigned long long value = 0l;

    unsigned long long low = 0l;
    unsigned long long high = HIGH_MAX;

    auto CMm1 = new contextModel(1,-1);
    auto CM0 = new contextModel(0,0);
    CM0_freq = CM0->freq;
    PPMmodel.insert(std::make_pair("000000", CMm1));
    PPMmodel.insert(std::make_pair("", CM0));
    string cur_str = "rndnw";//init string
    int previous_EOF = 0;
    int counted = 0;
    int ch = -1;
    unsigned symbol;


    for (unsigned idx = 1; idx <= 32; idx++) {
        value = 2 * value + Smart_in->input_bit();
    }



    auto curCM = PPMmodel["000000"];

    rate = MODEL_RATE;
    unsigned mask[258];
    for (int i = 0; i < 258; i++)//////mask to findCM
        mask[i] = 1;

    while ( !previous_EOF ) {
        if (ch != -1) {
            cur_str.erase(0, 1);
            cur_str.push_back(ch);
        }

        curCM = findCM(rate, counted, cur_str, PPMmodel);
        symbol = decode_sym(curCM, high, low, value, previous_EOF, mask,Smart_in, Outfile, PPMmodel);

        if (rate == -1 && symbol == CH_CARD + 1) {
            previous_EOF = 1;
            break;
        }

        if (symbol == CH_CARD + 1) {
            ch = EOF;
        } else {
            ch = symbol - 1;
        }

        if (!previous_EOF && symbol == CH_CARD + 1) {
            rate--;
            for (int i = 0; i < 257; i++) {
                mask[i] = mask[i] && (curCM->freq[i] == 0);
            }
        } else {
            if (counted < MODEL_RATE)
                counted++;
            for (int i = 0; i < 258; i++)//mask init
                mask[i] = 1;
            UPD_PPM(symbol, cur_str, counted,  PPMmodel);
            rate = MODEL_RATE;
        }
    }
}







void compress_ppm(char *inputfilename, char *outputfilename) {


    smart_ostream Smart_out = smart_ostream(outputfilename);
    size_count = 0;
    FILE * infile = fopen(inputfilename,"r+b");
    PPMcomp_n(&Smart_out,infile);
    fclose(infile);

}

void decompress_ppm(char *inputfilename, char *outputfilename) {

    smart_in Smart_in = smart_in();
    Smart_in.infile = fopen(inputfilename, "r+b");
    FILE *outfile = fopen(outputfilename, "w+b");
    size_count = 0;
    PPMdecomp(outfile, &Smart_in);
    fclose(Smart_in.infile);
    fclose(outfile);
}