#define DS_AP_IMPLEMENTATION
#include "../ds.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    ds_result result = DS_OK;

    ds_argparse_parser parser;
    ds_argparse_parser_init(&parser, "test", "test description", "0.0.1");

    ds_argparse_add_argument(&parser, (ds_argparse_options){
                                          .short_name = 'x',
                                          .long_name = "number",
                                          .description = "some number input",
                                          .type = ARGUMENT_TYPE_VALUE,
                                          .required = true,
                                      });
    ds_argparse_add_argument(&parser, (ds_argparse_options){
                                          .short_name = 'c',
                                          .long_name = "config",
                                          .description = "config file",
                                          .type = ARGUMENT_TYPE_POSITIONAL,
                                          .required = true,
                                      });
    ds_argparse_add_argument(&parser, (ds_argparse_options){
                                          .short_name = 'f',
                                          .long_name = "file",
                                          .description = "just a file",
                                          .type = ARGUMENT_TYPE_POSITIONAL,
                                          .required = false,
                                      });
    ds_argparse_add_argument(&parser, (ds_argparse_options){
                                          .short_name = 'o',
                                          .long_name = "output",
                                          .description = "optional output file",
                                          .type = ARGUMENT_TYPE_VALUE,
                                          .required = false,
                                      });
    ds_argparse_add_argument(&parser, (ds_argparse_options){
                                          .short_name = 'F',
                                          .long_name = "files",
                                          .description = "just some files",
                                          .type = ARGUMENT_TYPE_POSITIONAL_REST,
                                          .required = true,
                                      });
    ds_argparse_add_argument(&parser, (ds_argparse_options){
                                          .short_name = 'm',
                                          .long_name = "module",
                                          .description = "module name",
                                          .type = ARGUMENT_TYPE_VALUE_ARRAY,
                                          .required = false,
                                      });

    if (ds_argparse_parse(&parser, argc, argv) != DS_OK) {
        return_defer(DS_ERR);
    }

    char *number = ds_argparse_get_value(&parser, "number");
    char *file = ds_argparse_get_value(&parser, "file");

    ds_dynamic_array files;
    ds_argparse_get_values(&parser, "files", &files);

    ds_dynamic_array modules;
    if (ds_argparse_get_values(&parser, "module", &modules) != DS_OK) {
        DS_LOG_ERROR("Could not get values for module");
        return_defer(DS_ERR);
    }

    if (number) {
        printf("number: %s\n", number);
    }

    printf("file: %s\n", file);

    for (size_t i = 0; i < files.count; i++) {
        char *file = NULL;
        ds_dynamic_array_get(&files, i, &file);

        printf("file: %s\n", file);
    }

    for (size_t i = 0; i < modules.count; i++) {
        char *module = NULL;
        ds_dynamic_array_get(&modules, i, &module);

        printf("module: %s\n", module);
    }

defer:
    ds_argparse_parser_free(&parser);
    return result;
}
