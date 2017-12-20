# Enhance Connector for Defold

Setup
-----

Include the provided extension folder into your Defold project, or add this project as a [Defold library dependency](http://www.defold.com/manuals/libraries/). Open your `game.project` file and in the dependencies field under project add:

    https://github.com/enhance-co/defold-connector/archive/master.zip

Or point to the ZIP file of a [specific release](https://github.com/enhance-co/defold-connector/releases).

Interstitial Ads
----------------

Interstitial Ads are short static or video ads, usually shown between levels or when game is over.

1) Check whether any ad is available:

    is_ready = enhance.isInterstitialReady()

2) Show the ad:
    
    enhance.showInterstitialAd()

Note:
Interstitial ads will automatically interrupt the flow of your application until the user completes interaction with the ad.


Rewarded Ads
------------

Rewarded Ads are usually video ads which pay high CPM for the users who view them in return for an in-game reward.

1) Check whether any ad is available:

    is_ready = enhance.isRewardedAdReady()

2) Show the ad:

    enhance.showRewardedAd(on_reward_granted, on_reward_declined, on_reward_unavailable)

Note:
Rewarded ads will automatically interrupt the flow of your application until the user completes interaction with the ad.


Banner Ads
----------

Banner Ads are small sized ads displayed in the top or bottom of the app.

1) Check whether any ad is available:

    is_ready = enhance.isBannerAdReady(optionalPlacement)

2) Display the ad:

    enhance.showBannerAdWithPosition("position");

    Available positions:

    "top"
    "bottom"

3) Hide the ad:

    enhance.hideBannerAd()

Note:
You can also integrate Fixed Banner Ads through Enhance ZeroCode with no coding at all.


Offer Walls
-----------

Offer Walls show full screen of real world offers(e.g. surveys), usually with reward offered in return for a completion.

1) Check whether any offer wall is available:

    is_ready = enhance.isOfferwallReady()

2) Display the offer wall:

    enhance.showOfferwall()

Note:
Offer walls will automatically interrupt the flow of your application until the user completes interaction with the offer wall.


Special Offers
--------------

Special offers are real world offers(e.g. surveys). They are available through Enhance ZeroCode, but now you can also request them from code.

1) Check whether any special offer is available:

    is_ready = enhance.isSpecialOfferReady()

2) Display the special offer:

    enhance.showSpecialOffer()

Note:
Special Offers will automatically interrupt the flow of your application until the user completes interaction with the special offer.

Analytics
---------

Enhance can hook into various analytics networks. By default it will track sessions, users and retention.

1) Send a simple analytics event:

    enhance.logEvent("event_type")

2) Send a detailed analytics event:

    enhance.logEvent("event_type", "event_param_key", "event_param_value")


Local Notifications
-------------------

Local Notifications are reminders displayed on your mobile device's screen after the app becomes inactive.

1) Ask for local notifications permission:

    enhance.requestLocalNotificationPermission(on_permission_granted, on_permission_rejected)

2) Enable a new local notification:

    enhance.enableLocalNotification("title", "message", 5)

3) Disable the local notification:

    enhance.disableLocalNotification()


In-App Purchases
----------------

Enhance library provides a set of functions which help you to easily make use of different In-App Purchases SDKs in your application.

1) Check if In-App purchasing is currently supported:

    is_supported = enhance.iapIsSupported()

2) Start the purchase flow for the given product:

    enhance.attemptPurchase("product_name", on_purchase_success, on_purchase_failed)

3) Try to consume the given product from the user's inventory:

    enhance.consume("product_name", on_consume_success, on_consume_failed)

4) Check if item is already owned:

    enhance.isItemOwned("product_name")

5) Get the number of the given product that user owns, or 0 if none:

    count = enhance.getOwnedItemCount("product_name")

6) Get the string containing the localized display title:
    
    title = enhance.getDisplayTitle("product_name", "Default Title")

7) Get the string containing the localized display description:

    desc = enhance.getDisplayDescription("product_name", "Default Description")

8) Get the string containing the localized display price:

    price = enhance.getDisplayPrice("product_name", "Default Price")

9) Manually restore purchases and update the user's inventory, if applicable:

    enhance.manuallyRestorePurchases(on_restore_success, on_restore_failed)


Advanced Usage
--------------

Most of Enhance functions can accept additional parameters for advanced usage. For example, instead of simple enhance.isInterstitialReady() you can use enhance.isInterstitialReady("some_custom_placement").


Demo Project
--------------

For a full implementation example, please see the demo project which can be found in the 'demo_project' directory within the distribution package.

Please remember that none of these features will work properly before Enhancing your application on our website.
