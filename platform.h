/*
 * platform.h
 *
 *  Created on: Feb 26, 2016
 *      Author: eba
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <string>
#include <vector>

// These have nothing to do with ads or in-app billing, but I think they're useful to have anyway.
struct platform_info {
    std::string user_id;
    std::string platform_name;
    std::string device_name;
    std::string manufacturer;
    std::string os_version;
    std::string device_language;
};

struct item_info {
    std::string item_name;
    bool consumable;
};

struct item_inventory_info {
    std::string item_name;
    std::string price;
};

// The methods of the "Platform" class can be called from Orx's main thread
class Platform {
public:
  Platform(){};
  virtual ~Platform(){};
  virtual platform_info getPlatformInfo() = 0;

  // Request the platform to display an add. The platform will respond by calling SetAdRequestResult
  virtual void showAd() = 0;

  // Check whether the current user purchased "AdFree"
  virtual bool isAdFree() = 0;

  // Start the flow for purchasing "AdFree", if successful, adFreePurchased will be called
  virtual void purchaseAdFree() = 0;

  // Start the flow for purchasing a consumable item, if successful consumablePurchased will be called
  virtual void purchaseConsumable(item_info item) = 0;

  // Start the procedure to retreive information about the in-app billing item, which is, their prices
  // The platform will call reportInventory when the information is available
  virtual void queryInventory(std::vector<item_info> items) = 0;
};

Platform * GetPlatform(); // Access the current platform implementation

// The functions below will be called from a foreign thread!
void SetPlatform(Platform *); // This will typically happen before orxExecute

// Called at the end of an Ad display request, shown = true if an ad was indeed shown
void SetAdRequestResult(bool shown);

// The user went ahead and bought "AdFree"
void adFreePurchased();

// A consumable item has been successfully purchased. Note that whatever this function does, the item
// will be consumed as far as Google Play Store is concerned.
void consumablePurchased(item_info item);

// Called as response to a queryInventory request, this function essentially gives you the prices of items
// on the current device
void reportInventory(std::vector<item_inventory_info>);

#endif /* PLATFORM_H_ */
