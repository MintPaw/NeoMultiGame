#ifndef UTIL_H
#define UTIL_H

unsigned int prng();
#ifdef __cplusplus
extern "C" void prng_seed(unsigned int seed);
#else
void prng_seed(unsigned int seed);
#endif
char *tokenize(char *str, const char *delim, char **key);
char *strip(char *str);
int starts_with(const char *str, const char *prefix);
double now();

#endif
