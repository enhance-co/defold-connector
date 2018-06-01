# Enhance Drag & Drop Library by Enhance, Inc.
# https://enhance.co/documentation
# 
# DEFOLD

Setup
-----

Move the provided extension folder (enhance_extension) into your Defold project. Make sure it is visible in the Project Explorer.


Interstitial Ads
----------------

Interstitial Ads are short static or video ads, usually shown between levels or when game is over. 

Example Usage:

if enhance.isInterstitialReady() then
    -- The ad is ready, show it
    enhance.showInterstitialAd()
end

Methods:

boolean enhance.isInterstitialReady(
    string placement = "default"
)

Check if an interstitial ad from any of the included SDK providers is ready to be shown.
Placement is optional and specifies an internal placement (from the Enhance mediation editor). 
Returns true if any ad is ready, false otherwise.


nil enhance.showInterstitialAd(
    string placement = "default"
)

Display a new interstitial ad if any is currently available. The ad provider is selected based on your app's mediation settings. 
Placement is an optional internal placement (from the Enhance mediation editor).
Returns nothing.


Rewarded Ads
------------

Rewarded Ads are usually full-screen video ads which users can view to receive a reward inside the application, like an additional in-game currency or a health bonus.

Example Usage:

function show_ad()
    if enhance.isRewardedAdReady() then
        enhance.showRewardedAd(on_reward_granted, on_reward_declined, on_reward_unavailable)
    end
end

function on_reward_granted(self, reward_value, reward_type)
    if reward_type == "REWARDTYPE_ITEM" then
        write_log("Reward granted (item)")

    elseif reward_type == "REWARDTYPE_COINS" then
        write_log("Reward granted (coins), value: " .. reward_value)
    end
end

function on_reward_declined()
    write_log("Reward declined")
end

function on_reward_unavailable()
    write_log("Reward unavailable")
end

Methods:

boolean enhance.isRewardedAdReady(
    string placement = "neutral"
)

Check if a rewarded ad from any of the included SDK providers is ready to be shown.
Placement is an optional internal placement (from the Enhance mediation editor).
Returns true if any ad is ready, false otherwise.


nil enhance.showRewardedAd(
    string placement = "neutral"
    function onRewardGrantedCallback,
    function onRewardDeclinedCallback,
    function onRewardUnavailableCallback
)

Display a new rewarded ad if any is currently available. The ad provider is selected based on your app's mediation settings.
Callbacks specify functions which are invoked when reward is granted, declined or unavailable.
Placement is an optional internal placement (from the Enhance mediation editor).
Returns nothing.

Members:

"REWARDTYPE_ITEM"
The granted reward is a game-defined item.

"REWARDTYPE_COINS"
The granted reward is a specified number of coins.


Banner Ads
----------

Banner Ads are small sized ads displayed on the screen as a rectangle filled with content without interrupting the flow of the app.

Example Usage:

-- Toggle the banner ad

if isBannerVisible then
    enhance.hideBannerAd()
    isBannerVisible = false

elseif enhance.isBannerAdReady() then
    enhance.showBannerAdWithPosition("top")
    isBannerVisible = true
end

Methods:

boolean enhance.isBannerAdReady(
    string placement = "default"
)

Check if a banner ad from any of the included SDK providers is ready to be shown.
Placement is an optional internal placement (from the Enhance mediation editor).
Returns true if any ad is ready, false otherwise.


nil enhance.showBannerAdWithPosition(
    string placement = "default",
    string position = "bottom"
)

Display a new banner ad if any is currently available. The ad provider is selected based on your app's mediation settings.
Position specifies the position of the ad on the screen. Use “top” or “bottom”.
Placement is an optional internal placement (from the Enhance mediation editor).
Returns nothing.


nil enhance.hideBannerAd()

Hide the banner ad which is currently visible, if any.
Returns nothing.


Offer Walls
-----------

Example usage:

function init(self)
    enhance.setReceivedCurrencyCallback(on_currency_received)
end

function on_currency_received(self, amount)
    write_log("Currency received: " .. amount)
end

function showOfferwall()
    if enhance.isOfferwallReady() then
        enhance.showOfferwall()
    end
end

Methods:

nil enhance.setReceivedCurrencyCallback(
    function onCurrencyReceivedCallback
)

Specify the function which is called every time the user receives a reward from any offerwall. We recommend that you use this function at the beginning of your app’s logic to be sure the callback is ready as soon as an offerwall sends the reward. This could happen at different times, even right after your app starts! As a parameter, this callback will receive an amount of the granted currency (number).
Returns nothing.


boolean enhance.isOfferwallReady(
    string placement = "default"
)

Check if an offerwall from any of the included SDK providers is ready to be shown.
Placement is an optional internal placement (from the Enhance mediation editor).
Returns true if any offerwall is ready, false otherwise.


nil enhance.showOfferwall(
    string placement = "default"
)

Display a new offerwall if any is currently available. The offerwall provider is selected based on your app's mediation settings.
Placement is an optional internal placement (from the Enhance mediation editor).
Returns nothing.


Special Offers
--------------

Special offers are real world offers (e.g. surveys). They are available through Enhance ZeroCode, but you can also request them manually from code.

Example Usage:

if enhance.isSpecialOfferReady() then
    -- The offer is ready, show it
    enhance.showSpecialOffer()
end

Methods:

boolean enhance.isSpecialOfferReady(
    string placement = "default"
)

Check if a special offer from any of the included SDK providers is ready to be shown.
Placement is an optional internal placement (from the Enhance mediation editor).
Returns true if any offer is ready, false otherwise.


nil enhance.showSpecialOffer(
    string placement = "default"
)

Display a new special offer if any is currently available. The offer provider is selected based on your app's mediation settings.
Placement is an optional internal placement (from the Enhance mediation editor).
Returns nothing.


Analytics
---------

The connector library allows you to send custom events to the hooked analytics networks.

Example Usage:

enhance.logEvent("game_over", "level", "1");
enhance.logEvent("user_exit");

Methods:

nil Enhance.logEvent(
    string eventType,
    string paramKey = nil,
    string paramValue = nil
)

Send an event with an additional parameter (optional).


Local Notifications
-------------------

Local Notifications are reminders which show up on your screen after the app becomes inactive for a specific amount of time.

Example Usage:

function on_permission_granted()
    enhance.enableLocalNotification("Game", "Play me!", 60)
end

function on_permission_refused()
    -- Failure
end

enhance.requestLocalNotificationPermission(on_permission_granted, on_permission_refused)

Methods:

nil enhance.requestLocalNotificationPermission(
    function onPermissionGrantedCallback,
    function onPermissionRefusedCallback
)

Request a permission from the user to show local notifications. This won't have any effect on Android devices as you don't need a permission to schedule local notifications there (onPermissionGrantedCallback will be still fired).
Returns nothing.


nil enhance.enableLocalNotification(
    string title,
    string message,
    number delay
)

Schedule a new local notification, if possible. The notification will persist until you disable it manually. For example, if you set a notification for 60 seconds, it will invoke this notification 60 seconds after the app is closed every time.
Returns nothing.


nil enhance.disableLocalNotification()

Disable any local notification that was previously enabled.


In-App Purchases
-------------------

The connector library provides a set of functions which help you to make use of different In-App Purchases SDKs in your application.

Example Usage:

function on_purchase_success()
    local price = enhance.getDisplayPrice("my_product", "$5")

    local title = enhance.getDisplayTitle("my_product", "My Product")

    local desc = enhance.getDisplayDescription("my_product", "Some useful item.")
end

function on_purchase_failed()
    // Failure
end

function purchase()
    if enhance.iapIsSupported() then
        enhance.attemptPurchase("my_product", on_purchase_success, on_purchase_failed)
    end
end

Methods:

boolean enhance.iapIsSupported()

Check if the In-App Purchasing is currently available in your application.
Returns true if purchasing is available, false otherwise.


nil enhance.attemptPurchase(
    string productName,
    function onPurchaseSuccessCallback,
    function onPurchaseFailedCallback
)

Start the purchase flow for the given product.
Product name is the the reference name which you entered during the Enhance flow. 
Callbacks specify functions which are invoked when purchase is successful or not.
Returns nothing.


nil enhance.consume(
    string productName,
    function onConsumeSuccessCallback,
    function onConsumeFailedCallback
)

Consume the given product, if applicable (depends on the SDK provider).
Product name is the the reference name which you entered during the Enhance flow.
Callbacks specify functions which are invoked when consume is successful or not.
Returns nothing.


boolean enhance.isItemOwned(
    string productName
)


Check if the given product is already owned. The result may be inaccurate, depending on whether the SDK provider stores the information about your products or not.
Product name is the the reference name which you entered during the Enhance flow.
Returns true if the item is owned, false otherwise.


number enhance.getOwnedItemCount(
    string productName
)

Get a number of the given product that user owns, or 0 if none. The result may be inaccurate, depending on whether the SDK provider stores the information about your products or not.
Product name is the the reference name which you entered during the Enhance flow.
Returns a number of the given product copies.


nil enhance.manuallyRestorePurchases(
    function onRestoreSuccessCallback,
    function onRestoreFailedCallback
)

Manually restore purchases and update the user's inventory, if applicable (availability of this feature depends on the SDK provider).
Callbacks specify functions which are invoked when restore is successful or not.
Returns nothing.


string enhance.getDisplayPrice(
    string productName,
    string defaultPrice
)

Get a localized display price of the given product, for example: "100zł", "100¥".
Product name is the the reference name which you entered during the Enhance flow.
Default price will be used if a real one can't be fetched. 
Returns a string containing the localized display price.


string enhance.getDisplayTitle(
    string productName,
    string defaultTitle
)

Get a localized display title of the given product.
Product name is the the reference name which you entered during the Enhance flow.
Default title will be used if a real one can't be fetched.
Returns a string containing the localized display title.


string enhance.getDisplayDescription(
    string productName,
    string defaultDescription
)

Get a localized display description of the given product.
Product name is the the reference name which you entered during the Enhance flow.
Default description will be used if a real one can't be fetched.
Returns a string containing the localized display description.

Demo Project
--------------

For a full implementation example, please see the demo project which can be found in the 'demo_project' directory within the distribution package.

