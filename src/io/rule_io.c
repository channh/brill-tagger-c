#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "../rules/rules.h"
#include "../tagger/tags.h"
#include "rule_io.h"

#define MAX_TRIGGER_FN_STR_LEN 34
char *trigger_fns[13]= {
    "prev_tag_is",
    "next_tag_is",
    "prev_2_tag_is", //word two before is tagged X
    "next_2_tag_is",
    "prev_1_or_2_tag_is", //word one before or two before is tagged X
    "next_1_or_2_tag_is",
    "prev_1_or_2_or_3_tag_is",
    "next_1_or_2_or_3_tag_is",
    "prev_tag_is_x_and_next_tag_is_y",
    "prev_tag_is_x_and_next_2_tag_is_y", //word 2 before is tagged Y and word 1 before is tagged X
    "next_tag_is_x_and_prev_2_tag_is_y",
    "next_tag_is_x_and_next_2_tag_is_y",
    "prev_tag_is_x_and_prev_2_tag_is_y"
};
static void getfileinfo(FILE *file, size_t *numlines, size_t *numchars){
    char cur;
    *numlines = 0;
    *numchars = 0;
    while((cur = getc(file))!=EOF){
        *numchars+=1;
        if(cur == '\n')
            *numlines+=1;
    }
    fseek(file, 0, SEEK_SET);
}
rules_list_t * parse_rules_from_file(char * fp){
    FILE * file = fopen(fp, "r");
    if(file == NULL){
        printf("Could not open file '%s' (errno %d)\n", fp, errno);
        fclose(file);
        exit(EXIT_FAILURE);
    }
    ssize_t read;
    rules_list_t *list = 
        (rules_list_t*)malloc(sizeof(rules_list_t));
    size_t numlines = 0;
    size_t numchars = 0;
    getfileinfo(file, &numlines, &numchars);
    list->length = numlines;
    list->rules = 
        (contextual_rule_t*)malloc(sizeof(contextual_rule_t)*list->length);
    char contents[numchars];
    int cur = 0;
    fread(contents, numchars, 1, file);
    char *line;
    char *saveptr;
    for(int i = 0; i < list->length; i++){
        if(i == 0)
            line = strtok_r(contents, "\r\n", &saveptr);
        else 
            line = strtok_r(NULL, "\r\n", &saveptr);
        parse_contextual_rule(line, &list->rules[i]);
    }
    fclose(file);
    return list;
}
void parse_contextual_rule(char * rulestr, contextual_rule_t *rule){
    char *saveptr;
    char *delim = " \n\r";
    rule->tag1 = tag_to_hash(strtok_r(rulestr, delim, &saveptr));
    rule->tag2 = tag_to_hash(strtok_r(NULL, delim, &saveptr));
    rule->triggerfn = get_fn_number(strtok_r(NULL, delim, &saveptr)); 
        //^ index of the trigger function in the trigger function array
    rule->arg1 = tag_to_hash(strtok_r(NULL, delim, &saveptr));
    rule->arg2 = tag_to_hash(strtok_r(NULL, delim, &saveptr));
}
int get_fn_number(char *fnstr){
    for(int i = 0; i < 13; i++)
        if(strcmp(fnstr, trigger_fns[i]) == 0)
            return i;
    return -1;
}
void get_fn_string(int fn, char* fnstr){
    strncpy(fnstr, trigger_fns[fn], MAX_TRIGGER_FN_STR_LEN);
}

void print_rules_list(rules_list_t *list){
    char tag1[TAG_BUFFER_LENGTH];
    char tag2[TAG_BUFFER_LENGTH];
    char arg1[TAG_BUFFER_LENGTH];
    char arg2[TAG_BUFFER_LENGTH];
    char triggerfn[MAX_TRIGGER_FN_STR_LEN];
    contextual_rule_t *rules = list->rules;
    for(int i = 0; i < list->length; i++){
        hash_to_tag(rules[i].tag1, tag1);
        hash_to_tag(rules[i].tag2, tag2);
        hash_to_tag(rules[i].arg1, arg1);
        hash_to_tag(rules[i].arg2, arg2);
        get_fn_string(rules[i].triggerfn, triggerfn);
        printf("-------rules[%d]-------\n" 
            "tag1:     %s\n"
            "          %d\n"
            "tag2:     %s\n"
            "          %d\n"
            "fn:       %s\n"
            "          %d\n"
            "arg1:     %s\n"
            "          %d\n"
            "arg2:     %s\n"
            "          %d\n",
            i,
            tag1,
            rules[i].tag1, 
            tag2,
            rules[i].tag2, 
            triggerfn,
            rules[i].triggerfn,
            arg1,
            rules[i].arg1,
            arg2,
            rules[i].arg2);
    }
}
void free_rules_list(rules_list_t *list){
    free(list->rules);
    free(list);
}