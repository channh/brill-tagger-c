# brill-tagger-c
brill tagger in c

The current state of the Brill Tagger implements a hash map to store words as keys and a structure of possible tags for each word (CLAWSET 7). Lines of a corpus are parsed and the word is added to the map with the initial CLAWSET 7 structure updated accordingly. For example, a line that contains "school APPGE" would first look for the word school in the hashmap, if it exists, it will increment the APPGE variable in the keyword's corresponding structure by 1. To save time, string comparisons of keywords corresponding tags are customly hashed and an enumeration containing all of the tags hashed values are used as cases in a switch statement to increment the correct index.

This is the starting dictionary. After all tag frequencies are calculated, the program may begin initial tagging by taking in a word, locating it in the hashmap, and labeling it with the highest numberd tag in the CLAWSET 7 tag structure. The main reason for hashing tag values to update them is this, integer comparison works on the machine level in one cycle. String comparison would have been faster to write, but in the worst case must check all values of the string (char by char), thus resulting in a far slower program. 
