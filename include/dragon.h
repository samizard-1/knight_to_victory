#ifndef DRAGON_H
#define DRAGON_H

#include "monster.h"

// Dragon-specific customizations
void dragon_apply_customizations(Monster *dragon);

// Custom dragon heart drawing function
void dragon_draw_hearts(Monster *dragon, float screen_pos_x, float screen_pos_y);

#endif // DRAGON_H
