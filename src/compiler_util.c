#include "compiler_util.h"

#include <stdio.h>
#include <math.h>

void errloc(char* buf, u32 index, u32 line) {
    // Find the bounds of the line
    u32 start = index;
    u32 end = index;
    while (buf + start != buf
        && *(buf + start) != '\n')
        start--;

    while (*(buf + end) != '\n'
        && *(buf + end) != '\0')
        end++;
    // Get the line before for some more context too
    u32 preStart = start != 0 ? start - 1 : start;
    while (buf + preStart != buf
        && *(buf + preStart) != '\n')
        preStart--;

    // Calculate the number of characters to render the line number + 1
    i32 margin = floor(log10(line)) + 2;
    i32 l1Pad = margin - (floor(log10(line - 1)) + 2);


    printf("%*s%u|%.*s\n", l1Pad, "", line - 1, start - preStart - 1, buf + preStart + 1);
    printf("%u|%.*s\n", line, end - start - 1, buf + start + 1);
    printf("%*s^ HERE\n", margin + (index - start - 1), "");
}
