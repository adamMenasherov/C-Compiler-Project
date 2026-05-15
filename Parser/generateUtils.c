#include "generateUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int currGlobalInt = 0;

char* generateResultVarName() {
    char* var_name = malloc(strlen("result.") + 10);
    if (!var_name) return NULL;
    sprintf(var_name, "result.%d", currGlobalInt++);
    return var_name;
}

char* generateTempName() {
    char* temp_name = malloc(strlen("tmp.") + 10);
    if (!temp_name) return NULL;
    sprintf(temp_name, "tmp.%d", currGlobalInt++);
    return temp_name;
}

char* generateLoopName() {
    char* loop_name = malloc(strlen("loop") + 10);
    if (!loop_name) return NULL;
    sprintf(loop_name, "loop%d", currGlobalInt++);
    return loop_name;
}

char* generateBreakLabelFromLoopLabel(char* loopLabel) {
    char* label = malloc(strlen("break_") + strlen(loopLabel) + 1);
    if (!label) return NULL;
    sprintf(label, "break_%s", loopLabel);
    return label;
}

char* generateContinueLabelFromLoopLabel(char* loopLabel) {
    char* label = malloc(strlen("continue_") + strlen(loopLabel) + 1);
    if (!label) return NULL;
    sprintf(label, "continue_%s", loopLabel);
    return label;
}

char* generateFalseLabel() {
    static int falseCount = 0;
    char* label = malloc(strlen("false_label_") + 10);
    if (!label) return NULL;
    sprintf(label, "false_label_%d", falseCount++);
    return label;
}

char* generateTrueLabel() {
    static int trueCount = 0;
    char* label = malloc(strlen("true_label_") + 10);
    if (!label) return NULL;
    sprintf(label, "true_label_%d", trueCount++);
    return label;
}

char* generateEndLabel() {
    static int endCount = 0;
    char* label = malloc(strlen("end_label_") + 10);
    if (!label) return NULL;
    sprintf(label, "end_label_%d", endCount++);
    return label;
}

char* generateE2Label() {
    static int e2Count = 0;
    char* label = malloc(strlen("e2_label_") + 10);
    if (!label) return NULL;
    sprintf(label, "e2_label_%d", e2Count++);
    return label;
}

char* generateElseLabel() {
    static int elseCount = 0;
    char* label = malloc(strlen("else_label_") + 10);
    if (!label) return NULL;
    sprintf(label, "else_label_%d", elseCount++);
    return label;
}

char* generateDefaultLabel() {
    static int defaultCount = 0;
    char* label = malloc(strlen("default_label_") + 10);
    if (!label) return NULL;
    sprintf(label, "default_label_%d", defaultCount++);
    return label;
}

char* generateCaseLabel(int caseValue) {
    char* label = malloc(strlen("case_") + 24);
    if (!label) return NULL;
    sprintf(label, "case_%d_%d", caseValue, currGlobalInt++);
    return label;
}