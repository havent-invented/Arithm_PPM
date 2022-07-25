#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "utils.h"
#include "ari.h"
#include "ppm.h"

int main(int argc, char **argv) {
    CompressOptions *opts = parse_args(argc, argv);
    if (opts != NULL) {
        if (opts->mode == 'c') {
            if (opts->method == ARI) {
                compress_ari(opts->ifile, opts->ofile);
            }
            else if (opts->method == PPM) {
                compress_ppm(opts->ifile, opts->ofile);
            }
        }
        else if (opts->mode == 'd') {
            if (opts->method == ARI) {
                decompress_ari(opts->ifile, opts->ofile);
            }
            else if (opts->method == PPM) {
                decompress_ppm(opts->ifile, opts->ofile);
            }
        }
    }
    free_compress_opts(opts);
    return 0;
}
