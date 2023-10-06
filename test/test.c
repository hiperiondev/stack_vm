/*
 * @test.c
 *
 * @brief virtual machine
 * @details
 * This is based on other projects:
 *   Others (see individual files)
 *
 *   please contact their authors for more information.
 *
 * @author Emiliano Gonzalez (egonzalez . hiperion @ gmail . com))
 * @version 0.1
 * @date 2023
 * @copyright MIT License
 * @see https://github.com/hiperiondev/stack_vm
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "vm.h"

uint8_t test_pop(vm_t vm) {
    return 0;
}

uint8_t test_push(vm_t vm) {
    return 0;
}

uint8_t test_stack(vm_t vm) {
    return 0;
}

uint8_t test_goto(vm_t vm) {
    return 0;
}

uint8_t test_if(vm_t vm) {
    return 0;
}

uint8_t test_call(vm_t vm) {
    return 0;
}

uint8_t test_return(vm_t vm) {
    return 0;
}

uint8_t test_load(vm_t vm) {
    return 0;
}

uint8_t test_store(vm_t vm) {
    return 0;
}

uint8_t test_const(vm_t vm) {
    return 0;
}

uint8_t test_newarray(vm_t vm) {
    return 0;
}

uint8_t test_aload(vm_t vm) {
    return 0;
}

uint8_t test_astore(vm_t vm) {
    return 0;
}

uint8_t test_alu_arithmetic(vm_t vm) {
    return 0;
}

uint8_t test_alu_bitwise(vm_t vm) {
    return 0;
}

uint8_t test_convert(vm_t vm) {
    return 0;
}

uint8_t test_compare(vm_t vm) {
    return 0;
}

vm_image_t* vm_image_load(FILE *file) {
    vm_image_t *image;
    int j, fields;
    uint32_t byte, method_area_size, cpool_size, main_index;
    int32_t word;

    image = malloc(sizeof(vm_image_t));

    fields = fscanf(file, "main index: %d\n", &main_index);
    if (fields == 0) {
        printf("Bytecode file not recognized\n");
        exit(-1);
    }
    image->main_index = main_index;

    fields = fscanf(file, "method area: %d bytes\n", &method_area_size);
    if (fields == 0) {
        printf("Bytecode file not recognized\n");
        exit(-1);
    }

    image->method_area = malloc(method_area_size);
    image->method_area_size = method_area_size;
    for (j = 0; j < method_area_size; j++) {
        fscanf(file, "%x", &byte);
        image->method_area[j] = byte;
    }

    fields = fscanf(file, "\nconstant pool: %d words\n", &cpool_size);
    if (fields == 0) {
        printf("Bytecode file not recognized\n");
        exit(-1);
    }

    image->cpool = malloc(cpool_size * sizeof(int32_t));
    image->cpool_size = cpool_size;
    for (j = 0; j < cpool_size; j++) {
        fscanf(file, "%x", &word);
        image->cpool[j] = word;
    }

    return image;
}

int main(int argc, char *argv[]) {
    vm_error_t res = 0;
    FILE *file;
    vm_image_t *image;
    vm_t *i;

    if (argc < 2) {
        fprintf(stderr, "Usage: ijvm [OPTION] FILENAME [PARAMETERS ...]\n\n");
        fprintf(stderr, "Where OPTION is\n\n");
        fprintf(stderr, "  -f SPEC-FILE  The IJVM specification file to use.\n\n");
        fprintf(stderr, "If you pass `-' as the filename the simulator will read the bytecode\nfile from stdin.\n\n");
        fprintf(stderr, "You must specify as many arguments as your main method requires, except\n");
        fprintf(stderr, "one; the simulator will pass the initial object reference for you.\n");
        exit(-1);
    }

    if (strcmp(argv[1], "-") == 0)
        file = stdin;
    else
        file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Could not open bytecode file `%s'\n", argv[1]);
        exit(-1);
    }
    image = vm_image_load(file);
    fclose(file);
    i = vm_new(image, argc, argv);

    while (vm_active(i))
        if ((res = vm_step(i)) != VM_ERR_OK)
            break;

    printf("END STATUS: %d\n", res);

    return 0;
}
