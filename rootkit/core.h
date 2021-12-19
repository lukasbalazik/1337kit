#ifndef CORE_H
#define CORE_H

#define DECRYPT(in, key) ({                             \
    int i;                                              \
    char *output;                                       \
    output = (char *) kmalloc(strlen(in), GFP_KERNEL);  \
    if (!strlen(key))                                   \
      strncpy(output, in, strlen(in));                  \
    for (i = 0; i < strlen(in); ++i)                    \
      output[i] = in[i] ^ key[i % strlen(key)];         \
    output;                                             \
})


#endif
