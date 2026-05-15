#pragma once

extern int currGlobalInt;

char* generateTempName();
char* generateFalseLabel();
char* generateTrueLabel();
char* generateLoopName();
char* generateBreakLabelFromLoopLabel(char* loopLabel);
char* generateContinueLabelFromLoopLabel(char* loopLabel);
char* generateEndLabel();
char* generateE2Label();
char* generateElseLabel();
char* generateDefaultLabel();
char* generateCaseLabel(int caseValue);
char* generateResultVarName();