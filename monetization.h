/*
 * monetization.h
 *
 *  Created on: Mar 19, 2016
 *      Author: eba
 */

#ifndef MONETIZATION_H_
#define MONETIZATION_H_

// call this from game init
void init_monetization();

// call this from game update
void update_monetization();

// call this from game exit
void exit_monetization();

// Somewhere else in your game, you call this function to ask permission to do something.
// This lets the monetization subsystem to get a chance to break the game flow and show an add.
// Call this function at each frame until it returns true
bool RequestTransition();

// Call this function from the game to start an adfree purchase
void purchaseAdFree();

// Call this function from the game to start a consumable purchase
void purchaseConsumable(const char * item_name);

#endif /* MONETIZATION_H_ */
