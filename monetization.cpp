/*
 * monetization.cpp
 *
 *  Created on: Mar 19, 2016
 *      Author: eba
 */

#include <atomic>
#include <mutex>
#include <vector>
#include <memory>

#include <orx.h>

#include <monetization.h>
#include <platform.h>

enum AdRequestStatus {NO_REQUEST, ONGOING_REQUEST, REQUEST_SUCCESSFUL, REQUEST_FAILED};
std::atomic<AdRequestStatus> ad_request_status;
orxU64 last_display;

struct monetization_state {

    // Used to signal that the platform said something, this is not strictly necessary
    // but without this the update function would do a lot of unnecessary work
    std::atomic<bool> something_new;

    // For thread-safe use of this data structure
    std::mutex mut;

    std::vector<item_info> purchased_items;
    std::vector<item_inventory_info> inventory_info;
};

// Our global monetization state pointer
std::unique_ptr<monetization_state> shared_monetization_state;

void SetAdRequestResult(bool shown) {
    ad_request_status = shown ? REQUEST_SUCCESSFUL : REQUEST_FAILED;
    if(shown) {/* Maybe send an analytics event, or log */}
}

void adFreePurchased() {
    /* Maybe send an analytics event, or log */
}

// Called by the platform to indicate that a consumable item has been purchased
// Note that we can't do much here other than noting the fact since this call isn't
// happening on Orx's main thread
void consumablePurchased(item_info item) {
    std::lock_guard<std::mutex> guard(shared_monetization_state->mut);
    shared_monetization_state->something_new = true;
    shared_monetization_state->purchased_items.push_back(item);
}

// Called by the platform to report the result of an inventory information request
// Note that we can't do much here other than noting the fact since this call isn't
// happening on Orx's main thread
void reportInventory(std::vector<item_inventory_info> inventory) {
    std::lock_guard<std::mutex> guard(shared_monetization_state->mut);
    shared_monetization_state->something_new = true;
    for(auto & item: inventory) shared_monetization_state->inventory_info.push_back(item);
}

// Call this from game init
void init_monetization() {
    shared_monetization_state.reset(new monetization_state);
    ad_request_status = NO_REQUEST;
    last_display = orxSystem_GetRealTime();
    std::vector<item_info> items;

    // Note that adfree should be marked as not consumable.
    items.push_back({"adfree", false});

    items.push_back(...); // Intentional compiler error: you should enumerate any other in-app-billing items here

    // Start the flow for retreiving the inventory information
    GetPlatform()->queryInventory(items);
}

// This is happening on the Orx main thread, you now have the inventory information
// Use it however you want
void apply_inventory_info() {
    std::vector<item_inventory_info> inventory;
    {
        std::lock_guard<std::mutex> guard(shared_monetization_state->mut);
        std::swap(inventory, shared_monetization_state->inventory_info);
    }

    for(auto & item: inventory) {

        // Do something with item.price, like record it in config or display it somehow?
        IntentionalCompilerError!!!
    }
}

// This is happening on the Orx main thread, you now have a list of consumable items purchased
// you should apply those items to the game, like extra lives? whatever.
// Note that adfree is not a consumable item, it's never consumed.
void apply_purchased_items() {
    std::vector<item_info> purchased_items;
    {
        std::lock_guard<std::mutex> guard(shared_monetization_state->mut);
        std::swap(purchased_items, shared_monetization_state->purchased_items);
    }

    for(auto item: purchased_items) {

        // Do something based on the item consumed
        IntentionalCompilerError!!!
    }
}

// call this from game update
void update_monetization() {
   
    AdRequestStatus curstatus = ad_request_status;
    if(curstatus == REQUEST_SUCCESSFUL) {

        // This information is used by RequestTransition
        last_display = orxSystem_GetRealTime();

        ad_request_status = NO_REQUEST;
    }

    // Check if something important happened on the platform thread
    if(shared_monetization_state->something_new) {

        // This is thread safe, since even if something else has JUST happened, 
        // we're just about to process it anyway
        shared_monetization_state->something_new = false;

        apply_purchased_items();
        apply_inventory_info();
    }
}

// call this from game exit
void exit_monetization() {
}

// Somewhere else in your game, you call this function to ask permission to do something.
// This lets the monetization subsystem to get a chance to break the game flow and show an add.
// Call this function at each frame until it returns true
bool RequestTransition() {

    if(GetPlatform()->isAdFree()) return true; // Never show ads when adfree

    if(ad_request_status == ONGOING_REQUEST) return false; // We're waiting for an ongoing request

    orxFLOAT ad_display_period = // Set this value for a minimum period between ad display. 

    auto curtime = orxSystem_GetRealTime();
    if(curtime - last_display < ad_display period) return true; // It's too soon to show another Ad

    // Start a new request and wait until it's completed
    ad_request_status = ONGOING_REQUEST;
    GetPlatform()->showAd();
    return false;
}

// The game calls this function to start an adfree purchase
void purchaseAdFree() {
    GetPlatform()->purchaseAdFree();
}

// The game calls this function to start a consumable purchase
void purchaseConsumable(const char* item_name) {
    GetPlatform()->purchaseConsumable(item_info{item_name});
}

